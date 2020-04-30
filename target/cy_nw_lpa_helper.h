/*
 * $ Copyright Cypress Semiconductor Apache2 $
 * 
 */

/** @file
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
