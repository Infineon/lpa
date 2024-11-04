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
* @file cy_lpa_whd_tko_api.c
* @brief WHD TCP Keepalive Offload API
*/

#include "whd_types.h"
#include "whd_int.h"
#include "whd_proto.h"
#include "cy_lpa_common_priv.h"
#include "cy_lpa_wifi_ol_common.h"
#include "cy_lpa_wifi_ol.h"
#include "cy_lpa_wifi_tko_ol.h"
#include "cy_whd_tko_api.h"
#include "whd_cdc_bdc.h"
#include "whd_network_types.h"
#include "whd_buffer_api.h"
#include "cy_nw_helper.h"
#include "whd_wlioctl.h"
#ifdef COMPONENT_LWIP
#include "lwip/tcp.h"
#endif
#include "ip4string.h"
#ifdef COMPONENT_NETXDUO
#include "cy_network_mw_core.h"
#include "nx_api.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************
*
*  Defines
*
********************************************************************/

/* Default TCP Keepalive retry parameters.  */
#define TCP_KEEPALIVE_OFFLOAD_INTERVAL_SEC       (20)
#define TCP_KEEPALIVE_OFFLOAD_RETRY_COUNT        (3)
#define TCP_KEEPALIVE_OFFLOAD_RETRY_INTERVAL_SEC (3)


/********************************************************************
*
*  Macros
*
********************************************************************/
#define IFP_TO_DRIVER(ifp)  ( ( (whd_interface_t)ifp )->whd_driver )

#ifdef TKO_OL_DEBUG
#define TKO_ERROR_PRINTF(x) printf x
#define TKO_DEBUG_PRINTF(x) printf x
#else
#define TKO_ERROR_PRINTF(x)
#define TKO_DEBUG_PRINTF(x)
#endif

#define TKO_ERROR   0xfffe

#define print_ip4(ipaddr)  \
    printf("%lu.%lu.%lu.%lu\n", (ipaddr) & 0xff, (ipaddr) >> 8 & 0xff, (ipaddr) >> 16 & 0xff, (ipaddr) >> 24 & 0xff)

/********************************************************************
 *
 *  Structures
 *
 *********************************************************************/
static const whd_event_num_t tko_events[]   = { WLC_E_TKO, WLC_E_NONE };
static uint16_t tko_offload_update_entry = 0xFF;
static uint16_t tko_state_enabled = false;
void cylpa_on_emac_activity(bool is_tx_activity);    //RX_EVENT_FLAG
extern cy_mutex_t cy_lp_mutex;

/*********************************************************************************************
*
*  Forward Declarations
*
*********************************************************************************************/
static int tko_connect_init(wl_tko_connect_t *connect, sock_seq_t *keep_alive_offload, uint8_t index);
#ifdef COMPONENT_LWIP
ssize_t etharp_find_addr(struct netif *netif, const ip4_addr_t *ipaddr, wl_ether_addr_t **eth_ret,
                         const ip4_addr_t **ip_ret);
#endif
static int prep_packet(sock_seq_t *seq, int index, uint8_t *buf);

void *whd_callback_handler(whd_interface_t ifp, const whd_event_header_t *event_header, const uint8_t *event_data,
                           /*@null@*/ void *handler_user_data);

whd_result_t
whd_tko_enable(whd_t *whd)
{
    whd_result_t ret = WHD_SUCCESS;
    whd_event_handler_t handler = whd_callback_handler;

    ret =  whd_tko_toggle(whd, WHD_TRUE);

    /* Enable WLE_E_KTO Event */
    whd_management_set_event_handler(whd, tko_events, handler, (void *)whd, &tko_offload_update_entry);
    tko_state_enabled = WHD_TRUE;
    return ret;
}

