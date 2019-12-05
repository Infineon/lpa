/***************************************************************************//**
* \file cy_lpa_wifi_olm.c
* \version 1.0
*
* \brief
* Low Power Offload Assist
*
********************************************************************************
* \copyright
* Copyright 2019, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#include <string.h>
#include "cy_lpa_wifi_ol_debug.h"
#include "cy_lpa_wifi_result.h"
#include "cy_lpa_wifi_ol.h"
#include "cy_lpa_wifi_olm.h"
#include "cy_lpa_wifi_ol_priv.h"

#if !defined(OLM_NO_HARDWARE)
#include "mbed_toolchain.h"
#include "cy_worker_thread.h"
#endif

#if defined(MBED_CONF_APP_OLM_TEST)
#include <stdarg.h>
#endif

static const ol_desc_t null_ol_list = {NULL, NULL, NULL, NULL};

/* We have a non-hardware build that we need to handle - "OLM_NO_HARDWARE" defined in app/olm-sanity/Makefile */
#if defined(OLM_NO_HARDWARE)

#ifndef MIN
extern int MIN(/*@sef@*/ int x, /*@sef@*/ int y);  /* LINT : This tells lint that  the parameter must be side-effect free. i.e. evaluation does not change any values (since it is being evaulated more than once */
#define MIN(x, y) ( (x) < (y) ? (x) : (y) )
#endif /* ifndef MIN */

/**
 * Converts a unsigned long int to a decimal string
 *
 * @param value[in]      : The unsigned long to be converted
 * @param output[out]    : The buffer which will receive the decimal string
 * @param min_length[in] : the minimum number of characters to output (zero padding will apply if required)
 * @param max_length[in] : the maximum number of characters to output (up to 10 )
 *
 * @note: No trailing null is added.
 *
 * @return the number of characters returned.
 *
 */
uint8_t unsigned_to_decimal_string(uint32_t value, char *output, uint8_t min_length, uint8_t max_length);

#endif /* defined(OLM_NO_HARDWARE) */

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* Local definition
*******************************************************************************/
#define OLM_WORKER_THREAD_STACK_SIZE    (6 * 1024)  /* >4k needed for printf()calls in worker thread */

#if !defined(OLM_NO_HARDWARE)
static cy_worker_thread_info_t olm_worker_thread;

/* Define a stack buffer so we don't require a malloc when creating our worker thread Aligned on 8-byte boundary! */
MBED_ALIGN(CY_RTOS_ALIGNMENT) static uint8_t olm_worker_thread_stack[OLM_WORKER_THREAD_STACK_SIZE];

static cy_worker_thread_params_t olm_thread_params =
{
    /*
     * CY_WORKER_THREAD_PRIORITY_BELOW_NORMAL does not work for EventQueue->dispatch()
     * 1st deferred function is called, second is not called.
     * EventQueue->time_left first call returns -1,  indicated completed
     * EventQueue->time_left second call returns  0, indicating event is already due to be dispatched or is currently executing.
     */
    .priority = CY_WORKER_THREAD_PRIORITY_NORMAL,
    .stack_size = sizeof(olm_worker_thread_stack),
    .stack = olm_worker_thread_stack,
    .name = "OLM Worker"
};
#endif  /* !defined(OLM_NO_HARDWARE) */

/*******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************/
/** \addtogroup group_lpa_high_level *//** \{ */
/******************************************************************************/
/** \} */


/*******************************************************************************
* Function Name: olm_init
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
void olm_init(olm_t *olm, const ol_desc_t *ol_list)
{
    if (olm == NULL)
    {
        OL_LOG_OLM(LOG_OLA_LVL_ERR, "olm_init() Bad Arg\n");
        return;
    }
    memset( (void *)olm, 0x00, sizeof(*olm) );

    OL_LOG_OLM(LOG_OLA_LVL_INFO, "olm_init() olm:%p ol_list:%p\n", (void *)olm, (void *)ol_list);

    olm->ol_list = ol_list ? ol_list : &null_ol_list;

#if !defined(OLM_NO_HARDWARE)
    if (olm_worker_thread.tag != CY_WORKER_VALID_TAG)
    {
        cy_create_worker_thread(&olm_thread_params, &olm_worker_thread);
    }
    olm->ol_info.worker = &olm_worker_thread;
    OL_LOG_OLM(LOG_OLA_LVL_DEBUG, "olm_init()  olm:%p ol_list:%p worker:%p tag:0x%lx\n", (void *)olm, (void *)ol_list,
               olm->ol_info.worker, olm_worker_thread.tag);
#endif /* !OLM_NO_HARDWARE */

    OL_LOG_OLM(LOG_OLA_LVL_DEBUG, "olm_init()  Done\n");
}

/*******************************************************************************
* Function Name: olm_deinit
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
void olm_deinit(olm_t *olm)
{
    OL_LOG_OLM(LOG_OLA_LVL_DEBUG, "olm_deinit() olm:%p\n", (void *)olm);

#if !defined(OLM_NO_HARDWARE)
    cy_delete_worker_thread(olm->ol_info.worker);
    olm->ol_info.worker = NULL;
#endif  /* !defined(OLM_NO_HARDWARE) */

    OL_LOG_OLM(LOG_OLA_LVL_DEBUG, "olm_deinit() Done\n");
}

/*******************************************************************************
* Function Name: olm_init_ols
****************************************************************************//**
*
* This function initializes the Offload Manager.
*
* The initialization process includes following steps:
* 1. TDB.
* 2. TDB.
* 3. TDB.
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
int olm_init_ols(olm_t *olm, void *whd, void *ip)
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

    OL_LOG_OLM(LOG_OLA_LVL_INFO, "%s\n", __func__);

    for (it0 = olm->ol_list; it0->fns; it0++)
    {
        if (it0->fns->init != NULL)
        {
            result = (*it0->fns->init)(it0->ol, &olm->ol_info, it0->cfg);
            if (result != RESULT_OK)
            {
                OL_LOG_OLM(LOG_OLA_LVL_ERR, "%s offload %s fatal %d\n", __func__, it0->name, result);
                break;
            }
        }
    }

    if (result != RESULT_OK)
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
* Function Name: olm_deinit_ols
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
void olm_deinit_ols(olm_t *olm)
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
* Function Name: olm_dispatch_pm_notification
****************************************************************************//**
*
* This function dispatches pm notifications.
*
* This includes following steps:
* 1. TDB.
* 2. TDB.
* 3. TDB.
*
* \param olm
* The pointer to the olm structure \ref olm_t.
*
* \param st
* st New Power State ( see ol_pm_st_t)
*
*******************************************************************************/
void olm_dispatch_pm_notification(olm_t *olm, ol_pm_st_t st)
{
    const ol_desc_t *it;

    if (olm == NULL)
    {
        OL_LOG_OLM(LOG_OLA_LVL_ERR, "%s Bad Arg\n", __func__);
        return;
    }

    if (olm->ol_list == NULL)
    {
        OL_LOG_OLM(LOG_OLA_LVL_INFO, "%s BAD ARGS\n", __func__);
        return;
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

#if defined(MBED_CONF_APP_OLM_TEST)

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

