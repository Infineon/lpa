/*
 * Copyright 2025, Cypress Semiconductor Corporation (an Infineon company) or
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
* @file network_activity_handler.c
* @brief This file implements the functions needed to suspend
* network stack and wait till external event or timeout
*/

#include "network_activity_handler.h"
#include "cyabs_rtos.h"
#include "cy_network_mw_core.h"

#include "cy_OlmInterface.h"
#include "whd_int.h"
#ifndef COMPONENT_CAT5
#include <cycfg_system.h>
#include "cycfg.h"
#endif
#include "cyhal.h"
#ifdef COMPONENT_LWIP
#include "cy_network_buffer.h"
/* lwIP header files */
#include <lwip/tcpip.h>
#include <lwip/api.h>
#include <lwip/tcp.h>
#include <lwip/priv/tcp_priv.h>
#endif

#ifdef COMPONENT_NETXDUO
#include "nx_api.h"
#include "tx_api.h"
#include "nxd_dhcp_client.h"
#endif

#include "cy_nw_helper.h"

#include "cy_wcm.h"

/* secure socket Header files */
#include "cy_secure_sockets.h"
#include "cy_tls.h"

/******************************************************
 *                      Macros
 ******************************************************/

#define IDLE_POWER_MODE_STRING_LEN (32)

#define PACKET_PAYLOAD 1500
/* TCP user data buffer to send to server */
static uint8_t cy_tcp_databuf[PACKET_PAYLOAD];
#define HARDCODED_STR   "Some random stuff"

#if defined(COMPONENT_LWIP)
#define RX_PKT_Q_LENGTH            (20)
#endif
/******************************************************
 *                     Variable Definitions
 ******************************************************/
#if defined(COMPONENT_LWIP)
static bool is_rx_pkt_queue_created = false;
static cy_queue_t rx_pkt_q_handle;
#endif

/******************************************************
 *                     Structures
 ******************************************************/
#if defined(COMPONENT_LWIP)
typedef struct cylpa_rx_queue
{
    void *iface;
    void* buf;
}cylpa_rx_queue_t;
#endif

extern whd_interface_t cy_olm_get_whd_interface ( void );

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/

/*******************************************************************************
* Function Name: cylpa_get_idle_power_mode
********************************************************************************
*
* Summary: This function is used to get the currently configured MCU low power
* mode, and copy it as a string into the first argument.
*
* Parameters:
* char *str: Pointer to character array where idle power mode string should be copied
* uint32_t length of character array passed in argument 1.
*
* Return:
* void.
*
*******************************************************************************/
static void cylpa_get_idle_power_mode(char *str, uint32_t length);

/*******************************************************************************
* Function Name: cylpa_print_whd_bus_stats
********************************************************************************
*
* Summary: This function is used to print the WHD bus stats, one of the useful
* the information includes no of WLAN Out-of-Band interrupts and In-band interrupts.
*
* Parameters:
* void *iface: pointer to WLAN interface(pointer to network stack specific interface)
*
* Return:
* void.
*
*******************************************************************************/
static void cylpa_print_whd_bus_stats(void *iface);

/*******************************************************************************
* Function Name: cylpa_register_network_activity_callback
********************************************************************************
*
* Summary: This function is used to register a callback for the event of net
* activity.
*
* Return:
* cy_rslt_t: contains status of attaching callback.
*
*******************************************************************************/
static cy_rslt_t cylpa_register_network_activity_callback(void);

/*******************************************************************************
* Function Name: cylpa_network_state_handler
********************************************************************************
*
* Summary: This function prints state of the network stack on the terminal
  emulator.
*
*
* Parameters:
* cy_rslt_t state: contains the state of the network stack as defined in the
* enumeration, network_stack_state_t
*
*******************************************************************************/
static void cylpa_network_state_handler(cy_rslt_t state);

/*******************************************************************************
* Function Name: cylpa_network_act_handler_init
********************************************************************************
*
* Summary:
* In this function Callback is registered for Deep Sleep monitoring and
* can be used for lock/unlock deep sleep.Also this registers for network
* activity handling
*
* Parameters:
* void
*
* Return:
* void
*
*******************************************************************************/
static void cylpa_network_act_handler_init(void);

/******************************************************
 *               Variable Definitions
 ******************************************************/

/*  This variable is used as a check to prevent successive locking/unlocking
    of the TCP/IP stack. Locking the TCP/IP stack prevents the MCU from servicing
    the network timers. This enables the MCU to stay in deep sleep longer.
*/

