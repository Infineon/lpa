/*
 * (c) 2025, Infineon Technologies AG, or an affiliate of Infineon
 * Technologies AG. All rights reserved.
 * This software, associated documentation and materials ("Software") is
 * owned by Infineon Technologies AG or one of its affiliates ("Infineon")
 * and is protected by and subject to worldwide patent protection, worldwide
 * copyright laws, and international treaty provisions. Therefore, you may use
 * this Software only as provided in the license agreement accompanying the
 * software package from which you obtained this Software. If no license
 * agreement applies, then any use, reproduction, modification, translation, or
 * compilation of this Software is prohibited without the express written
 * permission of Infineon.
 * 
 * Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
 * IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
 * THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
 * SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
 * Infineon reserves the right to make changes to the Software without notice.
 * You are responsible for properly designing, programming, and testing the
 * functionality and safety of your intended application of the Software, as
 * well as complying with any legal requirements related to its use. Infineon
 * does not guarantee that the Software will be free from intrusion, data theft
 * or loss, or other breaches ("Security Breaches"), and Infineon shall have
 * no liability arising out of any Security Breaches. Unless otherwise
 * explicitly approved by Infineon, the Software may not be used in any
 * application where a failure of the Product or any consequences of the use
 * thereof can reasonably be expected to result in personal injury.
 */

/**
* @file cy_whd_tls_api.c
* @brief WHD MQTT TLS Keepalive Offload API
*/

#include "whd_types.h"
#include "whd_wifi_api.h"
#include "cy_lpa_wifi_ol_common.h"
#include "cy_whd_tls_api.h"
#include "cy_lpa_wifi_tls_ol.h"
#include "cy_lpa_wifi_ol.h"
#include "whd_cdc_bdc.h"
#include "whd_network_types.h"
#include "whd_buffer_api.h"
#include "cy_nw_helper.h"
#include "whd_wlioctl.h"
#include "cy_wcm.h"
#ifdef COMPONENT_LWIP
#include "lwip/tcp.h"
#endif
#ifdef COMPONENT_NETXDUO
#include "nx_ipv4.h"
#include "nx_api.h"
#include "cy_network_mw_core.h"
#include "nx_secure_tls.h"
#endif
#include "ip4string.h"
#include "cy_secure_sockets.h"
#include "cycfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************
*
*  Defines
*
********************************************************************/
#ifdef CYCFG_WIFI_MQTT_OL_SUPPORT
#ifdef TLSOE_OL_DEBUG
#define TLSOE_ERROR_PRINTF(x) printf x
#define TLSOE_DEBUG_PRINTF(x) printf x
#else
#define TLSOE_ERROR_PRINTF(x)
#define TLSOE_DEBUG_PRINTF(x)
#endif

/********************************************************************
 *
 *  Structures
 *
 *********************************************************************/
#ifdef CYCFG_WIFI_MQTT_OL_SUPPORT
static uint8_t tcp_reset;
static const whd_event_num_t tlsoe_events[]   = { WLC_E_WAKE_EVENT, WLC_E_NONE };
static uint16_t tlsoe_offload_update_entry = 0xFF;
void cylpa_on_emac_activity(bool is_tx_activity);
extern cy_mutex_t cy_lp_mutex;

extern whd_result_t whd_set_wowl_pattern(whd_interface_t ifp, char* opt, uint32_t offset, uint8_t mask_size,
                   uint8_t * mask, uint8_t pattern_size, uint8_t * pattern, uint8_t rule);
extern whd_result_t whd_get_wowl_cap(whd_interface_t ifp, uint32_t *value);
extern whd_result_t whd_set_wowl_cap(whd_interface_t ifp, uint32_t value);
extern whd_result_t whd_wowl_get_secure_session_status(whd_interface_t ifp, secure_sess_info_t *tls_sess_info);
extern whd_result_t whd_wowl_clear(whd_interface_t ifp);
extern whd_result_t whd_wowl_activate_secure(whd_interface_t ifp, tls_param_info_t *tlsparam);
#endif
#ifdef COMPONENT_LWIP
ssize_t etharp_find_addr(struct netif *netif, const ip4_addr_t *ipaddr, wl_ether_addr_t **eth_ret,
                         const ip4_addr_t **ip_ret);
