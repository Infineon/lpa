/*******************************************************************************
* File Name: network_activity_handler.c
*
* Version: 1.0
*
* Description: This file implements the functions needed to suspend
* network stack and wait till external event or timeout for AnyCloud SDK
*
********************************************************************************
* Copyright 2020 Cypress Semiconductor Corporation
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
********************************************************************************/

#include "network_activity_handler.h"
#include "cyabs_rtos.h"
#include "cy_lwip.h"
#include "cy_OlmInterface.h"
#include "whd_int.h"
#include <cycfg_system.h>
#include "cyhal.h"

/* lwIP header files */
#include <lwip/tcpip.h>
#include <lwip/api.h>
#include <lwip/tcp.h>
#include <lwip/priv/tcp_priv.h>
#include "cy_nw_helper.h"

/* secure socket Header files */
#include "cy_secure_sockets.h"
#include "cy_tls.h"

/******************************************************
 *                      Macros
 ******************************************************/

#define IDLE_POWER_MODE_STRING_LEN (32)

#define sleep_manager_lock_deep_sleep() \
        nw_act_handler_deepsleep_lock = true

#define sleep_manager_unlock_deep_sleep() \
        nw_act_handler_deepsleep_lock = false

#define sleep_manager_can_deep_sleep() \
    (nw_act_handler_deepsleep_lock == true ? true:false)

#define PACKET_PAYLOAD 1500
uint8_t tcp_databuf[PACKET_PAYLOAD];
#define HARDCODED_STR   "Some random stuff"

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/

/*******************************************************************************
* Function Name: get_idle_power_mode
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
static void get_idle_power_mode(char *str, uint32_t length);

/*******************************************************************************
* Function Name: print_whd_bus_stats
********************************************************************************
*
* Summary: This function is used to print the WHD bus stats, one of the useful
* the information includes no of WLAN Out-of-Band interrupts and In-band interrupts.
*
* Parameters:
* netif *wifi: pointer to WLAN interface
*
* Return:
* void.
*
*******************************************************************************/
static void print_whd_bus_stats(struct netif *wifi);


/*******************************************************************************
* Function Name: cy_register_network_activity_callback
********************************************************************************
*
* Summary: This function is used to register a callback for the event of net
* activity.
*
* Return:
* cy_rslt_t: contains status of attaching callback.
*
*******************************************************************************/
static cy_rslt_t cy_register_network_activity_callback(void);

/*******************************************************************************
* Function Name: network_state_handler
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
static void network_state_handler(cy_rslt_t state);

/*******************************************************************************
* Function Name: cy_network_act_handler_init
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
static void cy_network_act_handler_init(void);

/******************************************************
 *               Variable Definitions
 ******************************************************/

/*  This variable is used as a check to prevent successive locking/unlocking
    of the TCP/IP stack. Locking the TCP/IP stack prevents the MCU from servicing
    the network timers. This enables the MCU to stay in deep sleep longer.
*/

static bool s_ns_suspended;

/*  This event variable is used to alert the device if there is any network
    activity.
*/
cy_event_t lp_wait_net_event;

/*  This is mutex to sychronize multiple threads */
cy_mutex_t lp_mutex;

/* This variable is used to track total time spent in deep sleep */
uint32_t dsleep_nw_suspend_time;

/* This variable blocks deep sleep for freertos */
bool nw_act_handler_deepsleep_lock = false;

/* callback for SysPM to block deep sleep */
cy_stc_syspm_callback_t deepsleepPMCallback;

/* LP Timer object */
cyhal_lptimer_t lptickerObj;

/******************************************************
 *               Function Definitions
 ******************************************************/
