/*
 * $ Copyright Cypress Semiconductor Apache2 $
 * 
 */

/** @file
 *
 * Network IP change utility for FreeRTOS
 */
#include "cy_nw_lpa_helper.h"
#include "cy_lwip.h"
#include "cy_wcm.h"

#ifdef __cplusplus
extern "C" {
#endif

cylpa_nw_ip_status_change_callback_t cylpa_ip_status_change_callback = {
    .arg = NULL,
    .cb_func = NULL,
    .priv = NULL
};

void cylpa_nw_ip_status_change_handler ( cylpa_nw_ip_status_change_callback_t *cb );
static void cy_wcm_event_callback_func (cy_wcm_event_t event, cy_wcm_event_data_t *event_data);
static bool is_wcm_callback_registered = false;

void cylpa_nw_ip_status_change_handler ( cylpa_nw_ip_status_change_callback_t *cb )
{
    struct netif *iface = cy_lwip_get_interface( CY_LWIP_STA_NW_INTERFACE );
    if ( cb->cb_func != NULL )
    {
        cb->priv = iface;
        cb->cb_func((cy_nw_ip_interface_t)(cb->priv), cb->arg);
    }
}

void cylpa_nw_ip_initialize_status_change_callback(cylpa_nw_ip_status_change_callback_t *cb, cylpa_nw_ip_status_change_callback_func_t *cb_func, void *arg)
{
    struct netif *iface = cy_lwip_get_interface( CY_LWIP_STA_NW_INTERFACE );

    cb->cb_func = cb_func;
    cb->arg = arg;
    cb->priv = iface;

    cylpa_ip_status_change_callback.arg = arg;
    cylpa_ip_status_change_callback.cb_func = cb_func;
    cylpa_ip_status_change_callback.priv = iface;
}

void cylpa_nw_ip_register_status_change_callback(cy_nw_ip_interface_t nw_interface, cylpa_nw_ip_status_change_callback_t *cb)
{
    if ( is_wcm_callback_registered == false )
    {
        cy_wcm_register_event_callback(&cy_wcm_event_callback_func);
        is_wcm_callback_registered = true;
    }
}

void cylpa_nw_ip_unregister_status_change_callback(cy_nw_ip_interface_t nw_interface, cylpa_nw_ip_status_change_callback_t *cb)
{
    if (is_wcm_callback_registered == true )
    {
        cy_wcm_deregister_event_callback(&cy_wcm_event_callback_func);
        is_wcm_callback_registered = false;
    }
}

static void cy_wcm_event_callback_func (cy_wcm_event_t event, cy_wcm_event_data_t *event_data)
{
	if ( ( event_data != NULL) && (event == CY_WCM_EVENT_IP_CHANGED ))
	{
		if (event_data->ip_addr.version == CY_WCM_IP_VER_V4)
		{
			cylpa_nw_ip_status_change_handler(&cylpa_ip_status_change_callback);
		}
	}
}

#ifdef __cplusplus
}
#endif
