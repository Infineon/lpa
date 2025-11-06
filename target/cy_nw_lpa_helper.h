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
 * 
 */

/** @file cy_nw_lpa_helper.h
 * 
 * This is a collection of LPA network IP change functions used by LPA Middle-ware libraries.
 * 
 */
#pragma once
#include "cy_nw_helper.h"

#if defined(__cplusplus)
extern "C" {
#endif

/******************************************************************************/
/** \addtogroup group_nw_lpahelper_structures *//** \{ */
/******************************************************************************/
/** Network IP status change callback function
 *
 * @param[in] iface : Pointer to the network interface for which the callback is invoked.
 * @param[in] arg   : User data object provided during the status change callback registration.

 * @return none
 */
typedef void (cylpa_nw_ip_status_change_callback_func_t)(cy_nw_ip_interface_t iface, void *arg);

/** Network IP status change callback info */
typedef struct cylpa_nw_ip_status_change_callback
{
    cylpa_nw_ip_status_change_callback_func_t *cb_func; /**< IP address status change callback function */
    void *arg;                                    /**< User data */
    void *priv;                                   /**< NW interface */
} cylpa_nw_ip_status_change_callback_t;

/** \} */

/*****************************************************************************/
/**
 *
 *  @addtogroup group_nw_lpahelper_func
 *
 * This is a collection of network helper functions which would be used by various Cypress Middleware libraries.
 *
 *  @{
 */
/*****************************************************************************/

/** Initialize status change callback
 *
 * Initialize @ref cylpa_nw_ip_status_change_callback_t instead of
 * directly manipulating the callback struct.
 *
 * @param[in, out] info : Pointer to network IP status change callback information structure which would be filled upon return
 * @param[in] cbf       : Pointer to callback function to be invoked during network status change
 * @param[in] arg       : User data object to be sent sent in the callback function
 *
 * @return none
 */
void cylpa_nw_ip_initialize_status_change_callback(cylpa_nw_ip_status_change_callback_t *info, cylpa_nw_ip_status_change_callback_func_t *cbf, void *arg);

/** Registers IP status change callback
 *
 * @param[in] iface : Pointer to network interface object
 * @param[in] info  : Pointer to the status change information structure
 *
 * @return none
 */
void cylpa_nw_ip_register_status_change_callback(cy_nw_ip_interface_t iface, cylpa_nw_ip_status_change_callback_t *info);

/** Un-registers IP status change callback
 *
 * @param[in] iface : Pointer to network interface object
 * @param[in] info  : Pointer to the status change information structure
 *
 * @return none
 */
void cylpa_nw_ip_unregister_status_change_callback(cy_nw_ip_interface_t iface, cylpa_nw_ip_status_change_callback_t *info);

/** @} */

#if defined(__cplusplus)
}
#endif