static bool cylpa_s_ns_suspended;

/*  This event variable is used to alert the device if there is any network
    activity.
*/
cy_event_t cy_lp_wait_net_event;

/* This bool variable tracks if cy_lp_wait_net_event is initialized or not.*/
bool cy_lp_wait_net_event_valid;

/*  This is mutex to sychronize multiple threads */
cy_mutex_t cy_lp_mutex;

/* This variable is used to track total time spent in deep sleep */
uint32_t cy_dsleep_nw_suspend_time;

/* This variable is used to store the callback pointer provided by application */
static cylpa_suspend_callback_t suspend_notify_cb;

/* This variable is used to store the callback user data provided by application */
static void *cb_data;

/******************************************************
 *               Function Definitions
 ******************************************************/
void cylpa_network_act_handler_init(void)
{
    /* Register for network activity callback */
    cylpa_register_network_activity_callback();
}

static void cylpa_get_idle_power_mode(char *str, uint32_t length)
{
    switch (CY_CFG_PWR_SYS_IDLE_MODE)
    {
#ifndef COMPONENT_CAT5
        case CY_CFG_PWR_MODE_LP:
            strncpy(str, "LP", length);
            break;
        case CY_CFG_PWR_MODE_ULP:
            strncpy(str, "ULP", length);
            break;
#endif
        case CY_CFG_PWR_MODE_ACTIVE:
            strncpy(str, "Active", length);
            break;
        case CY_CFG_PWR_MODE_SLEEP:
            strncpy(str, "Sleep", length);
            break;
        case CY_CFG_PWR_MODE_DEEPSLEEP:
            strncpy(str, "DeepSleep", length);
            break;
        default:
            strncpy(str, "Active", length);
            break;
    }
}


static void cylpa_print_whd_bus_stats(void *iface)
{
#if defined(COMPONENT_LWIP)
    struct netif *wifi;
    whd_interface_t ifp = NULL;
#elif defined(COMPONENT_NETXDUO)
    NX_IP *wifi;
    whd_interface_t ifp = NULL;
    UINT res;
    UINT status;
#endif

#if defined(COMPONENT_LWIP)
    wifi = (struct netif *)iface;
    if (wifi->flags & NETIF_FLAG_UP)
#elif defined(COMPONENT_NETXDUO)
    wifi = (NX_IP *)iface;
    res = nx_ip_status_check(wifi, NX_IP_ADDRESS_RESOLVED, (ULONG *)&status, NX_IP_PERIODIC_RATE);
    if(res == NX_SUCCESS)
#endif
    {
        ifp = cy_olm_get_whd_interface();
        if (ifp == NULL)
        {
           return;
        }

        NW_INFO(("\n=====================================================\n"));
        (void)whd_print_stats(ifp->whd_driver, WHD_FALSE);
        NW_INFO(("=====================================================\n"));
    }
    else
    {
        NW_INFO(("Wi-Fi interface is not powered on, bus_stats are unavailable\n"));
    }
}

void cylpa_on_emac_activity(bool is_tx_activity)
{
    if (cy_lp_wait_net_event_valid)
    {
        cy_rtos_setbits_event(&cy_lp_wait_net_event,
            (uint32_t)(is_tx_activity ? CY_LPA_TX_EVENT_FLAG : CY_LPA_RX_EVENT_FLAG), true);
    }
}

#if defined(COMPONENT_LWIP)

static void cylpa_rx_activity(void *iface, void* buf)
{
    /* Push the incoming packet to the RX queue maintained by LPA during the wake up sequence
     * until the network stack is ready to process the incoming packets.
     * This will unblock the WHD thread which is providing the RX packets. */
    cylpa_rx_queue_t queue_item;
    cy_rslt_t result;

    queue_item.iface = iface;
    queue_item.buf = buf;
    result = cy_rtos_queue_put(&rx_pkt_q_handle, &queue_item, 0);
    if(result != CY_RSLT_SUCCESS)
    {
        NW_INFO(("Release the buffer as we are unable to push to the queue. This packet is lost\n"));
        cy_buffer_release(buf, WHD_NETWORK_RX) ;
    }
    return;
}

