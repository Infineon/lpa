/*
 * Copyright 2025, Cypress Semiconductor Corporation (an Infineon company) or
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
* @file cy_whd_tls_api.h
* @brief WHD MQTT TLS Offload API.
*/

#ifndef WHD_TLSOE_API_H__
#define WHD_TLSOE_API_H__  (1)

#include "cy_lpa_wifi_tls_ol.h"
#ifdef __cplusplus
extern "C" {
#endif

/** Get TCP socket sequence number info from network stack.  */
typedef struct  cy_tls_sock_seq
{
    whd_mac_t src_mac;  /**< local mac address */
    whd_mac_t dst_mac;  /**< remote mac address */
    uint32_t srcip;     /**< local IP address */
    uint32_t dstip;     /**< destination IP address */
    uint16_t srcport;   /**< Local TCP port */
    uint16_t dstport;   /**< Remote TCP port */
    uint32_t seqnum;    /**< Current sequence number for this socket */
    uint32_t acknum;    /**< Current ack number for this socket */
    uint16_t rx_window; /**< Current TCP rx window size */
} cy_tls_sock_seq_t;

whd_result_t
whd_tlsoe_update_sock_seq(uint16_t local_port, uint16_t remote_port, const char *remote_ip, uint32_t seq_num, uint32_t ack_num, uint8_t reset);

whd_result_t
whd_tlsoe_get_sock_stats(cy_tls_sock_seq_t *seq, uint16_t local_port, uint16_t remote_port, const char *remote_ip);

whd_result_t
whd_tlsoe_set_wowl_pattern(whd_t *whd, uint8_t* pattern, uint16_t pattern_len, uint16_t pattern_type, uint16_t patttern_offset);

whd_result_t
whd_tlsoe_del_wowl_pattern(whd_t *whd, uint8_t* pattern, uint16_t pattern_size, uint16_t pattern_type,  uint16_t patttern_offset);

whd_result_t
whd_tlsoe_disable(whd_t *whd, uint16_t local_port, uint16_t remote_port, const char *remote_ip, void* socket);

whd_result_t
whd_tlsoe_activate(whd_t *whd, uint16_t local_port, uint16_t remote_port, const char *remote_ip, uint32_t interval, uint8_t* pkt, uint32_t pkt_len, void* socket);

#ifdef __cplusplus
}
#endif

#endif /* !WHD_TLSOE_API_H__ */
