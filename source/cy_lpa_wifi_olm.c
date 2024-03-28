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
* @file cy_lpa_wifi_olm.c
* @brief Low Power Offload Assist
*/

#include <string.h>
#include <stdbool.h>
#include "cy_lpa_wifi_ol_debug.h"
#include "cy_lpa_wifi_result.h"
#include "cy_lpa_wifi_ol.h"
#include "cy_lpa_wifi_olm.h"
#include "cy_lpa_wifi_ol_priv.h"
#include "whd_int.h"

#include "cy_worker_thread.h"

#include <stdarg.h>

/* global flag to check for whd instance creation */
bool cy_olm_whd_instance_created = false;

/* pm2 sleep return value before going to sleep */
uint32_t cy_pm2_sleep_ret_value;

static const ol_desc_t cy_null_ol_list = {NULL, NULL, NULL, NULL};

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* Local definition
*******************************************************************************/
#define OLM_WORKER_THREAD_STACK_SIZE    (6 * 1024)  /* >4k needed for printf()calls in worker thread */

static cy_worker_thread_info_t cy_olm_worker_thread;

/* Define a stack buffer so we don't require a malloc when creating our worker thread Aligned on 8-byte boundary! */
__attribute__((aligned(CY_RTOS_ALIGNMENT))) static uint8_t cy_olm_worker_thread_stack[OLM_WORKER_THREAD_STACK_SIZE];

static cy_worker_thread_params_t cy_olm_thread_params =
{
    /*
     * CY_WORKER_THREAD_PRIORITY_BELOW_NORMAL does not work for EventQueue->dispatch()
     * 1st deferred function is called, second is not called.
     * EventQueue->time_left first call returns -1,  indicated completed
     * EventQueue->time_left second call returns  0, indicating event is already due to be dispatched or is currently executing.
     */
    .priority = CY_RTOS_PRIORITY_NORMAL,
    .stack_size = sizeof(cy_olm_worker_thread_stack),
    .stack = cy_olm_worker_thread_stack,
    .name = "OLM Worker",
    .num_entries = 0
};

/*******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************/
/** \addtogroup group_lpa_high_level *//** \{ */
/******************************************************************************/
/** \} */


/*******************************************************************************
* Function Name: cylpa_olm_init
****************************************************************************//**
*
* Offload Manager initialization function.
*
* Initialize the Offload Manager with a NULL-terminated list of offload descriptors
* (instance context, function pointers and instance configuration).
*
* \param olm
* The pointer to the olm structure \ref olm_t.
*
* \param ol_list
* The pointer to the ol_list structure \ref ol_desc_t.
*
*******************************************************************************/
void cylpa_olm_init(olm_t *olm, const ol_desc_t *ol_list)
{
    whd_interface_t iface = NULL;
    ol_info_t *olm_info = NULL;
    uint32_t value = 0;

    if (olm == NULL)
    {
        OL_LOG_OLM(LOG_OLA_LVL_ERR, "cylpa_olm_init() Bad Arg\n");
        return;
    }

    olm_info = &olm->ol_info;
    OL_LOG_OLM(LOG_OLA_LVL_INFO, "cylpa_olm_init() olm:%p ol_list:%p\n", (void *)olm, (void *)ol_list);

    olm->ol_list = ol_list ? ol_list : &cy_null_ol_list;

    if (whd_wifi_get_fwcap(olm_info->whd, &value) == WHD_SUCCESS)
    {
        if (value & (1 << WHD_FWCAP_OFFLOADS) )
        {
            olm->ol_info.fw_new_infra = 1;
        }
        else
        {
            olm->ol_info.fw_new_infra = 0;
        }
    }

    /* Create worker thread only for old infra to handle callbacks */
    if ( olm->ol_info.fw_new_infra == 0 )
    {
        if (cy_olm_worker_thread.state != CY_WORKER_THREAD_VALID)
        {
            cy_worker_thread_create(&cy_olm_worker_thread, &cy_olm_thread_params);
        }
        olm->ol_info.worker = &cy_olm_worker_thread;
        OL_LOG_OLM(LOG_OLA_LVL_DEBUG, "cylpa_olm_init()  olm:%p ol_list:%p worker:%p state:0x%lx whd:%x\n", (void *)olm, (void *)ol_list,
                   olm->ol_info.worker, cy_olm_worker_thread.state, olm->ol_info.whd);
    }

    iface = olm_info->whd;

    if ( iface != NULL )
    {
        cylpa_olm_init_wlan_config (iface );
        cy_olm_whd_instance_created = true;
    }
    else
    {
        OL_LOG_OLM(LOG_OLA_LVL_DEBUG, "whd interface is null!! connect STA to AP\n");
    }

    OL_LOG_OLM(LOG_OLA_LVL_DEBUG, "cylpa_olm_init() Done iface:%p\n", iface );
}