static void cylpa_rx_queue_pkt_input()
{
    /* This function pushes all the packets queued by the WHD thread during the wake up sequence.
     * Packets will be pushed to the lwip queue before resuming the network stack.
     */
    size_t remaining_items_in_queue = 0;
    cylpa_rx_queue_t queue_item;
    cy_rslt_t result;

    cy_rtos_queue_count(&rx_pkt_q_handle, &remaining_items_in_queue);
    while(remaining_items_in_queue)
    {
        result = cy_rtos_queue_get(&rx_pkt_q_handle, &queue_item, 0);
        if(result == CY_RSLT_SUCCESS)
        {
            if (((struct netif*)(queue_item.iface))->input == NULL || ((struct netif*)(queue_item.iface))->input(queue_item.buf, (struct netif*)queue_item.iface) != ERR_OK)
            {
                NW_INFO(("Drop packet as the lwip function call returned failure\n"));
                cy_buffer_release(queue_item.buf, WHD_NETWORK_RX);
            }
        }
        cy_rtos_queue_count(&rx_pkt_q_handle, &remaining_items_in_queue);
    }
}
#endif

static cy_rslt_t cylpa_register_network_activity_callback(void)
{
    cy_rslt_t result = cy_rtos_init_event(&cy_lp_wait_net_event);

    if (CY_RSLT_SUCCESS != result)
    {
        NW_INFO(("Failed to initialize Wait for Network Activity event.\n"));
        return result;
    }
    cy_lp_wait_net_event_valid = true;

    result  = cy_rtos_init_mutex(&cy_lp_mutex);

    if ( CY_RSLT_SUCCESS != result )
    {
        NW_INFO(("Failed to initialize Mutex \n"));
        return result;
    }
    else
    {
        /* Set EMAC activity callback for this module. */
        cy_network_activity_register_cb(cylpa_on_emac_activity);
        result = CY_RSLT_SUCCESS;
    }

    return result;
}

int32_t cylpa_suspend_ns(void)
{
    int32_t state;
#if defined(COMPONENT_NETXDUO)
    UINT    active;
    UINT    res;
    NX_IP   *ip_ptr;
    NX_DHCP *dhcp_ptr;
#endif

    if (true == cylpa_s_ns_suspended)
    {
        state = ST_BAD_STATE;
    }
    else
    {
#if defined(COMPONENT_LWIP)
        LOCK_TCPIP_CORE();
#elif defined(COMPONENT_NETXDUO)
        /* Get netxduo network interface pointer */
        ip_ptr = (NX_IP *)cy_network_get_nw_interface( CY_NETWORK_WIFI_STA_INTERFACE, CY_NETWORK_WIFI_STA_INTERFACE);
        if(ip_ptr == NULL)
        {
            /* network interface is not available. Suspending network stack failed. */
            state = ST_WIFI_NET_SUSPENDED_FAILED;
            return state;
        }

        /* Acquire the lock to prevent the netxduo IP thread from running */
        res = tx_mutex_get(&(ip_ptr -> nx_ip_protection), TX_WAIT_FOREVER);
        if(res != TX_SUCCESS)
        {
            /* Unable to acquire IP thread mutex. Suspending network stack failed. */
            state = ST_WIFI_NET_SUSPENDED_FAILED;
            return state;
        }

        /* Deactivate the netxduo timers to suspend the network stack */
        res = tx_timer_info_get(&ip_ptr->nx_ip_periodic_timer, TX_NULL, &active, TX_NULL, TX_NULL, TX_NULL);
        if((res == TX_SUCCESS) && (active == TX_TRUE))
        {
            tx_timer_deactivate(&(ip_ptr->nx_ip_periodic_timer));
        }
        res = tx_timer_info_get(&ip_ptr->nx_ip_fast_periodic_timer, TX_NULL, &active, TX_NULL, TX_NULL, TX_NULL);
        if((res == TX_SUCCESS) && (active == TX_TRUE))
        {
            tx_timer_deactivate(&(ip_ptr->nx_ip_fast_periodic_timer));
        }

        dhcp_ptr = cy_network_get_dhcp_handle(CY_NETWORK_WIFI_STA_INTERFACE, CY_NETWORK_WIFI_STA_INTERFACE);

        if (dhcp_ptr != NULL)
        {
            res = tx_mutex_get(&(dhcp_ptr -> nx_dhcp_mutex), TX_WAIT_FOREVER);
            if(res != TX_SUCCESS)
            {
                /* Unable to acquire DHCP mutex. Suspending network stack failed. */
                tx_timer_activate(&(ip_ptr->nx_ip_periodic_timer));
                tx_timer_activate(&(ip_ptr->nx_ip_fast_periodic_timer));
                tx_mutex_put(&(ip_ptr -> nx_ip_protection));
                state = ST_WIFI_NET_SUSPENDED_FAILED;
                return state;
            }

            res = tx_timer_info_get(&dhcp_ptr -> nx_dhcp_timer, TX_NULL, &active, TX_NULL, TX_NULL, TX_NULL);
            if((res == TX_SUCCESS) && (active == TX_TRUE))
            {
                tx_timer_deactivate(&(dhcp_ptr -> nx_dhcp_timer));
            }
        }
#endif
        cylpa_s_ns_suspended = true;
        state = ST_SUCCESS;
    }

    return state;
}

