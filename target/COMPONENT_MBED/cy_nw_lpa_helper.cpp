/*
 * $ Copyright Cypress Semiconductor Apache2 $
 * 
 */

/** @file
 *
 * Network IP change utility for MBEDOS
 */
#include <stddef.h>
#include "cy_nw_lpa_helper.h"

#ifdef __cplusplus
extern "C" {
#endif

static cy_nw_ip_status_change_callback_t *cy_ip_status_change_callback = NULL;

void cylpa_nw_ip_initialize_status_change_callback(cylpa_nw_ip_status_change_callback_t *cb, cylpa_nw_ip_status_change_callback_func_t *cb_func, void *arg)
{
    cy_ip_status_change_callback = (cy_nw_ip_status_change_callback_t *)cb;
    cy_ip_status_change_callback->arg = arg;
    cy_ip_status_change_callback->cb_func = ( cy_nw_ip_status_change_callback_func_t *)cb_func;
    cy_ip_status_change_callback->priv = NULL;
    cy_nw_ip_initialize_status_change_callback(cy_ip_status_change_callback, cb_func, arg);
}

void cylpa_nw_ip_register_status_change_callback(cy_nw_ip_interface_t nw_interface, cylpa_nw_ip_status_change_callback_t *cb)
{
    cy_nw_ip_register_status_change_callback(nw_interface, (cy_nw_ip_status_change_callback_t *)cb);
}

void cylpa_nw_ip_unregister_status_change_callback(cy_nw_ip_interface_t nw_interface, cylpa_nw_ip_status_change_callback_t *cb)
{
    cy_nw_ip_unregister_status_change_callback(nw_interface, (cy_nw_ip_status_change_callback_t *)cb);
}

#ifdef __cplusplus
}
#endif