whd_result_t
whd_tko_disable(whd_t *whd)
{
    if (tko_state_enabled)
    {
        tko_state_enabled = WHD_FALSE;

        /* Disable WLC_E_TKO Event */
        whd_wifi_deregister_event_handler(whd, tko_offload_update_entry);
    }

    return whd_tko_toggle(whd, WHD_FALSE);
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

    /* Get NX_IP pointer */
    ip_ptr = cy_network_get_nw_interface(CY_NETWORK_WIFI_STA_INTERFACE, CY_NETWORK_WIFI_STA_INTERFACE);

    /* Calculate the hash index for the specified IP address.
     * Note: The IP address should be in little-endian(host byte order) format for netxduo  */
    index =  (UINT)((ntohl(*dst_ip) + ((ntohl(*dst_ip)) >> 8)) & NX_ARP_TABLE_MASK);

    /* Pickup the head pointer of the ARP entries for this IP instance.  */
    arp_list_head =  ip_ptr -> nx_ip_arp_table[index];

    search_ptr =  arp_list_head;
    arp_ptr    =  NX_NULL;
    while (search_ptr)
    {
        /* Determine if there is a duplicate IP address.
         * Note: The IP address should be in little-endian(host byte order) format for netxduo  */
        if (search_ptr -> nx_arp_ip_address == ntohl(*dst_ip))
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
/*
 * Host is going into sleep mode, activate TKO in FW.
 *
 * For each connection {
 *   1 Query the host TCP stack to get current sequence numbers for each connection.
 *   2 Use this info to fill in FW ioctl struct.
 *   3 XXX How to get local and remote mac addresses???
 *   4 Call ioctl to configure.
 * }
 * Enable TKO feature
 *
 */
whd_result_t
whd_tko_activate(whd_t *whd, uint8_t index, uint16_t local_port, uint16_t remote_port, const char *remote_ip)
{
    whd_result_t result = WHD_SUCCESS;
    uint32_t len = 0;
    uint8_t *data = NULL;
    wl_tko_t *tko = NULL;
    wl_tko_connect_t *connect = NULL;
    sock_seq_t seq;
    int tko_len;
    whd_buffer_t buffer;
#if defined(COMPONENT_LWIP)
    const ip4_addr_t *ip_ret;
    wl_ether_addr_t *eth_ret;
#endif
#if defined(COMPONENT_NETXDUO)
    uint32_t ip_ret = 0;
    wl_ether_addr_t eth_ret;
#endif

    /* Get required Sequence and Ack numbers from TCP stack */
    memset(&seq, 0, sizeof(seq) );
    if (sock_stats(&seq, local_port, remote_port, remote_ip) != WHD_SUCCESS)
    {
        TKO_DEBUG_PRINTF( ("%s: Can't find socket: index[%d] %s Local Port %d, Remote Port %d\n", __func__, index,
                           remote_ip, local_port, remote_port) );
        return WHD_BADARG;
    }

    /* Local mac address */
    result =  whd_wifi_get_mac_address(whd, &seq.src_mac);
    if (result != WHD_SUCCESS)
    {
        TKO_ERROR_PRINTF( ("get_mac_address failed\n") );
        return WHD_BADARG;
    }

    /* Remote mac address */
#if defined(COMPONENT_LWIP)
    if (etharp_find_addr(NULL, (const ip4_addr_t * )&seq.dstip, &eth_ret, &ip_ret) >= 0)
    {
        if (ip_ret->addr != seq.dstip)
        {
            TKO_ERROR_PRINTF( ("%s: Hey orig IP and LWIP IP don't match!\n", __func__) );
        }
        memcpy(seq.dst_mac.octet, eth_ret, sizeof(seq.dst_mac.octet) );
    }
    else
#elif defined(COMPONENT_NETXDUO)
    if (find_mac_addr(&seq.dstip, &eth_ret, &ip_ret) >= 0)
    {
        /* Note: The IP address should be in little-endian(host byte order) format for netxduo */
        if (ip_ret != ntohl(seq.dstip))
        {
            TKO_ERROR_PRINTF( ("%s: Hey orig IP and Netxduo IP don't match!\n", __func__) );
        }
        memcpy(seq.dst_mac.octet, &eth_ret, sizeof(seq.dst_mac.octet) );
    }
    else
#endif
    {
        TKO_DEBUG_PRINTF( ("%s: Remote mac addr not found, using bssid\n", __func__) );
        result = whd_wifi_get_bssid(whd, &seq.dst_mac);
        if (result != WHD_SUCCESS)
        {
            TKO_ERROR_PRINTF( ("%s: get_bssid failed\n", __func__) );
            return WHD_BADARG;
        }
    }

    TKO_DEBUG_PRINTF( ("Local Mac Addr: %02X:%02X:%02X:%02X:%02X:%02X\n",
                       seq.src_mac.octet[0], seq.src_mac.octet[1], seq.src_mac.octet[2],
                       seq.src_mac.octet[3], seq.src_mac.octet[4], seq.src_mac.octet[5]) );
    TKO_DEBUG_PRINTF( ("Remote Mac addr: %02X:%02X:%02X:%02X:%02X:%02X\n",
                       seq.dst_mac.octet[0], seq.dst_mac.octet[1], seq.dst_mac.octet[2],
                       seq.dst_mac.octet[3], seq.dst_mac.octet[4], seq.dst_mac.octet[5]) );

    len = (int)(WHD_PAYLOAD_MTU - strlen(IOVAR_STR_TKO) - 1);
    data = (uint8_t * )(IFP_TO_DRIVER(whd)->proto->get_iovar_buffer(IFP_TO_DRIVER(whd), &buffer, (uint16_t)len, IOVAR_STR_TKO));

    tko = (wl_tko_t *)data;

    tko->subcmd_id = WL_TKO_SUBCMD_CONNECT;
    tko->len = offsetof(wl_tko_t, data);
    connect = (wl_tko_connect_t *)tko->data;

    tko_len = tko_connect_init(connect, &seq, index);
    if (tko_len == TKO_ERROR)
    {
        TKO_ERROR_PRINTF( ("%s: tko_connect_init() failed\n", __func__) );
        return tko_len;
    }

    tko->len += tko_len;
    len = offsetof(wl_tko_t, data) + tko->len;

    tko->subcmd_id = htod16(tko->subcmd_id);
    tko->len = htod16(tko->len);

    /* invoke SET iovar */
    result = IFP_TO_DRIVER(whd)->proto->set_iovar(whd, buffer, NULL);
    if (result != WHD_SUCCESS)
    {
        TKO_ERROR_PRINTF( ("%s: tko CONNECT subcmd IOVAR failed. Result: %u\n", __func__, (unsigned int)result) );
    }
    return result;
}

/* Given a pair of port and ip addresses, return sequence numbers
 * and other low level info about a connection from network stack.
 */
whd_result_t
sock_stats(sock_seq_t *seq, uint16_t local_port, uint16_t remote_port, const char *remote_ip)
{
#if defined(COMPONENT_LWIP) && (LWIP_TCP == 1)
    extern struct tcp_pcb *tcp_bound_pcbs;
    extern struct tcp_pcb *tcp_active_pcbs;
    extern struct tcp_pcb *tcp_tw_pcbs;
    struct tcp_pcb **const my_tcp_pcb_lists[] = {&tcp_bound_pcbs, &tcp_active_pcbs, &tcp_tw_pcbs };
    int i;
    struct tcp_pcb *list;

    memset(seq, 0, sizeof(sock_seq_t) );
    for  (i = 0; i < (int)(sizeof(my_tcp_pcb_lists) / sizeof(my_tcp_pcb_lists[0]) ); i++)
    {
        list = *my_tcp_pcb_lists[i];
        while (list)
        {
            uint32_t tmp_ip;
            stoip4(remote_ip, strlen(remote_ip), &tmp_ip);
            if ((list->local_port == local_port) && (list->remote_port == remote_port) && (IP_ADDR_STATS(list->remote_ip) == tmp_ip) )
            {
                seq->srcip = IP_ADDR_STATS(list->local_ip);
                seq->dstip = IP_ADDR_STATS(list->remote_ip);
                seq->srcport = list->local_port;
                seq->dstport = list->remote_port;
                seq->seqnum = list->snd_nxt;
                seq->acknum = list->rcv_nxt;
                seq->rx_window = list->rcv_wnd;
                return WHD_SUCCESS;
            }
            list = list->next;
        }
    }

#elif defined(COMPONENT_NETXDUO) && defined(NX_ENABLE_TCP_KEEPALIVE)

    NX_TCP_SOCKET  *socket_ptr;
    ULONG          sockets_count;
    UINT           keepalive_enabled = NX_FALSE;
    NX_IP          *ip_ptr = NULL;
    uint32_t       tmp_ip;

    /* Get NX_IP pointer */
    ip_ptr = cy_network_get_nw_interface(CY_NETWORK_WIFI_STA_INTERFACE, CY_NETWORK_WIFI_STA_INTERFACE);

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
#endif
    return WHD_BADARG;
}

void *whd_callback_handler(whd_interface_t ifp, const whd_event_header_t *event_header, const uint8_t *event_data,
                           /*@null@*/ void *handler_user_data)
{
    cy_rtos_get_mutex(&cy_lp_mutex, CY_RTOS_NEVER_TIMEOUT);
    TKO_ERROR_PRINTF( ("%s: Entered \n", __func__) );
    if (event_header != NULL)
    {
        switch (event_header->event_type)
        {
            case WLC_E_TKO:
                if (event_data != NULL)
                {
                    tko_state_enabled = WHD_FALSE;
                    cylpa_on_emac_activity(false);
                }
                break;
            default:
                break;
        } /* end of switch */
    }

    TKO_ERROR_PRINTF( ("%s: Exit \n", __func__) );
    cy_rtos_set_mutex(&cy_lp_mutex);
    return handler_user_data;
}

/* Prepare request packet */
static int
prep_packet(sock_seq_t *seq, int index, uint8_t *buf)
{
    whd_ether_header_t *eth;
    ipv4_hdr_t *ip;
    bcmtcp_hdr_t *tcp;

    if (buf == NULL)
    {
        TKO_ERROR_PRINTF( ("%s: ERROR: NULL buf\n", __func__) );
        return TKO_ERROR;
    }

    eth = (whd_ether_header_t *)buf;

    /* Copy mac HW addresses */
    memcpy(eth->ether_dhost, seq->dst_mac.octet, ETHER_ADDR_LEN);
    memcpy(eth->ether_shost, seq->src_mac.octet, ETHER_ADDR_LEN);

    TKO_DEBUG_PRINTF( ("dhost : %02X:%02X:%02X:%02X:%02X:%02X\n", eth->ether_dhost[0], eth->ether_dhost[1],
                       eth->ether_dhost[2], eth->ether_dhost[3], eth->ether_dhost[4], eth->ether_dhost[5]) );
    TKO_DEBUG_PRINTF( ("shost : %02X:%02X:%02X:%02X:%02X:%02X\n", eth->ether_shost[0], eth->ether_shost[1],
                       eth->ether_shost[2], eth->ether_shost[3], eth->ether_shost[4], eth->ether_shost[5]) );

    eth->ether_type = hton16(ETHER_TYPE_IP);
    ip = (ipv4_hdr_t *)(buf + sizeof(whd_ether_header_t) );

    memcpy(ip->dst_ip, &seq->dstip, CYLPA_IPV4_ADDR_LEN);
    memcpy(ip->src_ip, &seq->srcip, CYLPA_IPV4_ADDR_LEN);

    ip->version_ihl = (CYLPA_IP_VER_4 << CYLPA_IPV4_VER_SHIFT) | (CYLPA_IPV4_MIN_HEADER_LEN / 4);
    ip->tos = 0;
    ip->tot_len = hton16(sizeof(ipv4_hdr_t) + sizeof(bcmtcp_hdr_t) );
    ip->id = hton16(0);
    ip->frag = 0;
    ip->ttl = 32;
    ip->prot = CYLPA_IP_PROTO_TCP;
    ip->hdr_chksum = 0;

    ip->hdr_chksum = cylpa_ipv4_hdr_cksum( (uint8_t *)ip, CYLPA_IPV4_HLEN(ip) );

    tcp = (bcmtcp_hdr_t *)(buf + sizeof(whd_ether_header_t) +
                           sizeof(ipv4_hdr_t) );
    tcp->src_port = hton16(seq->srcport);
    tcp->dst_port = hton16(seq->dstport);
    tcp->seq_num  = hton32(seq->seqnum);
    tcp->ack_num  = hton32(seq->acknum);
    tcp->tcpwin   = hton16(seq->rx_window);
    tcp->chksum   = 0;
    tcp->urg_ptr  = 0;
    tcp->hdrlen_rsvd_flags = (CYLPA_TCP_FLAG_ACK << 8) |
                             ( (sizeof(bcmtcp_hdr_t) >> 2) << CYLPA_TCP_HDRLEN_SHIFT );

    /* calculate TCP header checksum */
    tcp->chksum = cylpa_ipv4_tcp_hdr_cksum( (uint8_t *)ip, (uint8_t *)tcp, sizeof(*tcp) );
    if (tcp->chksum == TKO_ERROR)
    {
        TKO_ERROR_PRINTF( ("%s ERROR, Bad checksum 0x%x\n", __func__, tcp->chksum) );
        return TKO_ERROR;
    }

    TKO_DEBUG_PRINTF( ("%s Checksum Success 0x%x\n", __func__, tcp->chksum) );
    return (sizeof(whd_ether_header_t) + sizeof(ipv4_hdr_t) + sizeof(bcmtcp_hdr_t) );
}

static int
tko_connect_init(wl_tko_connect_t *connect, sock_seq_t *keep_alive_offload, uint8_t index)
{
    int datalen = 0;
    uint32_t srcip;
    uint32_t destip;

    connect->index = index;
    connect->ip_addr_type = 0;
    connect->local_port  = htod16(keep_alive_offload->srcport);
    connect->remote_port = htod16(keep_alive_offload->dstport);
    connect->local_seq   = htod32(keep_alive_offload->seqnum);
    connect->remote_seq  = htod32(keep_alive_offload->acknum);

    destip = keep_alive_offload->dstip;
    srcip = keep_alive_offload->srcip;
    memcpy(&connect->data[datalen], &srcip, sizeof(keep_alive_offload->srcip) );
    datalen += sizeof(keep_alive_offload->srcip);
    memcpy(&connect->data[datalen], &destip, sizeof(keep_alive_offload->dstip) );
    datalen += sizeof(keep_alive_offload->dstip);

    /* Keepalive uses previous sequence number so its not confused with a real packet */
    keep_alive_offload->seqnum = keep_alive_offload->seqnum - 1;

    //TKO_DEBUG_PRINTF(("_init: keep alive Seq num:%lu\n",    keep_alive_offload->seqnum ));
    //TKO_DEBUG_PRINTF(("_init: keep alive Ack num:%lu\n",    keep_alive_offload->acknum ));
    //TKO_DEBUG_PRINTF(("_init: keep alive Rcv Window:%u\n", keep_alive_offload->rx_window ));

    /* Create Request Packet */
    connect->request_len = prep_packet(keep_alive_offload, index, &connect->data[datalen]);
    if (connect->request_len == TKO_ERROR)
    {
        TKO_ERROR_PRINTF( ("%s: Creating Request FAILED\n", __func__) );
        return TKO_ERROR;
    }
    datalen += connect->request_len;
    keep_alive_offload->seqnum = keep_alive_offload->seqnum + 1;

    /* Create Response Packet */
    connect->response_len = prep_packet(keep_alive_offload, index, &connect->data[datalen]);
    if (connect->request_len == TKO_ERROR)
    {
        TKO_ERROR_PRINTF( ("%s: Creating Response FAILED\n", __func__) );
        return TKO_ERROR;
    }
    datalen += connect->response_len;

    return (offsetof(wl_tko_connect_t, data) + datalen);
}

#ifdef __cplusplus
}
#endif

