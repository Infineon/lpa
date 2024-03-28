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
* @file  : cy_lpa_common.c
* @brief : LPA common routines
*/

#include "cy_lpa_common_priv.h"
#include "whd_endian.h"

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************
*
*  Macros
*
********************************************************************/
#define OFFSETOF(type, member)  ( (uintptr_t)&( (type *)0 )->member )

#define CYLPA_COMMON_ERR        0xffed

/*******************************************************************************
 * Function Name: ip_cksum_partial
 *******************************************************************************
 *
 * calculate partial checksum
 *
 *******************************************************************************/
static uint32_t ip_cksum_partial(uint32_t sum, uint8_t *val8, uint32_t count)
{
    uint32_t i;
    uint16_t *val16 = (uint16_t *)val8;

    if ( (val8 == NULL) || ( (count % 2) != 0 ) )
    {
        return CYLPA_COMMON_ERR;
    }

    count /= 2;

    for (i = 0; i < count; i++)
    {
        sum += *val16++;
    }
    return sum;
}

/*******************************************************************************
 * Function Name: ip_cksum
 *******************************************************************************
 *
 * calculate IP checksum
 *
 *******************************************************************************/
static uint16_t ip_cksum(uint32_t sum, uint8_t *val8, uint32_t count)
{
    uint16_t *val16 = (uint16_t * )val8;

    if (val8 == NULL)
    {
        return CYLPA_COMMON_ERR;
    }

    while (count > 1)
    {
        sum += *val16++;
        count -= 2;
    }

    /*  add left-over byte, if any */
    if (count > 0)
    {
        sum += (*(uint8_t *)val16);
    }

    /*  fold 32-bit sum to 16 bits */
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return ( (uint16_t) ~sum );
}

/*******************************************************************************
 * Function Name: tcp_hdr_chksum
 *******************************************************************************
 *
 * calculate TCP header checksum using partial sum
 *
 *******************************************************************************/
static uint16_t tcp_hdr_chksum(uint32_t sum, uint8_t *tcp_hdr, uint16_t tcp_len)
{
    uint8_t *ptr = tcp_hdr;

    if (tcp_len < CYLPA_TCP_MIN_HEADER_LEN)
    {
        return CYLPA_COMMON_ERR;
    }

    /* partial TCP cksum skipping the chksum field */
    sum = ip_cksum_partial(sum, ptr, offsetof(bcmtcp_hdr_t, chksum) );
    ptr += offsetof(bcmtcp_hdr_t, chksum) + 2;

    /* return calculated chksum */
    return ip_cksum(sum, ptr, tcp_len - offsetof(bcmtcp_hdr_t, urg_ptr) );
}

/*******************************************************************************
 * Function Name: cylpa_ipv4_hdr_cksum
 *******************************************************************************
 *
 * calculate IPv4 header checksum
 * - input ip points to IP header in network order
 * - output cksum is in network order
 *
 *******************************************************************************/
uint16_t cylpa_ipv4_hdr_cksum(uint8_t *ip, int ip_len)
{
    uint32_t sum = 0;
    uint8_t *ptr = ip;

    if ( (ip == NULL) || (ip_len < CYLPA_IPV4_MIN_HEADER_LEN) )
    {
        return CYLPA_COMMON_ERR;
    }

    /* partial cksum skipping the hdr_chksum field */
    sum = ip_cksum_partial(sum, ptr, OFFSETOF(ipv4_hdr_t, hdr_chksum) );
    ptr += OFFSETOF(ipv4_hdr_t, hdr_chksum) + 2;

    /* return calculated chksum */
    return ip_cksum(sum, ptr, ip_len - OFFSETOF(ipv4_hdr_t, src_ip) );
}

/*******************************************************************************
 * Function Name: cylpa_ipv4_tcp_hdr_cksum
 *******************************************************************************
 *
 * calculate IPv4 TCP header checksum
 * - input ip and tcp points to IP and TCP header in network order
 * - output cksum is in network order
 *
 *******************************************************************************/
uint16_t cylpa_ipv4_tcp_hdr_cksum(uint8_t *ip, uint8_t *tcp, uint16_t tcp_len)
{
    ipv4_hdr_t *ip_hdr = (ipv4_hdr_t *)ip;
    tcp_pseudo_hdr_t tcp_ps;
    uint32_t sum = 0;

    if (tcp_len < CYLPA_TCP_MIN_HEADER_LEN)
    {
        return CYLPA_COMMON_ERR;
    }

    /* pseudo header cksum */
    memset(&tcp_ps, 0, sizeof(tcp_ps) );
    memcpy(&tcp_ps.dst_ip, ip_hdr->dst_ip, CYLPA_IPV4_ADDR_LEN);
    memcpy(&tcp_ps.src_ip, ip_hdr->src_ip, CYLPA_IPV4_ADDR_LEN);
    tcp_ps.zero = 0;
    tcp_ps.prot = ip_hdr->prot;
    tcp_ps.tcp_size = hton16(tcp_len);
    sum = ip_cksum_partial(sum, (uint8_t *)&tcp_ps, sizeof(tcp_ps) );

    /* return calculated TCP header chksum */
    return tcp_hdr_chksum(sum, tcp, tcp_len);
}

/*******************************************************************************
 * Function Name: cylpa_checksum_add
 *******************************************************************************
 *
 * Add Buffer to checksum calculation
 *
 *******************************************************************************/
static uint32_t cylpa_checksum_add(uint16_t len, uint8_t *buf)
{
    uint32_t sum = 0;
    uint16_t i;

    for (i = 0; i < len; i++)
    {
        if (i & 1)
        {
            sum += (uint32_t)buf[i];
        }
        else
        {
            sum += (uint32_t)buf[i] << 8;
        }
    }
    return sum;
}

/*******************************************************************************
 * Function Name: cylpa_checksum_finish
 *******************************************************************************
 *
 * Compute final checksum
 *
 *******************************************************************************/
static uint16_t cylpa_checksum_finish(uint32_t sum)
{
    while (sum>>16)
        sum = (sum & 0xFFFF)+(sum >> 16);

    return ~sum;
}

/*******************************************************************************
 * Function Name: cylpa_udp_cksum
 *******************************************************************************
 *
 * Calculate UDP checksum
 *
 *******************************************************************************/
uint16_t cylpa_udp_cksum(uint16_t length, uint8_t *addrs, uint8_t *buf)
{
    uint32_t sum = 0;
    uint16_t chksum;

    /* Payload */
    sum += cylpa_checksum_add(length, buf);

    /* Source IP + Dest IP */
    sum += cylpa_checksum_add(CYLPA_IPV4_ADDR_LEN * 2, addrs);

    /* UDP + Length */
    sum += CYLPA_IP_PROTO_UDP + length;

    chksum = cylpa_checksum_finish(sum);

    /* chksum zero must become 0xffff, as zero means 'no checksum' */
    if ( chksum == 0x0000 )
    {
        chksum = 0xffff;
    }
    return chksum;
}

#ifdef __cplusplus
}
#endif