int32_t cylpa_resume_ns(void)
{
    int32_t state;
#if defined(COMPONENT_NETXDUO)
    UINT    active;
    UINT    res;
    NX_IP   *ip_ptr;
    NX_DHCP *dhcp_ptr;
#endif

    if (false == cylpa_s_ns_suspended)
    {
        state = ST_BAD_STATE;
    }
    else
    {
#if defined(COMPONENT_LWIP)
        UNLOCK_TCPIP_CORE();
#elif defined(COMPONENT_NETXDUO)

        dhcp_ptr = cy_network_get_dhcp_handle(CY_NETWORK_WIFI_STA_INTERFACE, CY_NETWORK_WIFI_STA_INTERFACE);
        if(dhcp_ptr != NULL)
        {
            res = tx_timer_info_get(&dhcp_ptr -> nx_dhcp_timer, TX_NULL, &active, TX_NULL, TX_NULL, TX_NULL);
            if((res == TX_SUCCESS) && (active == TX_FALSE))
            {
                tx_timer_activate(&(dhcp_ptr -> nx_dhcp_timer));
            }

            tx_mutex_put(&(dhcp_ptr -> nx_dhcp_mutex));
            if(res != TX_SUCCESS)
            {
                /* Unable to release IP thread mutex. Resuming network stack failed. */
                tx_timer_deactivate(&(dhcp_ptr -> nx_dhcp_timer));
                state = ST_WIFI_NET_RESUMING_FAILED;
                return state;
            }
        }

        /* Get netxduo network interface pointer */
        ip_ptr = (NX_IP *)cy_network_get_nw_interface( CY_NETWORK_WIFI_STA_INTERFACE, CY_NETWORK_WIFI_STA_INTERFACE);
        if(ip_ptr == NULL)
        {
            if(dhcp_ptr != NULL)
            {
                tx_mutex_get(&(dhcp_ptr -> nx_dhcp_mutex), TX_WAIT_FOREVER);
                tx_timer_deactivate(&(dhcp_ptr -> nx_dhcp_timer));
            }
            /* network interface is not available. Resuming network stack failed. */
            state = ST_WIFI_NET_RESUMING_FAILED;
            return state;
        }

        /* Activate the netxduo timers to resume the network stack */
        res = tx_timer_info_get(&ip_ptr->nx_ip_periodic_timer, TX_NULL, &active, TX_NULL, TX_NULL, TX_NULL);
        if((res == TX_SUCCESS) && (active == TX_FALSE))
        {
            tx_timer_activate(&(ip_ptr->nx_ip_periodic_timer));
        }

        res = tx_timer_info_get(&ip_ptr->nx_ip_fast_periodic_timer, TX_NULL, &active, TX_NULL, TX_NULL, TX_NULL);
        if((res == TX_SUCCESS) && (active == TX_FALSE))
        {
            tx_timer_activate(&(ip_ptr->nx_ip_fast_periodic_timer));
        }

        /* Release the lock to allow the netxduo IP thread to running */
        res = tx_mutex_put(&(ip_ptr -> nx_ip_protection));
        if(res != TX_SUCCESS)
        {
            /* Unable to release IP thread mutex. Resuming network stack failed. */
            tx_timer_deactivate(&(ip_ptr->nx_ip_periodic_timer));
            tx_timer_deactivate(&(ip_ptr->nx_ip_fast_periodic_timer));
            if(dhcp_ptr != NULL)
            {
                tx_mutex_get(&(dhcp_ptr -> nx_dhcp_mutex), TX_WAIT_FOREVER);
                tx_timer_deactivate(&(dhcp_ptr -> nx_dhcp_timer));
            }
            state = ST_WIFI_NET_RESUMING_FAILED;
            return state;
        }
#endif
        cylpa_s_ns_suspended = false;
        state = ST_SUCCESS;
    }

    return state;
}