#endif
/*********************************************************************************************
*
*  Forward Declarations
*
*********************************************************************************************/
/* Given a pair of port, ip addresses, sequence numbers and ack numbers
 * update the right tcp_pcb rcv and send low level info and make the host network stack tcp keep sync with wlan tcp offload
 */
whd_result_t whd_tlsoe_update_sock_seq( uint16_t local_port, uint16_t remote_port, const char *remote_ip, uint32_t seq_num, uint32_t ack_num, uint8_t reset )
{
    whd_result_t ret = WHD_SUCCESS;
#if LWIP_TCP
    extern struct tcp_pcb *tcp_bound_pcbs;
    extern struct tcp_pcb *tcp_active_pcbs;
    extern struct tcp_pcb *tcp_tw_pcbs;
    struct tcp_pcb **const my_tcp_pcb_lists[] = { &tcp_bound_pcbs, &tcp_active_pcbs, &tcp_tw_pcbs };
    int i;
    struct tcp_pcb *list;
    ip4_addr_t ip_addr;

    ip4addr_aton( remote_ip, &ip_addr );

    for  ( i = 0; i < (int)( sizeof( my_tcp_pcb_lists ) / sizeof( my_tcp_pcb_lists[0] ) ); i++ )
    {
        list = *my_tcp_pcb_lists[i];
        while (list)
        {
            if ( ( list->local_port == local_port ) && ( list->remote_port == remote_port ) && ( list->remote_ip.u_addr.ip4.addr == ip_addr.addr ) )
            {
                list->snd_nxt = list->lastack = list->snd_wl2 = list->snd_lbb = seq_num;
                list->rcv_nxt = ack_num;
                if( reset )
                {
                    list->state = FIN_WAIT_2;
                }
                return ret;
            }
            list = list->next;
        }
    }
#endif

#if defined(COMPONENT_NETXDUO)
    NX_TCP_SOCKET  *socket_ptr;
    ULONG          sockets_count;
    UINT           keepalive_enabled = NX_FALSE;
    NX_IP          *ip_ptr = NULL;
    uint32_t       tmp_ip;

    ip_ptr = cy_network_get_nw_interface(CY_NETWORK_WIFI_STA_INTERFACE, CY_NETWORK_WIFI_STA_INTERFACE);

    /* Pickup the number of created TCP sockets.  */
    sockets_count =  ip_ptr -> nx_ip_tcp_created_sockets_count;

    /* Pickup the first socket.  */
    socket_ptr =  ip_ptr -> nx_ip_tcp_created_sockets_ptr;

    while ((sockets_count--) && socket_ptr)
    {
        keepalive_enabled = socket_ptr -> nx_tcp_socket_keepalive_enabled;
        /* Is keep alive enabled on this socket? */
        if (keepalive_enabled)
        {
            if (socket_ptr -> nx_tcp_socket_state == NX_TCP_ESTABLISHED)
            {
                stoip4(remote_ip, strlen(remote_ip), &tmp_ip);

                /* In netxduo IP address is stored in little-endian format.
                 * Converting network order to host byte order which is little-endian for comparison */
                tmp_ip = ntohl(tmp_ip);

                if ((socket_ptr -> nx_tcp_socket_port == local_port) && (socket_ptr -> nx_tcp_socket_connect_port == remote_port) && (socket_ptr -> nx_tcp_socket_connect_ip.nxd_ip_address.v4 == tmp_ip) )
                {
                    /* Restore the updated TCP sequence numbers obtained from the FW. */
                    socket_ptr -> nx_tcp_socket_tx_sequence = seq_num;
                    socket_ptr -> nx_tcp_socket_rx_sequence = ack_num;
                    ret = WHD_SUCCESS;
                }
            }
        }
    }
#endif

    return ret;
}


/** GET sequence number of the TCP packet when provided source and destination port,
 * destination IP address.
 * */