cy_en_syspm_status_t cy_lpa_nw_activity_handler_pm_callback(cy_stc_syspm_callback_params_t *callbackParams, cy_en_syspm_callback_mode_t mode)
{
    cy_en_syspm_status_t retStatus = CY_SYSPM_FAIL;

    switch(mode)
    {
        case CY_SYSPM_CHECK_READY:
        {
            if(!nw_act_handler_deepsleep_lock)
            {
                retStatus = CY_SYSPM_SUCCESS;
            }
        }
        break;

        case CY_SYSPM_CHECK_FAIL:
        {
            retStatus = CY_SYSPM_SUCCESS;
        }
        break;

        case CY_SYSPM_BEFORE_TRANSITION:
        break;

        case CY_SYSPM_AFTER_TRANSITION:
        {
        }
        break;

        default:
            break;
    }

    return (retStatus);
}

void cy_network_act_handler_init(void)
{
    //register pm callback     
    deepsleepPMCallback.callback = &cy_lpa_nw_activity_handler_pm_callback;
    deepsleepPMCallback.callbackParams = NULL;
    deepsleepPMCallback.type = CY_SYSPM_DEEPSLEEP;
    deepsleepPMCallback.order = 0; 

    /* Register for SysPM and handle deep sleep based on nw_act_handler_deepsleep_lock variable*/
    Cy_SysPm_RegisterCallback(&deepsleepPMCallback);

    /* Register for network activity callback */
    cy_register_network_activity_callback();
}

