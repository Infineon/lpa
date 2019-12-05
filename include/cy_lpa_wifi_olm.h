/***************************************************************************//**
* \file cy_lpa_wifi_olm.h
* \version 1.0
*
* \brief
* Offload Manager API.
*
********************************************************************************
* \copyright
* Copyright 2019, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#ifndef OLM_H__
#define OLM_H__    (1)

#ifdef __cplusplus
extern "C" {
#endif

#include "cy_lpa_wifi_ol_common.h" /* for ol_info_t */


/*******************************************************************************
* LPA Data Structures
*******************************************************************************/

/******************************************************************************/
/** \addtogroup group_lpa_structures *//** \{ */
/******************************************************************************/

/** Offload Manager API (integrated into the platform/system). */
struct ol_desc;

/** Offload Manager context.
 * Private structure; visible to allow for static definition. */
typedef struct olm
{
    const struct ol_desc *ol_list;      /**< Offload Assist list */
    ol_info_t ol_info;                  /**< Offload info */
} olm_t;

/** \} */


/*******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************/
/** \addtogroup group_lpa_high_level *//** \{ */
/******************************************************************************/

/** Offload Manager initialization function.
 *
 * Initialize the Offload Manager with a NULL-terminated list of offload descriptors
 * (instance context, function pointers and instance configuration).
 *
 * @param olm        : The pointer to the olm structure @ref olm_t.
 * @param ol_list    : The pointer to the ol_list structure @ref ol_desc_t.
 *
 *******************************************************************************/
extern void olm_init(olm_t *olm, const struct ol_desc *ol_list);

/** This function initializes the Offload Manager.
 *
 * @param olm        : The pointer to the olm structure @ref olm_t.
 * @param whd        : The pointer to whd
 * @param ip         : The pointer to ip
 *
 *******************************************************************************/
extern int olm_init_ols(olm_t *olm, void *whd, void *ip);

/** Offload Manager de-initialization function.
 *
 * Call the de-initialization function for each offload.
 *
 * @param olm        : The pointer to the olm structure @ref olm_t.
 *
 *******************************************************************************/
extern void olm_deinit_ols(olm_t *olm);

/** \} */


#if defined(MBED_CONF_APP_OLM_TEST)

/* Offload Assistant Log Level enums */
typedef enum
{
    LOG_OLA_OLM = 0,
    LOG_OLA_ARP,
    LOG_OLA_PF,

    /* add new Offload Log Assist components here */

    LOG_OLA_MAX_INDEX
} LOG_OFFLOAD_ASSIST_T;

/* Offload logging error levels */
typedef enum
{
    LOG_OLA_LVL_OFF = 0,
    LOG_OLA_LVL_ERR,
    LOG_OLA_LVL_WARNING,
    LOG_OLA_LVL_NOTICE,
    LOG_OLA_LVL_INFO,
    LOG_OLA_LVL_DEBUG,

    LOG_OLA_LVL_MAX_INDEX   /* not for use */

} LOG_OFFLOAD_ASSIST_LEVEL_T;

/*
 * OL_LOG Debug verbosity settings
 */
/*******************************************************************************
* This function gets log level for a facility
*
* @param assist : Offload Assistant type @ref LOG_OFFLOAD_ASSIST_T
*
* @return       : currently set log level
*               : -1 BAD ARGS
*
*******************************************************************************/
extern int ol_log_get_level(LOG_OFFLOAD_ASSIST_T assist);

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
extern int ol_log_set_level(LOG_OFFLOAD_ASSIST_T assist, LOG_OFFLOAD_ASSIST_LEVEL_T level);

/** This function prints logging if levels are correct
 *
 * @param assist : Offload Assistant type @ref LOG_OFFLOAD_ASSIST_T
 * @param level  : Offload assist logging level @ref LOG_OFFLOAD_ASSIST_LEVEL_T
 * @param fmt    : Format string like printf()
 * @param ...    : variable argument list for printf
 *
 * @return       : # characters printed
 *
 *******************************************************************************/
extern int ol_logging(LOG_OFFLOAD_ASSIST_T assist, LOG_OFFLOAD_ASSIST_LEVEL_T level, const char *fmt, ...);

/**< Offload Logging Macros */
#define OL_LOG(...) ol_logging(__VA_ARGS__)                     /**< ex: OL_LOG(LOG_OLA_ARP, LOG_OLA_LVL_NOTICE, "ARP OL IP: %s\n", ip_string); */

#define OL_LOG_OLM(...) ol_logging(LOG_OLA_OLM, __VA_ARGS__)    /**< ex: OL_LOG_ARP(LOG_OLA_LVL_NOTICE, "ARP OL IP: %s\n", ip_string); */
#define OL_LOG_ARP(...) ol_logging(LOG_OLA_ARP, __VA_ARGS__)    /**< ex: OL_LOG_ARP(LOG_OLA_LVL_NOTICE, "ARP OL IP: %s\n", ip_string); */
#define OL_LOG_PF(...) ol_logging(LOG_OLA_PF, __VA_ARGS__)      /**< ex: OL_LOG_PF(LOG_OLA_LVL_ERR, "PF: Bad Args\n"); */

#else   /* next section is NOT defined(MBED_CONF_APP_OLM_TEST) */

#define ol_log_get_level(assist)            - 1
#define ol_log_set_level(assist, level)     - 1

/* No log printing when we are not building for TEST_CONSOLE */
#define OL_LOG(...)
#define OL_LOG_OLM(...)
#define OL_LOG_ARP(...)
#define OL_LOG_PF(...)

#endif  /* defined(MBED_CONF_APP_OLM_TEST) */

#ifdef __cplusplus
}
#endif

#endif /* !OLM_H__ */