/*******************************************************************************
* Function Name: cylpa_olm_deinit
****************************************************************************//**
*
* Offload Manager offloads initialization function.
*
* Initialize each offload in the offload list by calling each offload's initialization function
* (whd pointer is passed for IOVAR calls, and network interface for ip).
* If an offload initialization fails, stop and deinitialize any initialized offloads and return
* the result code of the failing initialization function.
*
* \param olm
* The pointer to the olm structure \ref olm_t.
*
*******************************************************************************/
void cylpa_olm_deinit(olm_t *olm)
{
    OL_LOG_OLM(LOG_OLA_LVL_DEBUG, "cylpa_olm_deinit() olm:%p\n", (void *)olm);

    if ( olm->ol_info.fw_new_infra == 1 )
    {
        return;
    }

    cy_worker_thread_delete(olm->ol_info.worker);
    olm->ol_info.worker = NULL;

    OL_LOG_OLM(LOG_OLA_LVL_DEBUG, "cylpa_olm_deinit() Done\n");
}

/*******************************************************************************
* Function Name: cylpa_olm_init_ols
****************************************************************************//**
*
* This function initializes the Offload Manager.
*
* \param olm
* The pointer to the olm structure \ref olm_t.
*
* \param whd
* The pointer to whd.
*
* \param ip
* The pointer to ip.
*
*******************************************************************************/
int cylpa_olm_init_ols(olm_t *olm, void *whd, void *ip)
{
    const ol_desc_t *it0, *it1;
    int result = RESULT_OK;

    olm->ol_info.whd = whd;
    olm->ol_info.ip = ip;

    if (olm->ol_list == NULL)
    {
        OL_LOG_OLM(LOG_OLA_LVL_INFO, "%s BAD ARGS\n", __func__);
        return RESULT_BADARGS;
    }

    /* if whd instance is created after connect to an AP then call cylpa_olm_init_wlan_config */
    if(  ( olm->ol_info.whd != NULL ) && ( cy_olm_whd_instance_created == false ) )
    {
         cylpa_olm_init_wlan_config (olm->ol_info.whd );
         cy_olm_whd_instance_created = true;
    }

    OL_LOG_OLM(LOG_OLA_LVL_INFO, "%s\n", __func__);

    for (it0 = olm->ol_list; it0->fns; it0++)
    {
        if (it0->fns->init != NULL)
        {
            result = (*it0->fns->init)(it0->ol, &olm->ol_info, it0->cfg);
            if ( result != RESULT_OK )
            {
                OL_LOG_OLM(LOG_OLA_LVL_ERR, "%s offload %s fatal %d\n", __func__, it0->name, result);
                break;
            }
        }
    }

    if ( result != RESULT_OK )
    {
        /* Failure */
        /* Undo any initializations. */
        for (it1 = olm->ol_list; it1 != it0; it1++)
        {
            if (it1->fns->deinit != NULL)
            {
                (*it1->fns->deinit)(it1->ol);
            }
        }
    }

    OL_LOG_OLM(LOG_OLA_LVL_DEBUG, "%s Done\n", __func__);
    return result;
}

/*******************************************************************************
* Function Name: cylpa_olm_deinit_ols
****************************************************************************//**
*
* Offload Manager de-initialization function.
*
* Call the de-initialization function for each offload.
*
* \param olm
* The pointer to the olm structure \ref olm_t.
*
*******************************************************************************/
void cylpa_olm_deinit_ols(olm_t *olm)
{
    const ol_desc_t *it;

    OL_LOG_OLM(LOG_OLA_LVL_INFO, "%s olm:%p\n", __func__, (void *)olm);

    if (olm->ol_list == NULL)
    {
        OL_LOG_OLM(LOG_OLA_LVL_INFO, "%s BAD ARGS\n", __func__);
        return;
    }

    for (it = olm->ol_list; it->fns; it++)
    {
        if (it->fns->deinit != NULL)
        {
            (*it->fns->deinit)(it->ol);
        }
    }

    OL_LOG_OLM(LOG_OLA_LVL_DEBUG, "%s Done\n", __func__);
}