whd_result_t whd_tlsoe_get_sock_stats( cy_tls_sock_seq_t *seq, uint16_t local_port, uint16_t remote_port, const char *remote_ip )
{
#if LWIP_TCP
    extern struct tcp_pcb *tcp_bound_pcbs;
    extern struct tcp_pcb *tcp_active_pcbs;
    extern struct tcp_pcb *tcp_tw_pcbs;
    struct tcp_pcb **const my_tcp_pcb_lists[] = { &tcp_bound_pcbs, &tcp_active_pcbs, &tcp_tw_pcbs };
    int i;
    struct tcp_pcb *list;
    ip4_addr_t ip_addr;

    ip4addr_aton( remote_ip, &ip_addr );

    for  ( i = 0; i < (int)( sizeof ( my_tcp_pcb_lists ) / sizeof ( my_tcp_pcb_lists[0] ) ); i++ )
    {
        list = *my_tcp_pcb_lists[i];
        while ( list )
        {
            if ( ( list->local_port == local_port ) && ( list->remote_port == remote_port ) && ( list->remote_ip.u_addr.ip4.addr == ip_addr.addr ) )
            {
                seq->srcip = list->local_ip.u_addr.ip4.addr;
                seq->dstip = list->remote_ip.u_addr.ip4.addr;
                seq->srcport = list->local_port;
                seq->dstport = list->remote_port;
                seq->seqnum = list->snd_nxt;
                seq->acknum = list->rcv_nxt;
                seq->rx_window = list->rcv_wnd;
                return 0;
            }
            list = list->next;
        }
    }
    return 1;
#endif

#if defined(COMPONENT_NETXDUO)

    NX_TCP_SOCKET  *socket_ptr;
    ULONG          sockets_count;
    UINT           keepalive_enabled = NX_FALSE;
    NX_IP          *ip_ptr = NULL;
    uint32_t       tmp_ip;

    /* Get NX_IP pointer */
    ip_ptr = cy_network_get_nw_interface(CY_NETWORK_WIFI_STA_INTERFACE, 0);

    /* Pickup the number of created TCP sockets.  */
    sockets_count =  ip_ptr -> nx_ip_tcp_created_sockets_count;

    /* Pickup the first socket.  */
    socket_ptr =  ip_ptr -> nx_ip_tcp_created_sockets_ptr;

    /* Loop through the created sockets.  */
    while ((sockets_count--) && socket_ptr)
    {
        keepalive_enabled = socket_ptr -> nx_tcp_socket_keepalive_enabled;
        /* Is keep alive enabled on this socket? */
        if (keepalive_enabled)
        {
            if (socket_ptr -> nx_tcp_socket_state == NX_TCP_ESTABLISHED)
            {
                stoip4(remote_ip, strlen(remote_ip), &tmp_ip);

                /* In netxduo IP address is stored in little-endian format.
                 * Converting network order to host byte order which is little-endian for comparison */
                tmp_ip = ntohl(tmp_ip);

                if ((socket_ptr -> nx_tcp_socket_port == local_port) && (socket_ptr -> nx_tcp_socket_connect_port == remote_port) && (socket_ptr -> nx_tcp_socket_connect_ip.nxd_ip_address.v4 == tmp_ip) )
                {
                    /* Store the IP address in Big-endian format.
                     * Convert host byte order to network order before storing */
                    seq->srcip = htonl(ip_ptr -> nx_ip_interface[0].nx_interface_ip_address);
                    seq->dstip = htonl(socket_ptr -> nx_tcp_socket_connect_ip.nxd_ip_address.v4);

                    seq->srcport = socket_ptr -> nx_tcp_socket_port;
                    seq->dstport = socket_ptr -> nx_tcp_socket_connect_port;
                    seq->seqnum = socket_ptr -> nx_tcp_socket_tx_sequence;
                    seq->acknum = socket_ptr -> nx_tcp_socket_rx_sequence;
                    seq->rx_window = socket_ptr -> nx_tcp_socket_rx_window_default;
                    return WHD_SUCCESS;
                }
            }
        }
        /* Move to the next TCP socket.  */
        socket_ptr =  socket_ptr -> nx_tcp_socket_created_next;
    }
    return 1;
#endif
}

