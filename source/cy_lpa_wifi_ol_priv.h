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