/*******************************************************************************
* Function Name: cylpa_olm_dispatch_pm_notification
****************************************************************************//**
*
* This function dispatches pm notifications.
*
* \param olm
* The pointer to the olm structure \ref olm_t.
*
* \param st
* st New Power State ( see ol_pm_st_t)
*
*******************************************************************************/
void cylpa_olm_dispatch_pm_notification(olm_t *olm, ol_pm_st_t st)
{
    const ol_desc_t *it;
    uint32_t pm2_lpa_value = LPA_PM2_SLEEP_RET_TIME;

    if (olm == NULL)
    {
        OL_LOG_OLM(LOG_OLA_LVL_ERR, "%s Bad Arg\n", __func__);
        return;
    }

    if ((olm->ol_list == NULL) || (olm->ol_info.whd == NULL))
    {
        OL_LOG_OLM(LOG_OLA_LVL_INFO, "%s BAD ARGS\n", __func__);
        return;
    }

    if ( st == OL_PM_ST_GOING_TO_SLEEP )
    {
        /* set PM2 sleep return value configured for WLAN Lowest Power */
        cylpa_olm_configure_wlan_pmode(olm->ol_info.whd, pm2_lpa_value, true);
    }
    else
    {
        /*
         * set PM2 sleep return value to default value
         * (i.e value before going to sleep/deep-sleep)
         */
        cylpa_olm_configure_wlan_pmode(olm->ol_info.whd, cy_pm2_sleep_ret_value, false);
    }

    OL_LOG_OLM(LOG_OLA_LVL_INFO, "%s st:%d\n", __func__, st);

    for (it = olm->ol_list; it->fns; it++)
    {
        if (it->fns->pm != NULL)
        {
            (*it->fns->pm)(it->ol, st);
        }
    }
}

/*******************************************************************************
* Function Name: cylpa_olm_init_wlan_config
****************************************************************************//**
*
* This function configures WLAN to operate at lowest possible power
* this function is called once after cylpa_olm_init
*
* This includes following steps:
* 1.  set the bcntrim iovar value ( LPA_BCNTRIM)
* 2.  set the bcn_wait_prd iovar value ( LPA_BCN_WAIT_PERIOD)
* 3.  set the bcn_reacquire_start iovar value ( LPA_BCN_REACQUIRE_START)
* 4.  set the roam_time_thresh iovar value ( LPA_ROAM_TIME_THRESHOLD )
*
* \param whd
* The pointer to the whd interface.
*
*******************************************************************************/
void cylpa_olm_init_wlan_config ( void *whd )
{
    uint32_t bcntrim_value = LPA_BCNTRIM;
    uint32_t bcn_wait_period = 0;
    uint32_t bcn_reacquire_start = LPA_BCN_REACQUIRE_START;
    uint32_t roam_time_threshold = LPA_ROAM_TIME_THRESHOLD;
    whd_interface_t ifp  = (whd_interface_t )whd;

    /* set bcntrim */
    whd_wifi_set_iovar_value ( ifp, "bcntrim ", bcntrim_value );

    /* get bcn_wait_period in ms */
    whd_wifi_get_iovar_value ( ifp, "bcn_wait_prd", &bcn_wait_period);

    if ( bcn_wait_period != LPA_BCN_WAIT_PERIOD )
    {
        /* set beacon wait period */
        whd_wifi_set_iovar_value ( ifp, "bcn_wait_prd", LPA_BCN_WAIT_PERIOD);
    }

    /* set beacon re-acquire time in seconds */
    whd_wifi_set_iovar_value ( ifp, "bcn_reacquire_start", bcn_reacquire_start);

    /* set the roam_time_threshold */
    whd_wifi_set_iovar_value ( ifp, "roam_time_thresh", roam_time_threshold );

    whd_wifi_get_iovar_value (ifp, "bcntrim", &bcntrim_value);
    whd_wifi_get_iovar_value (ifp, "bcn_reacquire_start", &bcn_reacquire_start);
    whd_wifi_get_iovar_value (ifp, "roam_time_thresh", &roam_time_threshold);

    OL_LOG_OLM(LOG_OLA_LVL_DEBUG, "\n\n********************************************************************\n");
    OL_LOG_OLM(LOG_OLA_LVL_DEBUG, "             WLAN Low Power IOVAR config values                    \n");
    OL_LOG_OLM(LOG_OLA_LVL_DEBUG, "bcntrim:%d bcn_wait_period:%d bcn_reacquire_start:%d roam_time_thresh:%d\n",
               bcntrim_value, bcn_wait_period, bcn_reacquire_start, roam_time_threshold );
    OL_LOG_OLM(LOG_OLA_LVL_DEBUG, "********************************************************************\n\n");
}