#ifdef CYCFG_WIFI_MQTT_OL_SUPPORT
static void *tlsoe_callback_handler( whd_interface_t ifp, const whd_event_header_t *event_header, const uint8_t *event_data,
                            void *handler_user_data )
{
    cy_rtos_get_mutex(&cy_lp_mutex, CY_RTOS_NEVER_TIMEOUT);
    TLSOE_DEBUG_PRINTF( ( "%s: Entered event %ld\n", __func__, event_header->event_type ) );

    if ( event_header != NULL )
    {
        switch ( event_header->event_type )
        {
            case WLC_E_WAKE_EVENT:
                if ( event_data != NULL )
                {
                    cylpa_on_emac_activity( false );
                    tcp_reset = 1;
                }
                break;
            default:
                break;
        } /* end of switch */
    }

    TLSOE_DEBUG_PRINTF( ( "%s: Exit \n", __func__ ) );
    cy_rtos_set_mutex( &cy_lp_mutex );

    return handler_user_data;
}
#endif

whd_result_t
whd_wowl_set_pattern( whd_t *whd, uint8_t* patt, uint16_t pattern_size, uint16_t pattern_type, uint16_t pattern_offset, uint8_t set_pattern )
{
    whd_result_t result = WHD_SUCCESS;
#ifdef CYCFG_WIFI_MQTT_OL_SUPPORT
    uint8_t mask_size = (pattern_size + 7) / 8;
    uint8_t mask[64];
    char *mode;
    if( set_pattern )
    {
        mode = "add";
    }
    else
    {
        mode = "del";
    }

    for( int i  = 0; i < mask_size; i++ )
    {
        mask[i] = 0xff;
    }

    result = whd_set_wowl_pattern( whd, mode , pattern_offset, mask_size, mask, pattern_size, patt, pattern_type );
    if ( result !=  WHD_SUCCESS)
    {
        TLSOE_DEBUG_PRINTF( ("%s: add pattern failed \n", __func__) );
    }
#endif
    return result;
}

whd_result_t
whd_tlsoe_set_wowl_pattern( whd_t *whd, uint8_t* pattern, uint16_t pattern_len, uint16_t pattern_type, uint16_t patttern_offset )
{
    whd_result_t result = WHD_SUCCESS;
#ifdef CYCFG_WIFI_MQTT_OL_SUPPORT
    uint32_t wowl;
    whd_get_wowl_cap( whd, &wowl );

    if( pattern_len > 0 )
    {
        wowl &= ( WL_WOWL_SECURE | WL_WOWL_NET );
        if(wowl != ( WL_WOWL_SECURE | WL_WOWL_NET ) )
        {
            wowl |= ( WL_WOWL_SECURE | WL_WOWL_NET );
            result = whd_set_wowl_cap( whd, wowl );
        }
        result |= whd_wowl_set_pattern( whd, pattern, pattern_len, pattern_type, patttern_offset,1 );
    }
    else
    {
        wowl &= WL_WOWL_SECURE;
        if( wowl != WL_WOWL_SECURE )
        {
            wowl |= WL_WOWL_SECURE;
            result = whd_set_wowl_cap( whd, wowl );
        }
    }
#endif
    return result;
}

whd_result_t
whd_tlsoe_del_wowl_pattern( whd_t *whd, uint8_t* pattern, uint16_t pattern_size, uint16_t pattern_type,  uint16_t patttern_offset )
{
    whd_result_t result = WHD_SUCCESS;
#ifdef CYCFG_WIFI_MQTT_OL_SUPPORT
    if( pattern_size > 0 )
    {
        result = whd_wowl_set_pattern( whd, pattern, pattern_size, pattern_type, patttern_offset, 0 );
    }

#endif
    return result;
}

