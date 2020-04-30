/*******************************************************************************
* File Name: network_activity_handler.cpp
*
* Version: 1.0
*
* Description: This file implements the functions needed to suspend
* network stack and wait till external event or timeout for MBED OS
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

#include "mbed.h"
#include "WhdSTAInterface.h"
#include "tcpip.h"
#include "network_activity_handler.h"

/******************************************************
 *                      Macros
 ******************************************************/

#define IDLE_POWER_MODE_STRING_LEN (32)
#define PACKET_PAYLOAD             (1500)
#define HARDCODED_STR   "Some random stuff"

/******************************************************
 *                   Global Declarations
 ******************************************************/
uint8_t tcp_databuf[PACKET_PAYLOAD];

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
* WhdSTAInterface *wifi: pointer to WLAN interface whose emac activity is being
* monitored.
*
* Return:
* void.
*
*******************************************************************************/
static void print_whd_bus_stats(WhdSTAInterface *wifi);


/*******************************************************************************
* Function Name: register_emac_activity_callback
********************************************************************************
*
* Summary: This function is used to register a callback for the event of emac
* activity.
*
* Parameters:
* WhdSTAInterface *wifi: pointer to WLAN interface whose emac activity is being
* monitored.
*
* Return:
* cy_rslt_t: contains status of attaching callback.
*
*******************************************************************************/

static cy_rslt_t register_emac_activity_callback(WhdSTAInterface *wifi);

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
cy_mutex_t lp_mutex;

/* This variable is used to track total time spent in deep sleep */
us_timestamp_t dsleep_nw_suspend_time;

/******************************************************
 *               Function Definitions
 ******************************************************/

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
static void print_whd_bus_stats(WhdSTAInterface *wifi)
{
    WHD_EMAC& whd_emac = static_cast<WHD_EMAC&>(wifi->get_emac());
    if (whd_emac.powered_up)
    {
        NW_INFO(("\n=====================================================\n"));
        (void)whd_print_stats(whd_emac.drvp, WHD_FALSE);
        NW_INFO(("=====================================================\n"));
    }
    else
    {
        NW_INFO(("EMAC interface is not powered on, bus_stats are unavailable\n"));
    }
}

static cy_rslt_t register_emac_activity_callback(WhdSTAInterface *wifi)
{
    cy_rslt_t result = cy_rtos_init_event(&lp_wait_net_event);

    if (CY_RSLT_SUCCESS != result)
    {
        NW_INFO(("Failed to initialize Wait for Network Activity event.\r\n"));
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
        static_cast<WHD_EMAC&>( wifi->get_emac()).set_activity_cb(mbed::callback(on_emac_activity));
        result = CY_RSLT_SUCCESS;
    }

    return result;
}