int32_t cylpa_register_suspend_notify_callback( cylpa_suspend_callback_t callback, void *user_data )
{
    if( callback == NULL )
    {
        return ST_BAD_ARGS;
    }

    suspend_notify_cb = callback;
    cb_data = user_data;

    return CY_RSLT_SUCCESS;
}

int32_t cylpa_wait_net_inactivity(uint32_t inactive_interval_ms, uint32_t inactive_window_ms)
{
    cy_time_t lp_start_time;
    cy_time_t lp_end_time;
    uint32_t state = ST_SUCCESS;
#ifdef CYCFG_ULP_SUPPORT_ENABLED
    static bool is_ulp_configured = false;
#endif

#ifdef CYCFG_ULP_SUPPORT_ENABLED
    whd_interface_t ifp = NULL;

    ifp = cy_olm_get_whd_interface();
    if (ifp == NULL)
    {
       return ST_BAD_STATE;
    }
#endif

    if (inactive_interval_ms > inactive_window_ms)
    {
        /* Clear event flags to start with an initial state of no activity. */
        cy_rtos_clearbits_event(&cy_lp_wait_net_event, (uint32_t)(CY_LPA_TX_EVENT_FLAG | CY_LPA_RX_EVENT_FLAG), false);

         /* Start the wait timer. */
        cy_rtos_get_time( &lp_start_time);

         /* Wait until either our wait expires or the network is inactive for the period. */
        while(true)
        {
            uint32_t result;
            uint32_t elapsed_ms;
            uint32_t flags;

            /* Wait for the timeout value to expire.  This is the time we want
               to monitor for inactivity.
            */
            flags = (CY_LPA_TX_EVENT_FLAG | CY_LPA_RX_EVENT_FLAG);

            /* Configure ULP DS2 mode on specified interface only once */
#ifdef CYCFG_ULP_SUPPORT_ENABLED
            if(cy_wcm_is_connected_to_ap() == true && is_ulp_configured == false)
            {
                uint32_t set_ulp_mode = CY_ULP_MODE_SUPPORT;
                uint32_t ulp_wait_time = inactive_window_ms;

                if(inactive_window_ms < CY_ULP_WAIT_TIME)
                {
                    ulp_wait_time = CY_ULP_WAIT_TIME;
                }

                whd_wifi_config_ulp_mode(ifp, &set_ulp_mode, &ulp_wait_time);

                is_ulp_configured = true;
            }
#endif

            result = cy_rtos_waitbits_event(&cy_lp_wait_net_event, &flags, true, false, inactive_window_ms);
            if (CY_RTOS_TIMEOUT == result)
            {
                /* Inactivity wait condition met. */
                break;
            }

            cy_rtos_get_time(&lp_end_time);
            elapsed_ms = lp_end_time - lp_start_time;

            if (elapsed_ms >= inactive_interval_ms)
            {
                /* The network did not become inactive and we burned through our wait time. */
                state = ST_WAIT_INACTIVITY_TIMEOUT_EXPIRED;
                break;
            }
        }
    }
    else
    {
        state = ST_BAD_ARGS;
    }

    return state;
}