whd_result_t
whd_tlsoe_disable( whd_t *whd, uint16_t local_port, uint16_t remote_port, const char *remote_ip, void* socket )
{
    whd_result_t ret = WHD_SUCCESS;
    cy_rslt_t result = CY_RSLT_SUCCESS;
    secure_sess_info_t tls_sess;

    ret = whd_wowl_get_secure_session_status( whd, &tls_sess );
    if ( ret == WHD_SUCCESS )
    {
        result = cy_socket_update_tls_sequence( socket, &(tls_sess.read_seq[0]), &(tls_sess.write_seq[0]) );
        if (result == WHD_SUCCESS)
        {
            ret = whd_tlsoe_update_sock_seq( local_port, remote_port, remote_ip,tls_sess.tcp_seq ,tls_sess.tcp_ack, tcp_reset );
            if ( ret != WHD_SUCCESS )
            {
                TLSOE_ERROR_PRINTF( ( "whd_tlsoe_update_sock_seq failed\n" ) );
            }
        }
    }
#if defined(COMPONENT_NETXDUO)
    /* Release the TLS protection */
    tx_mutex_put(&_nx_secure_tls_protection);
#endif
    ret = whd_wowl_clear( whd );

    whd_wifi_deregister_event_handler( whd, tlsoe_offload_update_entry );
    return ret;
}

#if defined(COMPONENT_NETXDUO)
/*
 * Returns table index if found, else returns -1
 */
static int find_mac_addr(uint32_t *dst_ip, wl_ether_addr_t *eth_ret, uint32_t *ip_ret)
{
    UINT          index=-1;
    NX_ARP       *arp_ptr;
    NX_ARP       *search_ptr;
    NX_ARP       *arp_list_head;
    NX_IP        *ip_ptr = NULL;
    NX_INTERFACE *outgoing_interface = NX_NULL;
    ULONG         res;
    ULONG         next_hop_address;

    /* Get NX_IP pointer */
    ip_ptr = cy_network_get_nw_interface(CY_NETWORK_WIFI_STA_INTERFACE, CY_NETWORK_WIFI_STA_INTERFACE);

    /* Find the next hop address of given IP address */
    res = _nx_ip_route_find(ip_ptr, *dst_ip, &outgoing_interface, &next_hop_address);
    if(res != 0)
    {
        TLSOE_ERROR_PRINTF( ( "_nx_ip_route_find failed. res = %x \n", res ) );
    }

    /* Calculate the hash index for the next hop IP address.
     * Note: The IP address should be in little-endian(host byte order) format for netxduo  */
    index =  (UINT)((next_hop_address + ((next_hop_address) >> 8)) & NX_ARP_TABLE_MASK);

    /* Pickup the head pointer of the ARP entries for this IP instance.  */
    arp_list_head =  ip_ptr -> nx_ip_arp_table[index];

    search_ptr =  arp_list_head;
    arp_ptr    =  NX_NULL;
    while (search_ptr)
    {
        /* Determine if there is a duplicate IP address.
         * Note: The IP address should be in little-endian(host byte order) format for netxduo  */
        if (search_ptr -> nx_arp_ip_address == next_hop_address)
        {
            /* Yes, the IP address matches, setup the ARP entry pointer.  */
            arp_ptr =  search_ptr;

            /* Get out of the loop.  */
            break;
        }

        /* Move to the next entry in the active list.  */
        search_ptr =  search_ptr -> nx_arp_active_next;

        /* Determine if the search pointer is back at the head of
           the list.  */
        if (search_ptr == arp_list_head)
        {
            /* End of the ARP list, end the search.  */
            break;
        }
    }

    /* Determine if an ARP entry is found.  */
    if (arp_ptr)
    {
        eth_ret->octet[0] = (uint8_t)((arp_ptr->nx_arp_physical_address_msw >>  8) & 0xFF);
        eth_ret->octet[1] = (uint8_t)((arp_ptr->nx_arp_physical_address_msw >>  0) & 0xFF);
        eth_ret->octet[2] = (uint8_t)((arp_ptr->nx_arp_physical_address_lsw >> 24) & 0xFF);
        eth_ret->octet[3] = (uint8_t)((arp_ptr->nx_arp_physical_address_lsw >> 16) & 0xFF);
        eth_ret->octet[4] = (uint8_t)((arp_ptr->nx_arp_physical_address_lsw >>  8) & 0xFF);
        eth_ret->octet[5] = (uint8_t)((arp_ptr->nx_arp_physical_address_lsw >>  0) & 0xFF);
        *ip_ret = arp_ptr->nx_arp_ip_address;
    }
    return index;
}
#endif

