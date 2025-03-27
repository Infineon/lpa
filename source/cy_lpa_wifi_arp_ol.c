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
* @file cy_lpa_wifi_arp_ol.c
* @brief Low Power Offload ARP Assist implementation.
*/

#include <stdio.h>

#include "string.h"
#include "cy_lpa_wifi_ol_debug.h"
#include "cy_lpa_wifi_result.h"
#include "cy_lpa_wifi_ol.h"
#include "cy_lpa_wifi_ol_priv.h"
#include "cy_lpa_wifi_arp_ol.h"

#include "cy_nw_helper.h"
#include "cy_nw_lpa_helper.h"
#include "ip4string.h"
#include "cy_worker_thread.h"
#include "whd_wifi_api.h"
#include "cyabs_rtos.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* Public definitions
*******************************************************************************/

/******************************************************************************/
/** \addtogroup group_lpa_macros_arp *//** \{ */
/******************************************************************************/

#define NULL_IP_ADDRESS                     (0UL)       /**< Invalid IP address */
#define CY_ARPOL_SHORT_DELAY_FOR_DHCP_MS    (500UL)    /**< Delay (ms) wait after link is up for DHCP to start */
#define CY_ARPOL_DELAY_FOR_DHCP_MS          (1000UL)    /**< Delay (ms) wait after link is up for DHCP to start */
#define CY_ARPOL_DHCP_RETRY_COUNT           (25UL)    /**< Number of times to retry getting the IP address */

static ol_init_t cylpa_arp_ol_init;            /**< Initialization of an arp_ol instance */
static ol_deinit_t cylpa_arp_ol_deinit;        /**< Deinitialization of an arp_ol instance */
static ol_pm_t cylpa_arp_ol_pm;                /**< Power manager change of power status */

/** \} */

/******************************************************************************
 *
 *         Data
 *
 *****************************************************************************/

/* Function list for the OLM */
const ol_fns_t arp_ol_fns =
{
    .init = cylpa_arp_ol_init,
    .deinit = cylpa_arp_ol_deinit,
    .pm = cylpa_arp_ol_pm,
};

/******************************************************************************
 *
 *         Variables
 *
 *****************************************************************************/
/* Structure for sal callback info */
static cylpa_nw_ip_status_change_callback_t cy_arp_ol_cb;

/* Timer for waiting for DHCP to get moving after a link up */
static cy_timer_t cy_delay_dhcp_timer;

/*******************************************************************************
* Function Prototypes
*******************************************************************************/

static void cylpa_arp_ol_nw_ip_change_timer_callback(cy_timer_callback_arg_t arg);
static int cy_dhcp_retry_count = CY_ARPOL_DHCP_RETRY_COUNT;

/******************************************************************************/
/** \cond SECTION_LPA_INTERNAL */
/** \addtogroup group_lpa_internal *//** \{ */
/******************************************************************************/

/*******************************************************************************
* Function Name: cylpa_arp_ol_nw_ip_change_work
****************************************************************************//**
*
* Called by the worker thread created in cylpa_olm_init() and stored as ol_info_ptr->worker
* Deferred to the worker thread in cylpa_arp_ol_nw_ip_change_callback() below
*
* \param iface
* Opaque sal pointer use for NetworkStack.
*
* \param arg
* Pointer to arp_ol_t structure.
*
*******************************************************************************/

