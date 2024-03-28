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
* @file cy_lpa_wifi_pf_ol.h
* @brief Defines the API into PktFilter offloads from personality.
*/

#ifndef PF_TKO_H__
#define PF_TKO_H__  (1)

#include <stdint.h>
#include "cy_lpa_compat.h"
#include "cy_lpa_wifi_ol_common.h"
#include "whd.h"
#include "whd_wlioctl.h"
#define MAX_TKO MAX_TKO_CONN

#ifdef __cplusplus
extern "C" {
#endif


/** User uses configurator to set these */
typedef struct cy_tko_connect
{
    uint16_t    local_port;         /**< Local port num for this socket */
    uint16_t    remote_port;        /**< Remote port num for this socket */
    char        remote_ip[16];      /**< IP address of remote side */
} cy_tko_ol_connect_t;

/** User uses configurator to set these */
typedef struct cy_tko_cfg
{
    uint16_t interval;                  /**< Interval (in seconds) between keepalives */
    uint16_t retry_interval;            /**< If a keepalive is not Acked, retry after this many seconds */
    uint16_t retry_count;               /**< Retry up to this many times */
    cy_tko_ol_connect_t ports[MAX_TKO]; /**< Port and IP address ofr each connection */
} cy_tko_ol_cfg_t;

/** Keep pointers to config space, system handle, etc */
typedef struct tko
{
    cy_tko_ol_cfg_t  *cfg;          /**< Pointer to config space */
    void             *whd;          /**< Pointer to system handle */
    ol_info_t        *ol_info_ptr;  /**< Offload Manager Info structure  \ref ol_info_t */
} tko_ol_t;


extern const ol_fns_t tko_ol_fns;

/** @addtogroup lpautilities LPA Utilities API
 * \{
 *
 *
 */

/**
 * Configure the user provided TCP keep alive parameters to LPA middleware.
 *
 * @param[in] remote_ip   Pointer to the destination IP address.
 * @param[in] remote_port Destination port number.
 * @param[in] local_port  Source port number.
 * @param[in] cfg         Structure containing user provided parameters.
 *
 * @return 0 on success; negative on failure.
 *
 */
int cylpa_tko_ol_update_config(const char *remote_ip, uint16_t remote_port, uint16_t local_port, cy_tko_ol_cfg_t *cfg );
/** \} */
#ifdef __cplusplus
}
#endif

#endif /* !TKO_OL_H__ */