/*******************************************************************************
* Function Name: cylpa_olm_configure_wlan_pmode
****************************************************************************//**
*
* This function configures WLAN iovar PM2 ( Power Management Mode 2) return to
* sleep value.
*
* \param whd
* The pointer to the whd interface.
*
* \param value
* The iovar value to be set ( in ms)
*
* \param min_power
* boolean value (true) if minimum PM2 value else (false) for defualt value.
*
*******************************************************************************/
void cylpa_olm_configure_wlan_pmode ( void *whd , uint32_t value, bool min_power )
{
    whd_interface_t ifp = whd;
    uint32_t lpa_pm2_value = 0;

    if ( whd != NULL )
    {
        if ( min_power )
        {
            /* get PM2 Global sleep return value */
            whd_wifi_get_iovar_value(ifp, "pm2_sleep_ret", (uint32_t *) &cy_pm2_sleep_ret_value);
            OL_LOG_OLM(LOG_OLA_LVL_DEBUG, "\nGet     default pm2_sleep_ret:%d(ms)\n", cy_pm2_sleep_ret_value);

            /* set PM2 sleep return value for WLAN Lowest Power */
            whd_wifi_enable_powersave_with_throughput(ifp, value);

            whd_wifi_get_iovar_value(ifp, "pm2_sleep_ret", (uint32_t *) &lpa_pm2_value);
            OL_LOG_OLM(LOG_OLA_LVL_DEBUG, "Set     LPA     pm2_sleep_ret:%d(ms)\n\n", lpa_pm2_value);
        }
        else
        {
            /* set PM2 sleep return value to default value */
            whd_wifi_enable_powersave_with_throughput(ifp, value );
            OL_LOG_OLM(LOG_OLA_LVL_DEBUG, "\nRestore default pm2_sleep_ret:%d(ms)\n", cy_pm2_sleep_ret_value);
        }
    }
    else
    {
        OL_LOG_OLM(LOG_OLA_LVL_DEBUG, "Wi-Fi interface is not powered on \n");
    }
}

#if defined(OLM_LOG_ENABLED)

/*******************************************************************************
*
* Debug logging levels for Offload Assist
*
*******************************************************************************/
int ol_log_level[LOG_OLA_MAX_INDEX];

/*******************************************************************************
* This function gets log level for a facility
*
* @param assist : Offload Assistant type @ref LOG_OFFLOAD_ASSIST_T
*
* @return       : currently set log level
*               : -1 BAD ARGS
*
*******************************************************************************/
int ol_log_get_level(LOG_OFFLOAD_ASSIST_T assist)
{
    if (assist >= LOG_OLA_MAX_INDEX)
    {
        return -1;
    }

    return ol_log_level[assist];
}

/*******************************************************************************
* This function sets log level for a facility
*
* @param assist : Offload Assistant type @ref LOG_OFFLOAD_ASSIST_T
* @param level  : Offload assist logging level @ref LOG_OFFLOAD_ASSIST_LEVEL_T
*
* @return       : 0 = success
*                 -1 = BAD ARGS
*
*******************************************************************************/
int ol_log_set_level(LOG_OFFLOAD_ASSIST_T assist, LOG_OFFLOAD_ASSIST_LEVEL_T level)
{
    if ( (assist >= LOG_OLA_MAX_INDEX) || (level > LOG_OLA_LVL_MAX_INDEX) )
    {
        return -1;
    }

    ol_log_level[assist] = level;
    return 0;
}

/*******************************************************************************
* This function tests log level and prints if settings are correct
*
* @param assist : Offload Assistant type @ref LOG_OFFLOAD_ASSIST_T
* @param level  : Offload assist logging level @ref LOG_OFFLOAD_ASSIST_LEVEL_T
* @param fmt    : Format string like printf()
* @param ...    : variable argument list for printf
*
* @return       : # characters printed
*
*******************************************************************************/
int ol_logging(LOG_OFFLOAD_ASSIST_T assist, LOG_OFFLOAD_ASSIST_LEVEL_T level, const char *fmt, ...)
{
    va_list args;
    int count;

    if (assist >= LOG_OLA_MAX_INDEX)
    {
        return 0;
    }

    if (level > ol_log_level[assist])
    {
        return 0;
    }

    va_start(args, fmt);
    count = vprintf(fmt, args);
    va_end(args);
    return count;
}

#endif

#ifdef __cplusplus
}
#endif