static void cylpa_arp_ol_nw_ip_change_work(void *arg)
{
    cy_nw_ip_address_t addr;
    cy_nw_ip_interface_t iface;
    volatile arp_ol_t *arp_ol = (arp_ol_t *)arg;
    bool ok = true;

    /* We know we are in Power on State if we get here
     */
    if ( (arp_ol == NULL) || (arp_ol->ol_info_ptr == NULL) )
    {
        OL_LOG_ARP(LOG_OLA_LVL_ERR, "cylpa_arp_ol_nw_ip_change_work() Bad Args.\n");
        return;
    }
    iface = (cy_nw_ip_interface_t)arp_ol->ol_info_ptr->ip;

    ok = cy_nw_ip_get_ipv4_address(iface, &addr);
    OL_LOG_ARP(LOG_OLA_LVL_DEBUG, "%s() iface:%p get ipv4 ok?%d ipv4: %d.%d.%d.%d\n", __func__, iface, ok,
               (int)( (addr.ip.v4 >>  0) & 0xFF ), (int)( (addr.ip.v4 >>  8) & 0xFF ),
               (int)( (addr.ip.v4 >> 16) & 0xFF ), (int)( (addr.ip.v4 >> 24) & 0xFF ) );

    if (ok && (addr.ip.v4 != NULL_IP_ADDRESS) )
    {
        if (addr.ip.v4 != arp_ol->ip_address)
        {
            /* Remove address from HostIP list
             * We know we are in Power on State if we get here
             */
            if (whd_arp_hostip_list_clear_id(arp_ol->ol_info_ptr->whd, arp_ol->ip_address) != WHD_SUCCESS)
            {
                OL_LOG_ARP(LOG_OLA_LVL_DEBUG, "ARPOL IP CLEAR B4 ADD clear FAIL (OK): %d.%d.%d.%d\n",
                           (int)( (addr.ip.v4 >>  0) & 0xFF ), (int)( (addr.ip.v4 >>  8) & 0xFF ),
                           (int)( (addr.ip.v4 >> 16) & 0xFF ), (int)( (addr.ip.v4 >> 24) & 0xFF ) );
            }
            arp_ol->ip_address = NULL_IP_ADDRESS;
        }

        /* Add address to HostIP list if enabled
         * We know we are in Power on State if we get here
         */
        if (whd_arp_hostip_list_add(arp_ol->ol_info_ptr->whd, &addr.ip.v4, 1) != WHD_SUCCESS)
        {
            OL_LOG_ARP(LOG_OLA_LVL_NOTICE, "ARPOL IP ADD FAIL: %d.%d.%d.%d\n",
                       (int)( (addr.ip.v4 >>  0) & 0xFF ), (int)( (addr.ip.v4 >>  8) & 0xFF ),
                       (int)( (addr.ip.v4 >> 16) & 0xFF ), (int)( (addr.ip.v4 >> 24) & 0xFF ) );
        }
        arp_ol->ip_address = addr.ip.v4;

        cy_dhcp_retry_count = CY_ARPOL_DHCP_RETRY_COUNT;
    }
    else
    {
        if (arp_ol->ip_address != NULL_IP_ADDRESS)
        {
            if (whd_arp_hostip_list_clear_id(arp_ol->ol_info_ptr->whd, arp_ol->ip_address) != WHD_SUCCESS)
            {
                OL_LOG_ARP(LOG_OLA_LVL_DEBUG, "ARPOL IP CLEAR FAIL (OK): %d.%d.%d.%d\n",
                           (int)( (addr.ip.v4 >>  0) & 0xFF ), (int)( (addr.ip.v4 >>  8) & 0xFF ),
                           (int)( (addr.ip.v4 >> 16) & 0xFF ), (int)( (addr.ip.v4 >> 24) & 0xFF ) );
            }
            arp_ol->ip_address = NULL_IP_ADDRESS;
        }

        /* whd_res values
         * WHD_JOIN_IN_PROGRESS    :0x20003ff
         * WHD_INVALID_JOIN_STATUS :0x2000401
         * WHD_UNKNOWN_INTERFACE   :0x2000402
         * WHD_INTERFACE_NOT_UP    :0x2000410
         * */
        int whd_res = whd_wifi_is_ready_to_transceive(arp_ol->ol_info_ptr->whd);
        if ( (whd_res == WHD_SUCCESS) ||
             (whd_res == WHD_INVALID_JOIN_STATUS) ||
             (whd_res == WHD_JOIN_IN_PROGRESS) )
        {
            if (arp_ol->ip_address == NULL_IP_ADDRESS)
            {
                if (--cy_dhcp_retry_count > 0)
                {
                    /* Start timer for future to handle timing of DHCP issue */
                    cy_rtos_start_timer(&cy_delay_dhcp_timer, CY_ARPOL_DELAY_FOR_DHCP_MS);
                }
            }
        }
    }
}

/*******************************************************************************
 * Function Name: cylpa_arp_ol_nw_ip_change_timer_callback
 ****************************************************************************/

static void cylpa_arp_ol_nw_ip_change_timer_callback(cy_timer_callback_arg_t arg)
{
    arp_ol_t *arp_ol = (arp_ol_t *)arg;
    cy_rtos_timer_stop(&cy_delay_dhcp_timer);
    if ( (arp_ol == NULL) || (arp_ol->ol_info_ptr == NULL) || (arp_ol->ol_info_ptr->worker == NULL) )
    {
        return;
    }
    cy_worker_thread_enqueue(arp_ol->ol_info_ptr->worker, cylpa_arp_ol_nw_ip_change_work, (void *)arg);
}

