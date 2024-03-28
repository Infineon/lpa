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
* @file   : cy_lpa_wifi_nullko_ol.h
* @brief  : Defines the API into NULL Keepalive offloads from personality.
*/

#ifndef LPA_NULLKO_H__
#define LPA_NULLKO_H__  (1)

#include <stdint.h>
#include "cy_lpa_compat.h"
#include "cy_lpa_wifi_ol_common.h"
#include "whd.h"
#include "whd_wlioctl.h"


#ifdef __cplusplus
extern "C" {
#endif

#define NULL_KEEPALIVE_DEFALUT_INTERVAL_SEC                (110)

/** User uses configurator to set these */
typedef struct cy_nullko_cfg
{
    uint16_t interval;                              /**< NULL KeepAlive interval in seconds     */
} cy_nullko_ol_cfg_t;

/** Keep pointers to config space, system handle, etc */
typedef struct nullko_ol
{
    cy_nullko_ol_cfg_t  *cfg;                          /**< Pointer to config space                        */
    void                *whd;                          /**< Pointer to system handle                       */
    ol_info_t           *ol_info_ptr;                  /**< Offload Manager Info structure  \ref ol_info_t */
} nullko_ol_t;

extern const ol_fns_t nullko_ol_fns;

#ifdef __cplusplus
}
#endif

#endif /* !LPA_NULLKO_H__ */
