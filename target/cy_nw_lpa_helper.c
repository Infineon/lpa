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

/** @file cy_nw_lpa_helper.c
 *
 * Network IP change utility for FreeRTOS
 */
#include "cy_nw_lpa_helper.h"
#include "cy_network_mw_core.h"
#include "cy_wcm.h"
#include <stddef.h>

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
#if defined(COMPONENT_LWIP)
    struct netif *iface = cy_network_get_nw_interface( CY_NETWORK_WIFI_STA_INTERFACE, CY_NETWORK_WIFI_STA_INTERFACE );
#elif defined(COMPONENT_NETXDUO)
    NX_IP *iface = (NX_IP *)cy_network_get_nw_interface( CY_NETWORK_WIFI_STA_INTERFACE, CY_NETWORK_WIFI_STA_INTERFACE);
#endif
    if ( cb->cb_func != NULL )
    {
        cb->priv = iface;
        cb->cb_func((cy_nw_ip_interface_t)(cb->priv), cb->arg);
    }
}

void cylpa_nw_ip_initialize_status_change_callback(cylpa_nw_ip_status_change_callback_t *cb, cylpa_nw_ip_status_change_callback_func_t *cb_func, void *arg)
{
#if defined(COMPONENT_LWIP)
    struct netif *iface = cy_network_get_nw_interface( CY_NETWORK_WIFI_STA_INTERFACE, CY_NETWORK_WIFI_STA_INTERFACE );
#elif defined(COMPONENT_NETXDUO)
    NX_IP *iface = (NX_IP *)cy_network_get_nw_interface( CY_NETWORK_WIFI_STA_INTERFACE, CY_NETWORK_WIFI_STA_INTERFACE);
#endif

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