/*******************************************************************************
* Function Name: cylpa_arp_ol_nw_ip_change_callback
*
* Initialize the callback with sal in cylpa_arp_ol_init()
* Register/unregister the callback with sal in cylpa_arp_ol_pm() or cylpa_arp_ol_deinit()
* Called by sal when sal receives a callback from the NetworkStack
* We defer calls to the worker thread, if available.
*
* \param iface
* Opaque sal pointer use for NetworkStack.
*
* \param arg
* Pointer to arp_ol_t structure.
*
*******************************************************************************/
static void cylpa_arp_ol_nw_ip_change_callback(cy_nw_ip_interface_t iface, void *arg)
{

    arp_ol_t *arp_ol = (arp_ol_t *)arg;
    if ( (arp_ol == NULL) || (arp_ol->ol_info_ptr == NULL) || (arp_ol->ol_info_ptr->worker == NULL) )
    {
        return;
    }

    /* Start a timer to handle timing of DHCP */
    cy_rtos_start_timer(&cy_delay_dhcp_timer, CY_ARPOL_SHORT_DELAY_FOR_DHCP_MS);
    return;
}


/******************************************************************************/
/** \addtogroup group_lpa_high_level *//** \{ */
/******************************************************************************/

/*******************************************************************************
* Function Name: cylpa_arp_ol_init
****************************************************************************//**
*
* Initialize the callback with sal in arp_ol_init()
* Register/Unregister the callback with sal in cylpa_arp_ol_pm() or cylpa_arp_ol_deinit()
* Called by sal when sal receives a callback from the NetworkStack
* We defer calls to the worker thread, if available.
*
* \param ol
* Pointer to arp_ol_t structure.
*
* \param ol_info_ptr
* Pointer to ol_info_t structure.
*
* \param cfg
* Pointer to arp_ol_cfg_t structure.
*
*******************************************************************************/
static int cylpa_arp_ol_init(void *ol, ol_info_t *ol_info, const void *cfg)
{
    arp_ol_t *arp_ol = (arp_ol_t *)ol;

    OL_LOG_ARP(LOG_OLA_LVL_DEBUG, "cylpa_arp_ol_init()\n");

    if ( (ol == NULL) || (ol_info == NULL) || (cfg == NULL) )
    {
        OL_LOG_ARP(LOG_OLA_LVL_DEBUG, "cylpa_arp_ol_init() Bad pointers arp_ol:%p ol_info:%p cfg:%p\n", ol, (void *)ol_info,
                   cfg);
        return RESULT_BADARGS;
    }

    /* clear out structure */
    memset(arp_ol, 0x00, sizeof(arp_ol_t) );

    /* copy for local storage so we can pass to callback/worker/pm change */
    strcpy(arp_ol->name, "ARP");
    arp_ol->config  = (arp_ol_cfg_t *)cfg;
    arp_ol->ol_info_ptr = ol_info;
    arp_ol->state   = ARP_OL_STATE_UNINITIALIZED;

    /* Do not configure ARP offload for new infra */
    if(arp_ol->ol_info_ptr->fw_new_infra)
    {
        /* ARP offload is enabled by default */
        OL_LOG_ARP(LOG_OLA_LVL_INFO, "ARP offload enabled by default\n");
        return RESULT_OK;
    }

    /* Initialize the timer to handle DHCP timing */
    cy_rtos_init_timer(&cy_delay_dhcp_timer, CY_TIMER_TYPE_ONCE, cylpa_arp_ol_nw_ip_change_timer_callback,
                       (cy_timer_callback_arg_t)arp_ol);

    /* Initialize the SAL IP change callback
     * - registered in the PM change callback below
     * - only used if SNOOP is off
     */
    cylpa_nw_ip_initialize_status_change_callback(&cy_arp_ol_cb, cylpa_arp_ol_nw_ip_change_callback, arp_ol);

    /* Clear out all ARP Offload features */
    whd_arp_arpoe_set(arp_ol->ol_info_ptr->whd, 1);
    whd_arp_features_set(arp_ol->ol_info_ptr->whd, 0);
    whd_arp_cache_clear(arp_ol->ol_info_ptr->whd);
    whd_arp_stats_clear(arp_ol->ol_info_ptr->whd);
    whd_arp_hostip_list_clear(arp_ol->ol_info_ptr->whd);
    whd_arp_peerage_set(arp_ol->ol_info_ptr->whd, arp_ol->config->peerage);
    whd_arp_arpoe_set(arp_ol->ol_info_ptr->whd, 0);

    /* TODO: We get here only if we are awake! Do we start up features as if we just woke up? */
    cylpa_arp_ol_pm(arp_ol, OL_PM_ST_AWAKE);

    return RESULT_OK;
}

