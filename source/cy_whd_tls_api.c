/*
 * Copyright 2024, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
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
#ifdef COMPONENT_LWIP
#include "lwip/tcp.h"
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
ssize_t etharp_find_addr(struct netif *netif, const ip4_addr_t *ipaddr, wl_ether_addr_t **eth_ret,
                         const ip4_addr_t **ip_ret);
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
    return ret;
#else
    return ret;
#endif
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
#else
    return 0;
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
whd_wowl_set_pattern( whd_t *whd, uint8_t* patt, uint16_t pattern_size, uint16_t pattern_rule, uint16_t pattern_offset, uint8_t set_pattern )
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
    
    result = whd_set_wowl_pattern( whd, mode , pattern_offset, mask_size, mask, pattern_size, patt, pattern_rule );
    if ( result !=  WHD_SUCCESS)
    {
        TLSOE_DEBUG_PRINTF( ("%s: add pattern failed \n", __func__) );
    }
#endif
    return result;
}

whd_result_t
whd_tlsoe_set_wowl_pattern( whd_t *whd, uint8_t* pattern, uint16_t pattern_len, uint16_t pattern_rule, uint16_t patttern_offset )
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
        result |= whd_wowl_set_pattern( whd, pattern, pattern_len, pattern_rule, patttern_offset,1 );
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
whd_tlsoe_del_wowl_pattern( whd_t *whd, uint8_t* pattern, uint16_t pattern_size, uint16_t pattern_rule,  uint16_t patttern_offset )
{
    whd_result_t result = WHD_SUCCESS;
#ifdef CYCFG_WIFI_MQTT_OL_SUPPORT
    if( pattern_size > 0 )
    {
        result = whd_wowl_set_pattern( whd, pattern, pattern_size, pattern_rule,patttern_offset, 0 );
    }
#endif
    return result;
}

whd_result_t
whd_tlsoe_disable( whd_t *whd, uint16_t local_port, uint16_t remote_port, const char *remote_ip, void* socket )
{
    whd_result_t ret = WHD_SUCCESS;
#ifdef CYCFG_WIFI_MQTT_OL_SUPPORT
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
    ret = whd_wowl_clear( whd );

    whd_wifi_deregister_event_handler( whd, tlsoe_offload_update_entry );
#endif
    return ret;
}

whd_result_t
whd_tlsoe_activate( whd_t *whd, uint16_t local_port, uint16_t remote_port, const char *remote_ip, uint32_t interval, uint8_t* pkt, uint32_t pkt_len, void* socket )
{
    whd_result_t result = WHD_SUCCESS;
#ifdef CYCFG_WIFI_MQTT_OL_SUPPORT
#if LWIP_TCP
    tls_param_info_t tls_info;
    cy_tls_offload_info_t cy_tls_offload_info;
    cy_tls_sock_seq_t seq;
    const ip4_addr_t *ip_ret;
    wl_ether_addr_t *eth_ret;

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
    if ( etharp_find_addr( NULL, (const ip4_addr_t * )&seq.dstip, &eth_ret, &ip_ret ) >= 0 )
    {
        if ( ip_ret->addr != seq.dstip )
        {
            TLSOE_ERROR_PRINTF( ( "%s: Hey orig IP and LWIP IP don't match!\n", __func__ ) );
        }
        memcpy( seq.dst_mac.octet, eth_ret, sizeof ( seq.dst_mac.octet ) );
    }
    else
    {
        TLSOE_DEBUG_PRINTF( ( "%s: Remote mac addr not found, using bssid\n", __func__ ) );
        result = whd_wifi_get_bssid( whd, &seq.dst_mac );
        if ( result != WHD_SUCCESS )
        {
            TLSOE_ERROR_PRINTF( ( "%s: get_bssid failed\n", __func__ ) );
            return WHD_BADARG;
        }
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
    
    /* Enable WLE_E_KTO Event */
    tcp_reset = 0;
    whd_management_set_event_handler(whd, tlsoe_events, tlsoe_callback_handler, (void *)whd, &tlsoe_offload_update_entry);
#endif /* LWIP_TCP */
#endif
    TLSOE_DEBUG_PRINTF( ( "%s: success\n", __func__ ) );
    return result;
}

#ifdef __cplusplus
}
#endif
