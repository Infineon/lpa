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
* @file   : cy_lpa_common_priv.h
* @brief  : LPA private definitions and functions
*/

#ifndef LPA_COMMON_PRIV_H__
#define LPA_COMMON_PRIV_H__  (1)

#ifdef __cplusplus
extern "C" {
#endif
#include "whd_types.h"
#include "whd_wlioctl.h"

/********************************************************************
*
*  Defines
*
********************************************************************/
#define CYLPA_IPV4_ADDR_LEN               4      /* IPV4 address length   */

/* IPV4 field decodes */
#define CYLPA_IPV4_VER_SHIFT              4      /* IPV4 version shift */
#define CYLPA_IP_VER_4                    4      /* version number for IPV4 */
#define CYLPA_IPV4_MIN_HEADER_LEN         20     /* Minimum size for an IP header (no options) */
#define CYLPA_IPV4_VER_HL_OFFSET          0      /* version and ihl byte offset */
#define CYLPA_IPV4_HLEN_MASK              0x0f   /* IPV4 header length mask */
#ifndef CYLPA_IPV4_HLEN
#define CYLPA_IPV4_HLEN(ipv4_body)        (4 * ( ( (uint8_t *)(ipv4_body) )[CYLPA_IPV4_VER_HL_OFFSET] & CYLPA_IPV4_HLEN_MASK ) )
#endif
#define CYLPA_IP_PROTO_TCP                6      /* TCP PROTOCOL */
#define CYLPA_IP_PROTO_UDP                17     /* UDP PROTOCOL */

#define CYLPA_UDP_HLEN                    8

/* 8bit TCP flag field */
#define CYLPA_TCP_FLAG_ACK                0x10
#define CYLPA_TCP_FLAG_PSH                0x08
#define CYLPA_TCP_FLAG_RST                0x04
#define CYLPA_TCP_FLAG_SYN                0x02
#define CYLPA_TCP_FLAG_FIN                0x01

#define CYLPA_TCP_MIN_HEADER_LEN          20
#define CYLPA_TCP_HDRLEN_SHIFT            4

/********************************************************************
 *
 *  Structures
 *
 *********************************************************************/
#pragma pack(1)
typedef struct
{
    uint8_t ether_dhost[ETHER_ADDR_LEN];
    uint8_t ether_shost[ETHER_ADDR_LEN];
    uint16_t ether_type;
} whd_ether_header_t;

/* IP V4 Header */
typedef struct
{
    uint8_t version_ihl;                    /* Version and Internet Header Length */
    uint8_t tos;                            /* Type Of Service */
    uint16_t tot_len;                       /* Number of bytes in packet (max 65535) */
    uint16_t id;
    uint16_t frag;                          /* 3 flag bits and fragment offset */
    uint8_t ttl;                            /* Time To Live */
    uint8_t prot;                           /* Protocol */
    uint16_t hdr_chksum;                    /* IP header checksum */
    uint8_t src_ip[CYLPA_IPV4_ADDR_LEN];    /* Source IP Address */
    uint8_t dst_ip[CYLPA_IPV4_ADDR_LEN];    /* Destination IP Address */
} ipv4_hdr_t;

/* TCP Header... used for checksumming */
typedef struct tcp_pseudo_hdr
{
    uint8_t src_ip[CYLPA_IPV4_ADDR_LEN];    /* Source IP Address */
    uint8_t dst_ip[CYLPA_IPV4_ADDR_LEN];    /* Destination IP Address */
    uint8_t zero;
    uint8_t prot;
    uint16_t tcp_size;
} tcp_pseudo_hdr_t;

/* These fields are stored in network order */
typedef struct bcmtcp_hdr
{
    uint16_t src_port;                      /* Source Port Address */
    uint16_t dst_port;                      /* Destination Port Address */
    uint32_t seq_num;                       /* TCP Sequence Number */
    uint32_t ack_num;                       /* TCP Sequence Number */
    uint16_t hdrlen_rsvd_flags;             /* Header length, reserved bits and flags */
    uint16_t tcpwin;                        /* TCP window */
    uint16_t chksum;                        /* Segment checksum with pseudoheader */
    uint16_t urg_ptr;                       /* Points to seq-num of byte following urg data */
} bcmtcp_hdr_t;


typedef struct udp_hdr
{
    uint16_t src_port;                      /* Source Port Address */
    uint16_t dst_port;                      /* Destination Port Address */
    uint16_t len;                           /* UDP Len */
    uint16_t chksum;                        /* UDP Checksum */
} udp_hdr_t;
#pragma pack()

/*********************************************************************************************
*
*  Function Prototypes
*
*********************************************************************************************/
uint16_t cylpa_ipv4_hdr_cksum(uint8_t *ip, int ip_len);

uint16_t cylpa_ipv4_tcp_hdr_cksum(uint8_t *ip, uint8_t *tcp, uint16_t tcp_len);

uint16_t cylpa_udp_cksum(uint16_t length, uint8_t *addrs, uint8_t *buf);

#ifdef __cplusplus
}
#endif

#endif /* LPA_COMMON_PRIV_H__ */