whd_result_t
whd_tlsoe_activate( whd_t *whd, uint16_t local_port, uint16_t remote_port, const char *remote_ip, uint32_t interval, uint8_t* pkt, uint32_t pkt_len, void* socket )
{
    whd_result_t result = WHD_SUCCESS;
    cy_wcm_mac_t mac_addr;
#if defined(COMPONENT_NETXDUO)
    uint32_t ip_ret = 0;
    wl_ether_addr_t eth_ret;
    uint32_t dst_ip;
#endif
#if defined(COMPONENT_LWIP)
    const ip4_addr_t *ip_ret;
    wl_ether_addr_t *eth_ret;
#endif
    tls_param_info_t tls_info;
    cy_tls_offload_info_t cy_tls_offload_info;
    cy_tls_sock_seq_t seq;
    memset( &cy_tls_offload_info, 0x00, sizeof( cy_tls_offload_info ) );
    memset( &tls_info, 0x00, sizeof ( tls_info ) );
    memset( &seq, 0, sizeof ( seq ) );
    /* Get required Sequence and Ack numbers from TCP stack */
    if ( whd_tlsoe_get_sock_stats( &seq, local_port, remote_port, remote_ip ) != 0 )
    {
        TLSOE_ERROR_PRINTF( ( "%s: Can't find socket:  %s Local Port %d, Remote Port %d\n", __func__,remote_ip, local_port, remote_port ) );
        return WHD_BADARG;
    }
    /* Local mac address */
    result =  whd_wifi_get_mac_address( whd, &seq.src_mac );
    if ( result != WHD_SUCCESS )
    {
        TLSOE_ERROR_PRINTF( ( "get_mac_address failed\n" ) );
        return WHD_BADARG;
    }

    /* Remote mac address */
#ifdef COMPONENT_LWIP
    if ( etharp_find_addr( NULL, (const ip4_addr_t * )&seq.dstip, &eth_ret, &ip_ret ) >= 0 )
    {
        if ( ip_ret->addr != seq.dstip )
        {
            TLSOE_ERROR_PRINTF( ( "%s: Hey orig IP and LWIP IP don't match!\n", __func__ ) );
        }
        memcpy( seq.dst_mac.octet, eth_ret, sizeof ( seq.dst_mac.octet ) );
    }
    else
#endif
#if defined(COMPONENT_NETXDUO)
    dst_ip = ntohl(seq.dstip);
    if (find_mac_addr(&dst_ip, &eth_ret, &ip_ret) >= 0)
    {
        memcpy(seq.dst_mac.octet, &eth_ret, sizeof(seq.dst_mac.octet) );
    }
    else
#endif
    {
        TLSOE_DEBUG_PRINTF( ("%s: Remote mac addr not found, using Gatway mac address\n", __func__) );
        result = cy_wcm_get_gateway_mac_address(&mac_addr);
        if(result != CY_RSLT_SUCCESS)
        {
            TLSOE_ERROR_PRINTF( ("%s: Unable to get gateway mac address\n", __func__) );
            return WHD_BADARG;
        }
        memcpy(seq.dst_mac.octet, mac_addr, sizeof(seq.dst_mac.octet));
    }

    TLSOE_DEBUG_PRINTF( ( "Local Mac Addr: %02X:%02X:%02X:%02X:%02X:%02X\n",
                       seq.src_mac.octet[0], seq.src_mac.octet[1], seq.src_mac.octet[2],
                       seq.src_mac.octet[3], seq.src_mac.octet[4], seq.src_mac.octet[5] ) );
    TLSOE_DEBUG_PRINTF( ( "Remote Mac addr: %02X:%02X:%02X:%02X:%02X:%02X\n",
                       seq.dst_mac.octet[0], seq.dst_mac.octet[1], seq.dst_mac.octet[2],
                       seq.dst_mac.octet[3], seq.dst_mac.octet[4], seq.dst_mac.octet[5] ) );

    result = cy_socket_get_tls_info( socket, &cy_tls_offload_info );
    if ( result != WHD_SUCCESS )
    {
        TLSOE_ERROR_PRINTF( ( "%s: cy_socket_get_tls_info failed: 0X%X\n", __func__, result ) );
        return WHD_BADARG;
    }

    tls_info.version.major = cy_tls_offload_info.protocol_major_ver;
    tls_info.version.minor = cy_tls_offload_info.protocol_minor_ver;

    tls_info.compression_algorithm = cy_tls_offload_info.compression_algorithm;
    tls_info.cipher_algorithm = cy_tls_offload_info.cipher_algorithm;
    tls_info.cipher_type = cy_tls_offload_info.cipher_type;
    tls_info.mac_algorithm = cy_tls_offload_info.mac_algorithm;

    tls_info.encrypt_then_mac = cy_tls_offload_info.encrypt_then_mac;

    tls_info.write_iv_len = cy_tls_offload_info.write_iv_len;
    tls_info.read_iv_len = cy_tls_offload_info.read_iv_len;
    memcpy(tls_info.write_iv,cy_tls_offload_info.write_iv, tls_info.write_iv_len);
    memcpy(tls_info.read_iv, cy_tls_offload_info.read_iv,  tls_info.read_iv_len);

    tls_info.write_master_key_len = cy_tls_offload_info.write_master_key_len;
    tls_info.read_master_key_len = cy_tls_offload_info.read_master_key_len;
    memcpy(tls_info.write_master_key,cy_tls_offload_info.write_master_key, tls_info.write_master_key_len);
    memcpy(tls_info.read_master_key,cy_tls_offload_info.read_master_key, tls_info.read_master_key_len);

    tls_info.write_mac_key_len = cy_tls_offload_info.write_mac_key_len;
    tls_info.read_mac_key_len = cy_tls_offload_info.read_mac_key_len;
    memcpy(tls_info.write_mac_key,cy_tls_offload_info.write_mac_key, tls_info.write_mac_key_len);
    memcpy(tls_info.read_mac_key,cy_tls_offload_info.read_mac_key, tls_info.read_mac_key_len);

    tls_info.read_sequence_len = cy_tls_offload_info.read_sequence_len;
    memcpy(tls_info.read_sequence, cy_tls_offload_info.read_sequence, tls_info.read_sequence_len);
    tls_info.write_sequence_len = cy_tls_offload_info.write_sequence_len;
    memcpy(tls_info.write_sequence, cy_tls_offload_info.write_sequence, tls_info.write_sequence_len);

    tls_info.local_port = SWAP16(seq.srcport);
    memcpy(tls_info.local_ip, &seq.srcip, sizeof(seq.srcip));
    memcpy(tls_info.local_mac_addr, seq.src_mac.octet, sizeof ( seq.src_mac.octet ) );

    tls_info.remote_port = SWAP16(seq.dstport);
    memcpy(tls_info.remote_ip, &seq.dstip, sizeof(seq.dstip));
    memcpy(tls_info.remote_mac_addr, seq.dst_mac.octet, sizeof ( seq.dst_mac.octet ) );

    tls_info.tcp_ack_num = SWAP32(seq.acknum);
    tls_info.tcp_seq_num = SWAP32(seq.seqnum);
    tls_info.app_syncid = (uint32_t) rand();
    tls_info.keepalive_interval = interval;

    tls_info.payload_len = ( (pkt_len) < (TLS_MAX_PAYLOAD_LEN) ? (pkt_len) : (TLS_MAX_PAYLOAD_LEN) );
    memcpy(tls_info.payload, pkt, tls_info.payload_len);

    result = whd_wowl_activate_secure(whd, &tls_info);
    if (result != WHD_SUCCESS)
    {
        TLSOE_ERROR_PRINTF( ("whd_tlsoe_conn_add failed\n") );
        return WHD_BADARG;
    }

#if defined(COMPONENT_NETXDUO)
    /* Get TLS protection */
    tx_mutex_get(&_nx_secure_tls_protection, TX_WAIT_FOREVER);
#endif

    /* Enable WLE_E_KTO Event */
    tcp_reset = 0;
    whd_management_set_event_handler(whd, tlsoe_events, tlsoe_callback_handler, (void *)whd, &tlsoe_offload_update_entry);
    TLSOE_DEBUG_PRINTF( ( "%s: success\n", __func__ ) );
    return result;
}

#ifdef __cplusplus
}
#endif

#endif //CYCFG_WIFI_MQTT_OL_SUPPORT