int32_t wait_net_suspend(void *net_intf, uint32_t wait_ms, uint32_t network_inactive_interval_ms,
        uint32_t network_inactive_window_ms)
{
    int32_t state;
    uint32_t result, flags;
    char idle_power_mode[IDLE_POWER_MODE_STRING_LEN];
    static bool emac_activity_callback_registered = false;
    cy_time_t lp_start_time;
    cy_time_t lp_end_time;

#if defined(COMPONENT_LWIP)
    struct netif *wifi = (struct netif *)cy_network_get_nw_interface( CY_NETWORK_WIFI_STA_INTERFACE, CY_NETWORK_WIFI_STA_INTERFACE );
#elif defined(COMPONENT_NETXDUO)
    NX_IP *wifi = (NX_IP *)cy_network_get_nw_interface( CY_NETWORK_WIFI_STA_INTERFACE, CY_NETWORK_WIFI_STA_INTERFACE);
#endif
    if((wifi == NULL) || (cy_wcm_is_connected_to_ap() == 0))
    {
        return ST_BAD_STATE;
    }

    if (false == emac_activity_callback_registered)
    {
        cylpa_network_act_handler_init();
        emac_activity_callback_registered = true;
    }

#if defined(COMPONENT_LWIP)
    if(is_rx_pkt_queue_created == false)
    {
        /* Create a queue to handle incoming packets from WHD when the network stack is not yet resumed */
        cy_rtos_queue_init(&rx_pkt_q_handle, RX_PKT_Q_LENGTH, sizeof(cylpa_rx_queue_t));
        is_rx_pkt_queue_created = true;
    }
#endif

    cyhal_syspm_lock_deepsleep();
    state = cylpa_wait_net_inactivity(network_inactive_interval_ms, network_inactive_window_ms);

    if (ST_SUCCESS == state)
    {
        /* Suspend network stack.
         * State data (e.g. caches) may be adjusted here so that the stack resumes properly.
         */
        state = cylpa_suspend_ns();
#if defined(COMPONENT_LWIP)
        /* Register the rx packet queue callback to queue the incoming packets
         * while the network stack is in suspended state. */
        cy_network_register_rx_queue_cb(cylpa_rx_activity);
#endif
        if (ST_SUCCESS == state)
        {
            /* Notify application about the network stack suspend */
            if(suspend_notify_cb)
            {
                suspend_notify_cb(CYLPA_NW_SUSPENDED, cb_data);
            }

            cy_rtos_clearbits_event(&cy_lp_wait_net_event, (uint32_t)(CY_LPA_TX_EVENT_FLAG | CY_LPA_RX_EVENT_FLAG), false);

            cylpa_get_idle_power_mode(idle_power_mode, sizeof(idle_power_mode));
            cylpa_olm_dispatch_pm_notification(cy_get_olm_instance(), OL_PM_ST_GOING_TO_SLEEP);
            NW_INFO(("\nNetwork Stack Suspended, MCU will enter %s power mode\n", idle_power_mode));
            cyhal_syspm_unlock_deepsleep();
            flags = (CY_LPA_RX_EVENT_FLAG | CY_LPA_TX_EVENT_FLAG);

            cy_rtos_get_time( &lp_start_time);

            /* Wait till there is emac activity. */
            result = cy_rtos_waitbits_event(&cy_lp_wait_net_event, &flags, true, false, wait_ms);
            if (CY_RTOS_TIMEOUT == result)
            {
                state = ST_WAIT_TIMEOUT_EXPIRED;
            }
            else
            {
                state = ST_NET_ACTIVITY;
            }

            cy_rtos_get_time( &lp_end_time);

            if(lp_end_time > lp_start_time)
            {
                lp_end_time = (lp_end_time-lp_start_time);
            }
            else
            {
                lp_end_time = (lp_start_time-lp_end_time);
            }

            cy_rtos_get_mutex(&cy_lp_mutex, wait_ms);

            cyhal_syspm_lock_deepsleep();
            /* Resume the network stack.
             * State data (e.g. caches) may be adjusted here so that the stack resumes properly.
            */
            NW_INFO(("Resuming Network Stack, Network stack was suspended for %ums\n",(unsigned int) lp_end_time));
            cy_dsleep_nw_suspend_time += lp_end_time;
            cylpa_print_whd_bus_stats((void*)wifi);

            /* Notify application about the network stack resuming */
            if(suspend_notify_cb)
            {
                suspend_notify_cb(CYLPA_NW_RESUMING, cb_data);
            }

            /* Update state to timeout expired even if event is set after timeout is expired as
             * xEventGroupWaitBits function returns the current bit for timeout scenario with
             * OOB disabled
             */
            if (lp_end_time > wait_ms) {
                state = ST_WAIT_TIMEOUT_EXPIRED;
            }
            cylpa_network_state_handler(state);
            /* Call OLM API to reset back the configurations */
            cylpa_olm_dispatch_pm_notification(cy_get_olm_instance(), OL_PM_ST_AWAKE);
#if defined(COMPONENT_LWIP)
            /* De-register the rx packet queue callback by passing NULL so that the packets from WHD
             * will be posted to lwip queue directly in cy_network_process_ethernet_data */
            cy_network_register_rx_queue_cb(NULL);
            cylpa_rx_queue_pkt_input();
#endif
            cylpa_resume_ns();
            cy_rtos_set_mutex(&cy_lp_mutex);
        }
    }

    cyhal_syspm_unlock_deepsleep();
    return state;
}