void on_emac_activity(bool is_rx_activity)
{
    cy_rtos_setbits_event(&lp_wait_net_event,
        (uint32_t)(is_rx_activity ? RX_EVENT_FLAG : TX_EVENT_FLAG), true);
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

int32_t wait_net_suspend(void *net_intf, uint32_t wait_ms,
        uint32_t network_inactive_interval_ms,
        uint32_t network_inactive_window_ms)

{
    int32_t state;
    uint32_t result, flags;
    us_timestamp_t start, end;
    char idle_power_mode[IDLE_POWER_MODE_STRING_LEN];
    static bool emac_activity_callback_registered = false;
    WhdSTAInterface *wifi = (WhdSTAInterface *) net_intf;

    if (false == emac_activity_callback_registered)
    {
        register_emac_activity_callback(static_cast<WhdSTAInterface*>(wifi));
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
            get_idle_power_mode(idle_power_mode, sizeof(idle_power_mode));
            wifi->net_suspended();
            NW_INFO(("\nNetwork Stack Suspended, MCU will enter %s power mode\n", idle_power_mode));
            sleep_manager_unlock_deep_sleep();
            flags = (RX_EVENT_FLAG | TX_EVENT_FLAG);

            start = mbed_uptime();
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

            end = mbed_uptime();
            cy_rtos_get_mutex(&lp_mutex, wait_ms);
            sleep_manager_lock_deep_sleep();
            wifi->net_resuming();
            /* Resume the network stack.
             * State data (e.g. caches) may be adjusted here so that the stack resumes properly.
            */
            NW_INFO(("Resuming Network Stack, Network stack was suspended for %llums\n", (end-start)/1000));
            dsleep_nw_suspend_time += end-start;
            print_whd_bus_stats(wifi);
            resume_ns();
            network_state_handler(state);
            cy_rtos_set_mutex(&lp_mutex);
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

int cy_tcp_create_socket_connection ( void *netif_if, void **global_socket, const char *remote_ip, uint16_t remote_port, uint16_t local_port,
		                              cy_tko_ol_cfg_t *tko_ol_cfg, int socket_keepalive_enable )
{
    int response;
    int data_length = 0;
    nsapi_error_t ns_ret;
    int32_t cfg_interval;
    int32_t retry_interval;
    int32_t enable = 1;
    TCPSocket *socket_ptr = NULL;
    SocketAddress src_sockaddress;
    SocketAddress remote_sockaddress;
    WhdSTAInterface *wifi_intf = (WhdSTAInterface *)netif_if;
    NetworkInterface *networkInterface;

    networkInterface = (NetworkInterface *)wifi_intf;

    if ( wifi_intf == NULL )
    {
    	printf("Network Interface is not up !\n");
    	return NSAPI_ERROR_NO_CONNECTION;
    }

    *global_socket = new TCPSocket();
    socket_ptr = (TCPSocket *)*global_socket;

    if ( socket_ptr == NULL )
    {
    	printf("TCP socket create failed\n");
    	return NSAPI_ERROR_NO_SOCKET;
    }

    /* Create TCP Socket */
    response = socket_ptr->open((NetworkInterface *)wifi_intf);
    if (response != NSAPI_ERROR_OK) {
    	delete socket_ptr;
    	*global_socket = NULL;
    	return response;
    }

    networkInterface->get_ip_address(&src_sockaddress);
    src_sockaddress.set_port(local_port);

    /* Bind socket to local port */
    response = socket_ptr->bind(src_sockaddress);
    if (response != NSAPI_ERROR_OK) {
            printf("socket.bind() failed: %d\n", response);
            socket_ptr->close();
            delete socket_ptr;
            *global_socket = NULL;
            return response;
    }

    remote_sockaddress.set_ip_address((const char*)remote_ip);
    remote_sockaddress.set_port(remote_port);

    /* Establish TCP Connection */
    socket_ptr->set_timeout(-1);
    response = socket_ptr->connect(remote_sockaddress);
    if (response != NSAPI_ERROR_OK) {
    	socket_ptr->close();
    	delete socket_ptr;
    	*global_socket = NULL;
        return response;
    }

    /* Use socket level tko, while host is awake */
    cfg_interval = tko_ol_cfg->interval * 1000;    /* keep_idle, After this much idle time, send Packet */
    retry_interval = tko_ol_cfg->retry_interval * 1000;    /* keep_intvl, if don't get ack retry in this many seconds */

    if ( socket_keepalive_enable)
    {
        ns_ret = socket_ptr->setsockopt(NSAPI_SOCKET, NSAPI_KEEPIDLE, &cfg_interval, sizeof(cfg_interval));
        if (ns_ret != NSAPI_ERROR_OK) {
          printf("KEEPIDLE failed %d, abandon\n", ns_ret);
          socket_ptr->close();
          delete socket_ptr;
          *global_socket = NULL;
          return ns_ret;
        }

        ns_ret = socket_ptr->setsockopt(NSAPI_SOCKET, NSAPI_KEEPINTVL, &retry_interval, sizeof(retry_interval));
        if (ns_ret != NSAPI_ERROR_OK) {
          printf("KEEPINTVL failed %d, abandon\n", ns_ret);
          socket_ptr->close();
          delete socket_ptr;
          *global_socket = NULL;
          return ns_ret;
        }

        ns_ret = socket_ptr->setsockopt(NSAPI_SOCKET, NSAPI_KEEPALIVE, &enable, sizeof(enable));
        if (ns_ret != NSAPI_ERROR_OK) {
          printf("KEEPALIVE failed %d, abandon\n", ns_ret);
          socket_ptr->close();
          delete socket_ptr;
          *global_socket = NULL;
          return ns_ret;
        }
    }
    printf("Set LWIP keepalive: Interval %d, Retry Interval %d, keepalive enable/disable %d\n",
    		cfg_interval, retry_interval, enable);

    if (1) {
        int len = strlen(HARDCODED_STR);
        memcpy(tcp_databuf, HARDCODED_STR, len);
        data_length = socket_ptr->send(tcp_databuf, len);
        if (len != data_length) {
            printf("Could only send %d of %d bytes on socket: %s\n", data_length, len, tcp_databuf);
        }
    }

    if (0) {
        memset(tcp_databuf, 0, sizeof(tcp_databuf));
        data_length = socket_ptr->recv(tcp_databuf, PACKET_PAYLOAD );
        printf("Got %d bytes form socket: %s\n", data_length, tcp_databuf);
    }
    return response;
}
