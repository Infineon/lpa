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
* @file cy_whd_tko_api.h
* @brief WHD TKO Offload API.
*/


#ifndef WHD_TKO_API_H__
#define WHD_TKO_API_H__  (1)

#include "cy_lpa_wifi_tko_ol.h"
#ifdef __cplusplus
extern "C" {
#endif

#define IP_ADDR_STATS(ipaddr)   (ipaddr.u_addr.ip4.addr)

/** Get TCP socket sequence number info from network stack.  */
typedef struct sock_seq
{
    whd_mac_t src_mac; /**< local mac address */
    whd_mac_t dst_mac; /**< remote mac address */
    uint32_t srcip;    /**< local IP address */
    uint32_t dstip;    /**< destination IP address */
    uint16_t srcport;  /**< Local TCP port */
    uint16_t dstport;  /**< Remote TCP port */
    uint32_t seqnum;   /**< Current sequence number for this socket */
    uint32_t acknum;   /**< Current ack number for this socket */
    uint16_t rx_window; /**< Current TCP rx window size */
} sock_seq_t;

whd_result_t
sock_stats(sock_seq_t *seq, uint16_t local_port, uint16_t remote_port, const char *remote_ip);

whd_result_t
whd_tko_activate(whd_t *whd, uint8_t index, uint16_t local_port, uint16_t remote_port, const char *remote_ip);

whd_result_t
whd_tko_enable(whd_t *whd);

whd_result_t
whd_tko_disable(whd_t *whd);

#ifdef __cplusplus
}
#endif

#endif /* !WHD_TKO_API_H__ */