static void cylpa_network_state_handler(cy_rslt_t state)
{
    switch(state)
    {
        case ST_WAIT_TIMEOUT_EXPIRED:
            NW_INFO( ( "Host wait timeout expired, network did not become active.\r\n" ) );
            break;
        case ST_WAIT_INACTIVITY_TIMEOUT_EXPIRED:
            NW_INFO( ( "Host wait timeout expired, network did not become inactive.\r\n" ) );
            break;
        case ST_BAD_ARGS:
            NW_INFO( ( "Bad arguments passed for network suspension.\r\n" ) );
            break;
        case ST_BAD_STATE:
            NW_INFO( ( "Network stack state is corrupted.\r\n" ) );
            break;
        case ST_NET_ACTIVITY:
            NW_INFO( ( "Network is active. Resuming network stack\r\n" ) );
            break;
        default:
            break;
    }
}


/* This function can be used for doing TCP Keepalive verification
 *
 */
int cy_tcp_create_socket_connection( void *net_intf, void **global_socket_ptr, const char *remote_ip, uint16_t remote_port, uint16_t local_port,
                                          cy_tko_ol_cfg_t *downloaded, int socket_keepalive_enable)
{
    cy_rslt_t response = CY_RSLT_SUCCESS;
    cy_socket_sockaddr_t remote_addr = {0};
    cy_nw_ip_address_t nw_ip_addr;
#if defined(COMPONENT_LWIP)
    struct netif *net = NULL;
#elif defined(COMPONENT_NETXDUO)
    NX_IP *net = NULL;
    ULONG ipv4_addr;
    ULONG netmask;
    UINT  res;
#endif
    int32_t seconds, retry_count;
#if (LWIP_TCP == 1) || defined(NX_ENABLE_TCP_KEEPALIVE)
    uint32_t sock_handle;
    cy_socket_sockaddr_t sockaddr;
#endif

    if (net_intf == NULL )
    {
        OL_LOG_TKO(LOG_OLA_LVL_DEBUG, "Network is not up!\n");
        return CY_RSLT_TCPIP_ERROR;
    }

    int len = strlen(HARDCODED_STR);
    uint32_t bytes_sent = 0;

#if defined(COMPONENT_LWIP)
    net = (struct netif *)net_intf;
#elif defined(COMPONENT_NETXDUO)
    net = (NX_IP *)net_intf;
#endif

    cy_nw_str_to_ipv4((const char *)remote_ip, &nw_ip_addr);
    remote_addr.ip_address.version = CY_SOCKET_IP_VER_V4;
    remote_addr.ip_address.ip.v4 = nw_ip_addr.ip.v4;
    remote_addr.port = remote_port;
    sockaddr.ip_address.version = CY_SOCKET_IP_VER_V4;
    sockaddr.port = local_port;
#if defined(COMPONENT_LWIP)
    sockaddr.ip_address.ip.v4 = net->ip_addr.u_addr.ip4.addr;
#elif defined(COMPONENT_NETXDUO)
    res = nx_ip_address_get(net, &ipv4_addr, &netmask);
    if(res != NX_SUCCESS)
    {
        OL_LOG_TKO(LOG_OLA_LVL_DEBUG, "Failed to get the IP address!\n");
        return CY_RSLT_TCPIP_ERROR_BAD_ARG;
    }
    sockaddr.ip_address.ip.v4 = ipv4_addr;
#endif

    OL_LOG_TKO(LOG_OLA_LVL_DEBUG, "TCP remote IP Address %s  remote port:%d\n", remote_ip, remote_port );

    /* Create TCP Socket */
    response = cy_socket_create(CY_SOCKET_DOMAIN_AF_INET, CY_SOCKET_TYPE_STREAM, CY_SOCKET_IPPROTO_TCP, (cy_socket_t *)&sock_handle);
    if (response != CY_RSLT_SUCCESS)
    {
        OL_LOG_TKO(LOG_OLA_LVL_ERR, "TCP socket create failed ret:%ld\n", response);
        return response;
    }

    /* Bind socket to local port */
    response = cy_socket_bind((void * )sock_handle, &sockaddr, (uint32_t)sizeof(cy_socket_sockaddr_t));
    if ( CY_RSLT_SUCCESS != response)
    {
        cy_socket_delete((void *)sock_handle);
        OL_LOG_TKO(LOG_OLA_LVL_ERR, "TCP socket bind failed response: %ld\n", response);
        return response;
    }

    /* Establish TCP Connection */
    response = cy_socket_connect((void *)sock_handle, &remote_addr, (uint32_t)sizeof(cy_socket_sockaddr_t));

    if ( response != CY_RSLT_SUCCESS )
    {
        OL_LOG_TKO(LOG_OLA_LVL_ERR, "TCP connect failed to IP %s local port:%d remote port:%d err=%ld\n",
                   remote_ip, local_port, remote_port, response);
        cy_socket_delete((void *)sock_handle);
        return response;
    }

    OL_LOG_TKO(LOG_OLA_LVL_DEBUG, "Created TCP connection to IP %s, local port %d, remote port %d\n",
               remote_ip, local_port, remote_port);

#if (LWIP_TCP == 1) || defined(NX_ENABLE_TCP_KEEPALIVE)
    if (socket_keepalive_enable)
    {
        response = cy_socket_setsockopt((void *)sock_handle, CY_SOCKET_SOL_SOCKET,
                                         CY_SOCKET_SO_TCP_KEEPALIVE_ENABLE, &socket_keepalive_enable,
                                         sizeof(socket_keepalive_enable));

        if ( response != CY_RSLT_SUCCESS)
        {
            OL_LOG_TKO(LOG_OLA_LVL_ERR, "Host TCP keepalive enable failed %ld\n", response );
        }

        seconds = downloaded->retry_interval * 1000;
        response = cy_socket_setsockopt((void *)sock_handle, CY_SOCKET_SOL_TCP,
                                        CY_SOCKET_SO_TCP_KEEPALIVE_INTERVAL,
                                        &seconds, sizeof(seconds));

        if ( response != CY_RSLT_SUCCESS )
        {
            OL_LOG_TKO(LOG_OLA_LVL_ERR, "TCP keepalive interval time configuration failed :%ld\n", response );
        }

        retry_count = downloaded->retry_count;
        response = cy_socket_setsockopt((void *)sock_handle, CY_SOCKET_SOL_TCP,
                                        CY_SOCKET_SO_TCP_KEEPALIVE_COUNT,
                                        &retry_count, sizeof(retry_count));

        if ( response != CY_RSLT_SUCCESS )
        {
            OL_LOG_TKO(LOG_OLA_LVL_ERR, "TCP keepalive retry count configuration failed :%ld\n", response );
        }

        seconds = downloaded->interval * 1000;
        response = cy_socket_setsockopt((void *)sock_handle, CY_SOCKET_SOL_TCP,
                                        CY_SOCKET_SO_TCP_KEEPALIVE_IDLE_TIME,
                                        &seconds, sizeof(seconds ));

        if ( response != CY_RSLT_SUCCESS )
        {
            OL_LOG_TKO(LOG_OLA_LVL_ERR, "TCP keepalive Idle time configuration failed :%ld\n", response );
        }
        OL_LOG_TKO(LOG_OLA_LVL_DEBUG, "setsockopt LWIP keepalive: Interval %d, Retry Interval %d, keepalive value %d\n",
                   downloaded->interval, downloaded->retry_interval, downloaded->interval);
    }
#endif

    memcpy(cy_tcp_databuf, HARDCODED_STR, len);
    response = cy_socket_send((void *)sock_handle, cy_tcp_databuf, len, CY_SOCKET_FLAGS_NONE, &bytes_sent);
    if (response != CY_RSLT_SUCCESS)
    {
        OL_LOG_TKO(LOG_OLA_LVL_ERR, "Could only send %u bytes on socket of request length err: %ld\n", (unsigned int)bytes_sent, response );
    }

    *global_socket_ptr = (void *)sock_handle;
    cylpa_tko_ol_update_config(remote_ip, remote_port, local_port, downloaded);
    return response;
}

/**
 * Restart olm with a new configuration
 */
int cylpa_restart_olm( ol_desc_t *offload_list , void *net_intf )
{
    olm_t *olm_ptr = (olm_t *)cy_get_olm_instance();
    const ol_desc_t *oflds_list = (const ol_desc_t *)offload_list;
    cylpa_olm_deinit(olm_ptr);
    cylpa_olm_init(olm_ptr, oflds_list);
    return CY_RSLT_SUCCESS;
}

/*-----------------------------------------------------------*/
/*
 * Find the descriptor for the given filter.
 */
ol_desc_t *cylpa_find_my_descriptor(const char *name, ol_desc_t *offload_list )
{
    ol_desc_t *oflds_list = offload_list;

    if (oflds_list == NULL)
    {
        return NULL;
    }

    /* Loop through the offload list to find by name */
    while (oflds_list && oflds_list->name && strncmp(oflds_list->name, name, strlen(name))) {
        oflds_list++;
    }
    if (!oflds_list || !oflds_list->name) {
        return NULL;
    }
    return oflds_list;
}
