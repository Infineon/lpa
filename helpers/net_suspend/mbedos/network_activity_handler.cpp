/*******************************************************************************
* File Name: network_activity_handler.cpp
*
* Version: 1.0
*
* Description: This file implements the functinos needed to suspend
* network stack and wait till external event or timeout.
*
********************************************************************************
* Copyright 2019 Cypress Semiconductor Corporation
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
* Function Name: on_emac_activity
********************************************************************************
*
* Summary: This is the callback that is called on detecting emac activity. It
* sets the bits of a flag indicating TX event or RX event.
*
*
* Parameters:
* bool is_tx_activity: used to determine which bit of flag is to be set. If true,
* TX bit in the flag is set. Else, RX bit is set.
*
*******************************************************************************/
static void on_emac_activity(bool is_rx_activity);

/*******************************************************************************
* Function Name: suspend_ns
********************************************************************************
*
* Summary:
* This function suspends the network stack by taking a lock on the TCP/IP stack.
*
* Return: int32_t: contains status of suspending the network stack.
*
*******************************************************************************/
static int32_t suspend_ns(void);

/*******************************************************************************
* Function Name: resume_ns
********************************************************************************
*
* Summary:
* This function suspends the network stack by unlocking the TCP/IP stack.
*
* Return: int32_t: contains status of resuming the network stack.
*
*******************************************************************************/
static int32_t resume_ns(void);

/*******************************************************************************
* Function Name: wait_net_inactivity
********************************************************************************
*
* Summary:
* In this function the network is monitored for inactivity in an interval of
* length inactive_interval_ms. If the network is inactive for a continuous
* duration specified by inactive_window_ms, the network is declared as inactive
* and the corresponding network state value is returned.
*
* Parameters:
* uint32_t inactive_interval_ms: The interval for which the network is monitored
* for inactivity.
* uint32_t inactive_window_ms: The continuous duration for which network has to
* be inactive in inactive_interval_ms.
*
* Return:
* int32_t: contains status of network inactivity.
*
*******************************************************************************/
static int32_t wait_net_inactivity(uint32_t inactive_interval_ms, uint32_t inactive_window_ms);


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
    }
    else
    {
        /* Set EMAC activity callback for this module. */
        static_cast<WHD_EMAC&>( wifi->get_emac()).set_activity_cb(mbed::callback(on_emac_activity));
        result = CY_RSLT_SUCCESS;
    }

    return result;
}

static void on_emac_activity(bool is_rx_activity)
{
    cy_rtos_setbits_event(&lp_wait_net_event,
        (uint32_t)(is_rx_activity ? RX_EVENT_FLAG : TX_EVENT_FLAG), true);
}

static int32_t suspend_ns(void)
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

static int32_t resume_ns(void)
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

static int32_t wait_net_inactivity(uint32_t inactive_interval_ms, uint32_t inactive_window_ms)
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


int32_t wait_net_suspend(WhdSTAInterface *wifi, uint32_t wait_ms,
        uint32_t network_inactive_interval_ms,
        uint32_t network_inactive_window_ms)

{
    int32_t state;
    uint32_t result, flags;
    us_timestamp_t start, end;
    char idle_power_mode[IDLE_POWER_MODE_STRING_LEN];
    static bool emac_activity_callback_registered = false;

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
            sleep_manager_lock_deep_sleep();
            /* Resume the network stack.
             * State data (e.g. caches) may be adjusted here so that the stack resumes properly.
            */
            NW_INFO(("Resuming Network Stack, Network stack was suspended for %llums\n", (end-start)/1000));
            dsleep_nw_suspend_time += end-start;
            print_whd_bus_stats(wifi);
            state = resume_ns();
            network_state_handler(state);
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
