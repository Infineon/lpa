/*
 * Copyright 2024, Cypress Semiconductor Corporation (an Infineon company) or
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
* @file network_activity_handler.h
* @brief This file contains the macros, enumerations and function
* prototypes used by network_activity_handler.
*/

#ifdef __cplusplus
extern "C" {
#endif

#include "stdbool.h"
#include "cy_lpa_wifi_ol.h"
#include "cy_lpa_wifi_tko_ol.h"
#include "cy_result_mw.h"

#define NW_INFO( x )      printf x

#define RX_EVENT_FLAG     ( 1UL << 0 )
#define TX_EVENT_FLAG     ( 1UL << 1 )

#define PKT_FILTER_NAME "Pkt_Filter"
#define ARP_NAME        "ARP"
#define TKO_NAME        "TKO"
#define TLSOE_NAME      "TLSOE"
#define WOWLPF_NAME      "WOWLPF"
#define NULLKO_NAME      "NULLKO"

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

/* LPA event notification types */
typedef enum cylpa_notify_event_type
{
    CYLPA_NW_SUSPENDED = 0, /**< State when network stack is suspended */
    CYLPA_NW_RESUMING  = 1  /**< State when network stack is resuming */
} cylpa_notify_event_type_t;

/*******************************************************************************
* Function Name: cylpa_suspend_ns
********************************************************************************
*
* Summary:
* This function suspends the network stack by taking a lock on the TCP/IP stack.
*
* Return: int32_t: contains status of suspending the network stack.
*
*******************************************************************************/
int32_t cylpa_suspend_ns(void);

/*******************************************************************************
* Function Name: cylpa_resume_ns
********************************************************************************
*
* Summary:
* This function suspends the network stack by unlocking the TCP/IP stack.
*
* Return: int32_t: contains status of resuming the network stack.
*
*******************************************************************************/
int32_t cylpa_resume_ns(void);

/*******************************************************************************
* Function Name: cylpa_on_emac_activity
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

void cylpa_on_emac_activity(bool is_tx_activity);

/*******************************************************************************
* Function Name: cylpa_wait_net_inactivity
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

int32_t cylpa_wait_net_inactivity(uint32_t inactive_interval_ms, uint32_t inactive_window_ms);


/** @addtogroup lpautilities LPA Utilities API
 *  The Documentation is provided for Network Utility functions
 *  used by the LPA application to demonstrate the Low power functionality.
 * \{
 *
 *
 */

/** Application callback function.
 *
 *  This LPA callback function is used for notification of network stack suspend to the application.
 *
 *  @param   notify_type      : notify event type
 *  @param   *arg             : pointer to WLAN interface
 *
 */
typedef void (*cylpa_suspend_callback_t)(cylpa_notify_event_type_t notify_type, void *arg);

/** Registers suspend notify callback.
 *
 *
 *  @param   *callback        : Application callback function which needs to be called on  network suspend.
 *  @param   *user_data       : Pointer to user data to be passed in the event callback.
 *
 *  @return int32_t           : CY_RSLT_SUCCESS on success, otherwise it returns error.
 */
int32_t cylpa_register_suspend_notify_callback( cylpa_suspend_callback_t callback, void *user_data);

/** Network Monitor Function
 *
 *  In this function the network is monitored for inactivity in an interval of
 *  length inactive_interval_ms. If the network is inactive for a continuous
 *  duration specified by inactive_window_ms, the TCP/IP network stack is
 *  suspended and stays suspended until either there is emac activity or a
 *  duration of wait_ms has expired. Network stack is resumed on
 *  either detecting emac activity or if wait_ms time has elapsed.
 *
 *  @param   *net_intf             : pointer to WLAN interface
 *  @param   wait_ms               : The interval for which the network is monitored
 *                                   for activity. Network stack is resumed ether at the end of
 *                                   wait_ms or on emac activity.
 *  @param   inactive_interval_ms  : The interval for which the network is monitored
 *                                   for inactivity.
 *  @param   inactive_window_ms    : The continuous duration for which network has to
 *                                   be inactive in inactive_interval_ms.
 *
 *  @return int32_t                : Returns status on one of these cases: Network error status if the network stack suspension failed or EMAC activity status as a result of network inactivity monitor.
 */
extern int32_t wait_net_suspend(void *net_intf, uint32_t wait_ms, uint32_t inactive_interval_ms, uint32_t inactive_window_ms);

/*! \cond */
/** Network Monitor Function
 *
 *  This function should be used for waking up Network Stack from suspended/lock state.
 *  TCPIP Core will locked in wait_net_suspend function for putting MCU to Deep Sleep.
 *  Any task which needs to queue to LwIP should be call this function to unlock TCPIP
 *  Core.
 *
 *  @param is_tx_activity  : set event bit for TX/RX activity
 *
 *
 *  @return void
 */
extern void cylpa_on_emac_activity(bool is_tx_activity);
/*! \endcond */

/** Creates TCP Socket connection
 *
 *  This function can be used as reference to create tcp socket connection to
 *  provided remote ip address, remote port, local port and the tcp socket handle is returned in *socket_handle
 *
 *  @param net_intf                : Pointer to WLAN interface
 *  @param socket_handle           : Pointer to an Array of TCP Socket Handle which is populated by this API
 *  @param remote_ip               : Pointer to Remote TCP IP Address
 *  @param remote_port             : Pointer to Remote TCP IP Port number
 *  @param local_port              : Pointer to Local TCP IP Port number
 *  @param downloaded              : Pointer to TKO offload list
 *  @param socket_keepalive_enable : Host TCP Keepalive enable 1 or 0 disable.
 *  @return int                    : Returns CY_RSLT_SUCCESS if the TCP socket connection is successful, otherwise it returns a socket error status
 */
extern int cy_tcp_create_socket_connection(void *net_intf, void **socket_handle, const char *remote_ip, uint16_t remote_port, uint16_t local_port,
    cy_tko_ol_cfg_t *downloaded, int socket_keepalive_enable);

/** Restarts OLM
 *
 *  This function restarts OLM manager with configuration passed by the caller.
 *  @note: This API should only be invoked when device is not connected to AP,
 *  if it is connected to an AP then it has to do disconnect first and then
 *  invoke this API
 *
 *  @param  offload_list           : Pointer to the user defined configuration list \ref ol_desc_t
 *  @param  net_intf               : Pointer to WLAN interface
 *  @return int                    : Returns CY_RSLT_SUCCESS if the restart is successful, otherwise it returns error.
 */
extern int cylpa_restart_olm( ol_desc_t *offload_list, void *net_intf );

/** Finds OLM descriptor for a given name
 *
 *  This function finds the OLM descriptor for a given name
 *  passed by the caller in a OLM descriptor list.
 *
 *  @param  name                   : Pointer to the string name of OLM descriptor
 *  @param  offload_list           : OLM descriptor list
 *  @return ol_desc_t              : Returns OLM descriptor matching the name, otherwise it returns NULL
 */
extern ol_desc_t *cylpa_find_my_descriptor(const char *name, ol_desc_t *offload_list );

/*! \cond */
/** Get OLM instance
 *
 *  This function returns OLM instance
 *  @return  olm_t      : Return OLM structure pointer
 *
 */
extern void *cy_get_olm_instance( void );
/*! \endcond */
/** \} */
#ifdef __cplusplus
} /* extern C */
#endif
