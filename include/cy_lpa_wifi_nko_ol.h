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
* @file   : cy_lpa_wifi_nko_ol.h
* @brief  : Defines the API into NAT Keepalive offloads from personality.
*/

#ifndef PF_NKO_H__
#define PF_NKO_H__  (1)

#include <stdint.h>
#include "cy_lpa_compat.h"
#include "cy_lpa_wifi_ol_common.h"
#include "whd.h"
#include "whd_wlioctl.h"


#ifdef __cplusplus
extern "C" {
#endif

/* NAT server address len */
#ifndef CYLPA_NAT_SERVER_NAME_LEN
#define CYLPA_NAT_SERVER_NAME_LEN                         (127)
#endif

/* Max NAT Keep alive payload length */
#ifndef CYLPA_NAT_KEEPALIVE_MAX_PAYLOAD
#define CYLPA_NAT_KEEPALIVE_MAX_PAYLOAD                   (127)
#endif

/** User uses configurator to set these */
typedef struct cy_lpa_nko_cfg
{
    uint16_t interval;                                  /**< NAT KeepAlive interval in milliseconds     */
    char     server[CYLPA_NAT_SERVER_NAME_LEN+1];       /**< Server Name / IP address                   */
    uint16_t d_port;                                    /**< UDP Destination Port                       */
    uint16_t s_port;                                    /**< UDP Source Port                            */
    char     payload[CYLPA_NAT_KEEPALIVE_MAX_PAYLOAD+1];/**< Keepalive Payload                          */
} cy_lpa_nko_ol_cfg_t;

/** Keep pointers to config space, system handle, etc */
typedef struct nko_ol
{
    cy_lpa_nko_ol_cfg_t  *cfg;                          /**< Pointer to config space                        */
    void                 *whd;                          /**< Pointer to system handle                       */
    ol_info_t            *ol_info_ptr;                  /**< Offload Manager Info structure  \ref ol_info_t */
} nko_ol_t;


extern const ol_fns_t nko_ol_fns;

/*********************************************************************************************
*
*  Functions
*
*********************************************************************************************/
int32_t cylpa_nko_ol_update_config(cy_lpa_nko_ol_cfg_t *cfg);

#ifdef __cplusplus
}
#endif

#endif /* !TKO_OL_H__ */