static void get_idle_power_mode(char *str, uint32_t length)
{
    switch (CY_CFG_PWR_SYS_IDLE_MODE)
    {
        case CY_CFG_PWR_MODE_LP:
            strncpy(str, "LP", length);
            break;
        case CY_CFG_PWR_MODE_ULP:
            strncpy(str, "ULP", length);
            break;
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

static void print_whd_bus_stats(struct netif *wifi)
{
    whd_interface_t ifp = NULL;
    if (wifi->flags & NETIF_FLAG_UP)
    {
        ifp = (whd_interface_t)wifi->state;
        NW_INFO(("\n=====================================================\n"));
        (void)whd_print_stats(ifp->whd_driver, WHD_FALSE);
        NW_INFO(("=====================================================\n"));
    }
    else
    {
        NW_INFO(("Wi-Fi interface is not powered on, bus_stats are unavailable\n"));
    }
}

void on_emac_activity(bool is_tx_activity)
{
    cy_rtos_setbits_event(&lp_wait_net_event,
        (uint32_t)(is_tx_activity ? TX_EVENT_FLAG : RX_EVENT_FLAG), true);
}

static cy_rslt_t cy_register_network_activity_callback(void)
{
    cy_rslt_t result = cy_rtos_init_event(&lp_wait_net_event);

    if (CY_RSLT_SUCCESS != result)
    {
        NW_INFO(("Failed to initialize Wait for Network Activity event.\n"));
        return result;
    }

    result  = cy_rtos_init_mutex(&lp_mutex);

    if ( CY_RSLT_SUCCESS != result )
    {
    	NW_INFO(("Failed to initialize Mutex \n"));
    	return result;
    }
    else
    {
        /* Set EMAC activity callback for this module. */
        cy_network_activity_register_cb(on_emac_activity);
        result = CY_RSLT_SUCCESS;
    }

    return result;
}

int32_t suspend_ns(void)
{
    int32_t state;

    if (true == s_ns_suspended)
    {
        state = ST_BAD_STATE;
    }
    else
    {
        LOCK_TCPIP_CORE();
        s_ns_suspended = true;
        state = ST_SUCCESS;
    }

    return state;
}

int32_t resume_ns(void)
{
    int32_t state;

    if (false == s_ns_suspended)
    {
        state = ST_BAD_STATE;
    }
    else
    {
        UNLOCK_TCPIP_CORE();
        s_ns_suspended = false;
        state = ST_SUCCESS;
    }

    return state;
}

int32_t wait_net_inactivity(uint32_t inactive_interval_ms, uint32_t inactive_window_ms)
{
    cy_time_t lp_start_time;
    cy_time_t lp_end_time;
    uint32_t state = ST_SUCCESS;

    if (inactive_interval_ms > inactive_window_ms)
    {
        /* Clear event flags to start with an initial state of no activity. */
        cy_rtos_clearbits_event(&lp_wait_net_event, (uint32_t)(TX_EVENT_FLAG | RX_EVENT_FLAG), false);

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
            flags = (TX_EVENT_FLAG | RX_EVENT_FLAG);
            result = cy_rtos_waitbits_event(&lp_wait_net_event, &flags, true, false, inactive_window_ms);
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
    static uint8_t statsCount = 0;
    int32_t state;
    uint32_t result, flags;
    uint32_t start, end;
    char idle_power_mode[IDLE_POWER_MODE_STRING_LEN];
    static bool emac_activity_callback_registered = false;
    static bool lp_tmr_initialized = false;
    struct netif *wifi = (struct netif *)net_intf;
    if(!lp_tmr_initialized)
    {
        cy_rslt_t result = cyhal_lptimer_init(&lptickerObj);
        lp_tmr_initialized = (result == CY_RSLT_SUCCESS);
    }

    if (false == emac_activity_callback_registered)
    {
        cy_network_act_handler_init();
        emac_activity_callback_registered = true;
    }

    sleep_manager_lock_deep_sleep();
    state = wait_net_inactivity(network_inactive_interval_ms, network_inactive_window_ms);

    if (ST_SUCCESS == state)
    {
        /* Suspend network stack.
         * State data (e.g. caches) may be adjusted here so that the stack resumes properly.
         */
        state = suspend_ns();
        if (ST_SUCCESS == state)
        {
            cy_rtos_clearbits_event(&lp_wait_net_event, (uint32_t)(TX_EVENT_FLAG | RX_EVENT_FLAG), false);

            get_idle_power_mode(idle_power_mode, sizeof(idle_power_mode));
            olm_dispatch_pm_notification(cy_get_olm_instance(), OL_PM_ST_GOING_TO_SLEEP);
            NW_INFO(("\nNetwork Stack Suspended, MCU will enter %s power mode\n", idle_power_mode));
            sleep_manager_unlock_deep_sleep();
            flags = (RX_EVENT_FLAG | TX_EVENT_FLAG);

            start = cyhal_lptimer_read(&lptickerObj);

            /* Wait till there is emac activity. */
            result = cy_rtos_waitbits_event(&lp_wait_net_event, &flags, true, false, wait_ms);
            if (CY_RTOS_TIMEOUT == result)
            {
                state = ST_WAIT_TIMEOUT_EXPIRED;
            }
            else
            {
                state = ST_NET_ACTIVITY;
            }

            end = cyhal_lptimer_read(&lptickerObj);

            if(end > start)
            {
                end = (end-start)/32;
            }
            else
            {
                end = (start-end)/32;
            }

            cy_rtos_get_mutex(&lp_mutex, wait_ms);

            sleep_manager_lock_deep_sleep();
            /* Resume the network stack.
             * State data (e.g. caches) may be adjusted here so that the stack resumes properly.
            */
            NW_INFO(("Resuming Network Stack, Network stack was suspended for %lums\n",end));
            dsleep_nw_suspend_time += end-start;
            olm_dispatch_pm_notification(cy_get_olm_instance(), OL_PM_ST_AWAKE);
            if(statsCount == 0)
            {
                print_whd_bus_stats(wifi);
            }
            else
            {
                statsCount++;
                if(statsCount == 5) statsCount = 0;
            }
            /* Update state to timeout expired even if event is set after timeout is expired as
             * xEventGroupWaitBits function returns the current bit for timeout scenario with
             * OOB disabled
             */
            if (end > wait_ms) {
                state = ST_WAIT_TIMEOUT_EXPIRED;
            }
            network_state_handler(state);
            cy_rtos_set_mutex(&lp_mutex);
            resume_ns();
        }
    }

    sleep_manager_unlock_deep_sleep();
    return state;
}

static void network_state_handler(cy_rslt_t state)
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
/* TCP user data buffer to send to server */
static uint8_t tcp_user_databuf[PACKET_PAYLOAD];
int cy_tcp_create_socket_connection( void *net_intf, void **global_socket_ptr, const char *remote_ip, uint16_t remote_port, uint16_t local_port,
                                          cy_tko_ol_cfg_t *downloaded, int socket_keepalive_enable)
{
    cy_rslt_t response = CY_RSLT_SUCCESS;
    cy_socket_sockaddr_t remote_addr = {0};
    cy_nw_ip_address_t nw_ip_addr;
    struct netif *net = NULL;
    int32_t seconds;
#if LWIP_TCP
    extern struct tcp_pcb *tcp_active_pcbs;
    uint32_t sock_handle;
    cy_socket_sockaddr_t sockaddr;
#endif

    if (net_intf == NULL )
    {
        printf("Network is not up!\n");
        return CY_RSLT_TCPIP_ERROR;
    }

    int len = strlen(HARDCODED_STR);
    uint32_t bytes_sent = 0;

    net = (struct netif *)net_intf;

    cy_nw_str_to_ipv4((const char *)remote_ip, &nw_ip_addr);
    remote_addr.ip_address.version = CY_SOCKET_IP_VER_V4;
    remote_addr.ip_address.ip.v4 = nw_ip_addr.ip.v4;
    remote_addr.port = remote_port;
    sockaddr.ip_address.version = CY_SOCKET_IP_VER_V4;
    sockaddr.ip_address.ip.v4 = net->ip_addr.u_addr.ip4.addr;
    sockaddr.port = local_port;

    printf("TCP remote IP Address %lx  remote port:%d\n", remote_addr.ip_address.ip.v4, remote_port );

    /* Create TCP Socket */
    response = cy_socket_create(CY_SOCKET_DOMAIN_AF_INET, CY_SOCKET_TYPE_STREAM, CY_SOCKET_IPPROTO_TCP, (cy_socket_t *)&sock_handle);
    if (response != CY_RSLT_SUCCESS) {
          printf("TCP socket create failed ret:%ld\n", response);
          return response;
    }

    /* Bind socket to local port */
    response = cy_socket_bind((void * )sock_handle, &sockaddr, (uint32_t)sizeof(cy_socket_sockaddr_t));
    if ( CY_RSLT_SUCCESS != response) {
          printf("socket.bind() failed: %ld\n", response);
          return response;
    }

    /* Establish TCP Connection */
    response = cy_socket_connect((void *)sock_handle, &remote_addr, (uint32_t)sizeof(cy_socket_sockaddr_t));

    if ( response != CY_RSLT_SUCCESS ) {
        printf("TCP connect failed on remote port:%d err=%ld\n", remote_port, response);
        return response;
    }
    if ( response == CY_RSLT_SUCCESS)
    {
        printf("TCP Connection Established Successfully ctx:%lx\n", sock_handle);
    }

#if LWIP_TCP
    if (socket_keepalive_enable)
    {
        /* Enable Host TCP Keepalive */
        ip_set_option(tcp_active_pcbs, SOF_KEEPALIVE);

        /* keep_idle, After this much idle time, send Packet */
        seconds = downloaded->interval * 1000;
        tcp_active_pcbs->keep_idle = seconds ? seconds : TCP_KEEPIDLE_DEFAULT;

        /* If don't get ack retry keep_intvl seconds for keep_cnt times */
        seconds = downloaded->retry_interval * 1000;
        tcp_active_pcbs->keep_cnt = seconds ? seconds : TCP_KEEPCNT_DEFAULT;

        seconds = downloaded->interval * 1000;
        tcp_active_pcbs->keep_intvl = seconds ? seconds : TCP_KEEPINTVL_DEFAULT;

        printf("Set LWIP keepalive: Interval %d, Retry Interval %d, keepalive value %d\n",
                  downloaded->interval, downloaded->retry_interval, downloaded->interval);
    }
#endif

    memcpy(tcp_user_databuf, HARDCODED_STR, len);
    response = cy_socket_send((void *)sock_handle, tcp_user_databuf, len, NETCONN_NOFLAG, &bytes_sent);
    if (response != CY_RSLT_SUCCESS) {
            printf("Could only send %ld bytes on socket err: %ld\n", bytes_sent, response );
    }

    *global_socket_ptr = (void *)sock_handle;
    return response;
}
