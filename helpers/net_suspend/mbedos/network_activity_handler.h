/*******************************************************************************
* File Name: network_activity_handler.h
*
* Version: 1.0
*
* Description: This file contains the macros, enumerations and function
* prototypes used by network_activity_handler.cpp.
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

#define NW_INFO( x )      printf x

#define RX_EVENT_FLAG     ( 1UL << 0 )
#define TX_EVENT_FLAG     ( 1UL << 1 )

/* This enumeration enlists the different states of the network stack */
enum network_stack_state_t
{
    ST_SUCCESS,
    ST_WAIT_TIMEOUT_EXPIRED,
    ST_WAIT_INACTIVITY_TIMEOUT_EXPIRED,
    ST_WAIT_ACTIVITY_TIMEOUT_EXPIRED,
    ST_BAD_ARGS,
    ST_BAD_STATE,
    ST_CONNECT_FAILED,
    ST_DISCONNECT_FAILED,
    ST_WIFI_NET_SUSPENDED_FAILED,
    ST_WIFI_NET_RESUMING_FAILED,
    ST_NET_ACTIVITY
};

/* Forward Declarations */
class WhdSTAInterface;

/*******************************************************************************
* Function Name: wait_net_suspend
********************************************************************************
*
* Summary:
* In this function the network is monitored for inactivity in an interval of
* length inactive_interval_ms. If the network is inactive for a continuous
* duration specified by inactive_window_ms, the TCP/IP network stack is
* suspended and stays suspended until either there is emac activity or a
* duration of activity_interval_ms has expired. Network stack is resumed on
* either detecting emac activity or if activity_interval_ms time has elapsed.
*
* Parameters:
* WhdSTAInterface *wifi: pointer to WLAN interface
* uint32_t wait_ms: The interval for which the network is monitored
* for activity. Network stack is resumed ether at the end of
* activity_interval_ms or on emac activity.
* uint32_t inactive_interval_ms: The interval for which the network is monitored
* for inactivity.
* uint32_t inactive_window_ms: The continuous duration for which network has to
* be inactive in inactive_interval_ms.
*
* Return:
* int32_t: contains status of network stack suspension and corresponding resumption
*
*******************************************************************************/
int32_t wait_net_suspend(WhdSTAInterface *wifi, uint32_t wait_ms,
        uint32_t inactive_interval_ms, uint32_t inactive_window_ms);