/*******************************************************************************
* Function Name: cylpa_arp_ol_deinit
****************************************************************************//**
*
* De-Initialization of an ARP OL instance.
* Called by Offload Power Manager.
*
* \param ol
* Pointer to arp_ol_t structure.
*
*******************************************************************************/
static void cylpa_arp_ol_deinit(void *ol)
{
    arp_ol_t *arp_ol = (arp_ol_t *)ol;

    OL_LOG_ARP(LOG_OLA_LVL_DEBUG, "cylpa_arp_ol_deinit()\n");

    if (arp_ol == NULL)
    {
        return;
    }

    if(arp_ol->ol_info_ptr->fw_new_infra)
    {
        return;
    }

    arp_ol->state   = ARP_OL_STATE_UNINITIALIZED;

    /* Un-register the ip change callback with sal api */
    cylpa_nw_ip_unregister_status_change_callback( (uintptr_t)arp_ol->ol_info_ptr->ip, &cy_arp_ol_cb );
    /* deinit timer */
    cy_rtos_deinit_timer(&cy_delay_dhcp_timer);

    /* Turn off ARP OL when we de-init ? */
    whd_arp_arpoe_set(arp_ol->ol_info_ptr->whd, 0);
}

/*******************************************************************************
* Function Name: cylpa_arp_ol_pm
****************************************************************************//**
*
* Power manager change for an ARP OL instance.
*
* \param ol
* Pointer to arp_ol_t structure.
*
* \param st
* New Power Manager State (ol_pm_st_t).
*
*******************************************************************************/
static void cylpa_arp_ol_pm(void *ol, ol_pm_st_t st)
{
    arp_ol_t *arp_ol = (arp_ol_t *)ol;
    arp_ol_config_state_t new_arp_ol_state;

    if ( (arp_ol == NULL) || (arp_ol->ol_info_ptr->whd == NULL) )
    {
        OL_LOG_ARP(LOG_OLA_LVL_ERR, "cylpa_arp_ol_pm() Bad Args!\n");
        return;
    }

    if (arp_ol->ol_info_ptr->fw_new_infra)
    {
        OL_LOG_ARP(LOG_OLA_LVL_INFO, "ARP offload enabled by default\n");
        return;
    }

    if ( arp_ol->config == NULL )
    {
        OL_LOG_ARP(LOG_OLA_LVL_ERR, "cylpa_arp_ol_pm() Bad Args!\n");
        return;
    }

    OL_LOG_ARP(LOG_OLA_LVL_DEBUG, "cylpa_arp_ol_pm(arp:%p, cfg:%p st:%d - %s)\n", (void *)arp_ol, (void *)arp_ol->config, st,
                  (st == OL_PM_ST_AWAKE) ? "Awake" : "Sleep");

    new_arp_ol_state = (st == OL_PM_ST_AWAKE) ? ARP_OL_STATE_AWAKE : ARP_OL_STATE_GOING_TO_SLEEP;
    if (new_arp_ol_state == arp_ol->state)
    {
        return;
    }

    /* Set ARP Offload Enable  & feature settings */
    arp_ol_enable_mask_t enable_flags = 0UL;
    if ( (st == OL_PM_ST_AWAKE) && (arp_ol->config->awake_enable_mask != 0) )
    {
        enable_flags = arp_ol->config->awake_enable_mask;
    }
    if ( (st == OL_PM_ST_GOING_TO_SLEEP) && ( (arp_ol->config->sleep_enable_mask) != 0 ) )
    {
        enable_flags = arp_ol->config->sleep_enable_mask;
    }
    if ( (arp_ol->state == ARP_OL_STATE_UNINITIALIZED) ||
         (arp_ol->config->awake_enable_mask != arp_ol->config->sleep_enable_mask) )
    {
        cylpa_nw_ip_register_status_change_callback( (uintptr_t)arp_ol->ol_info_ptr->ip, &cy_arp_ol_cb );

        /* check that AGENT is on if HOST_AUTO_REPLY or PEER_AUTO_REPLY is on */
        if ( (enable_flags & (CY_ARP_OL_HOST_AUTO_REPLY_ENABLE | CY_ARP_OL_PEER_AUTO_REPLY_ENABLE) ) != 0 )
        {
            enable_flags |= CY_ARP_OL_AGENT_ENABLE;
        }

        /* enable ARP Offload */
        whd_arp_arpoe_set(arp_ol->ol_info_ptr->whd, 1);

        /* set the features */
        OL_LOG_ARP(LOG_OLA_LVL_DEBUG, "whd_arp_features_set(0x%x)!\n", enable_flags);
        if (whd_arp_features_set(arp_ol->ol_info_ptr->whd, enable_flags) != WHD_SUCCESS)
        {
            OL_LOG_ARP(LOG_OLA_LVL_DEBUG, "cylpa_arp_ol_pm() whd_arp_features_set() Failed!\n");
        }
    }
    else
    {
        /* disable features */
        whd_arp_features_set(arp_ol->ol_info_ptr->whd, 0);

        /* disable ARP Offload */
        whd_arp_arpoe_set(arp_ol->ol_info_ptr->whd, 0);
    }
    arp_ol->state   = new_arp_ol_state;
}

/** \} \endcond SECTION_LPA_INTERNAL */

#ifdef __cplusplus
}
#endif

