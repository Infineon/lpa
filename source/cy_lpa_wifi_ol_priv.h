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
* @file cy_lpa_wifi_ol_priv.h
* @brief Offload API called by Offload Manager.
*/

#ifndef OL_PRIV_H__
#define OL_PRIV_H__  (1)

#ifdef __cplusplus
extern "C" {
#endif


#include "cy_lpa_wifi_ol_common.h"  /* for ol_info_t */
#include "cy_lpa_wifi_olm.h"        /* for olm_t */


/******************************************************************************/
/** \addtogroup group_lpa_internal *//** \{ */
/******************************************************************************/

/**
 * Offload host power-mode status.
 */
typedef enum ol_pm_st
{
    OL_PM_ST_GOING_TO_SLEEP,    /**< Host is going to sleep. */
    OL_PM_ST_AWAKE,            /**< Host is awake. */
    OL_PM_ST_MAX,            /**< Maximum power-mode status value; invalid power-mode status. */
} ol_pm_st_t;

/**< Offload initialization function
 *
 * Offload will cache or interpret configuration parameter and
 * register for callbacks.  IOVARs may be called through the
 * whd pointer.
 *
 * Returns RESULT_UNSUPPORTED if WiFi device firmware does not
 * support the offload.
 */
typedef int (ol_init_t)(void *ol, ol_info_t *ol_info, const void *cfg);

/**< Offload de-initialization function
 *
 * Offload will free resource and unregister callbacks.
 */
typedef void (ol_deinit_t)(void *ol);

/**< Offload host power-mode changed handler function
 *
 * Offload is notified that the host power-mode is changing
 * and has an opportunity to change its internal state.
 */
typedef void (ol_pm_t)(void *ol, ol_pm_st_t pm_state);

/**< Offload function pointer table
 *
 * Offload must define an implementation for each table entry.
 */
struct ol_fns
{
    ol_init_t *init;        /**< Offload initialization function. */
    ol_deinit_t *deinit;    /**< Offload deinitialization function. */
    ol_pm_t *pm;            /**< Offload host power-mode changed handler function. */
};

/**< Offload power management notification */
void cylpa_olm_dispatch_pm_notification(olm_t *olm, ol_pm_st_t st);

/** \} */

#ifdef __cplusplus
}
#endif
#endif /* !OL_PRIV_H__ */

