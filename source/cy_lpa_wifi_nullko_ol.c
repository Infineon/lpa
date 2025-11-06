/*
 * (c) 2025, Infineon Technologies AG, or an affiliate of Infineon
 * Technologies AG. All rights reserved.
 * This software, associated documentation and materials ("Software") is
 * owned by Infineon Technologies AG or one of its affiliates ("Infineon")
 * and is protected by and subject to worldwide patent protection, worldwide
 * copyright laws, and international treaty provisions. Therefore, you may use
 * this Software only as provided in the license agreement accompanying the
 * software package from which you obtained this Software. If no license
 * agreement applies, then any use, reproduction, modification, translation, or
 * compilation of this Software is prohibited without the express written
 * permission of Infineon.
 * 
 * Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
 * IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
 * THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
 * SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
 * Infineon reserves the right to make changes to the Software without notice.
 * You are responsible for properly designing, programming, and testing the
 * functionality and safety of your intended application of the Software, as
 * well as complying with any legal requirements related to its use. Infineon
 * does not guarantee that the Software will be free from intrusion, data theft
 * or loss, or other breaches ("Security Breaches"), and Infineon shall have
 * no liability arising out of any Security Breaches. Unless otherwise
 * explicitly approved by Infineon, the Software may not be used in any
 * application where a failure of the Product or any consequences of the use
 * thereof can reasonably be expected to result in personal injury.
 */
 
/**
* @file  : cy_lpa_wifi_nullko_ol.c
* @brief : Handles NULL Keep Alive offload
*/

#include "string.h"
#include "cy_lpa_wifi_ol_debug.h"
#include "cy_lpa_wifi_ol.h"
#include "cy_lpa_wifi_ol_priv.h"
#include "cy_lpa_wifi_nullko_ol.h"
#include "cy_lpa_wifi_result.h"

#include "whd_wifi_api.h"
#include "whd_wlioctl.h"
#include "whd_endian.h"
#include "whd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************
*
*  Defines
*
********************************************************************/
#ifdef NULLKO_OL_DEBUG
#define NULLKO_ERROR_PRINTF(x)             printf x
#define NULLKO_DEBUG_PRINTF(x)             printf x
#else
#define NULLKO_ERROR_PRINTF(x)
#define NULLKO_DEBUG_PRINTF(x)
#endif

#define MILLISEC_TO_SEC           1000 /* multiplier for seconds to milliseconds */
/********************************************************************
 *
 *  Structures
 *
 *********************************************************************/

/*********************************************************************************************
*
*  Forward Declarations
*
*********************************************************************************************/
static ol_init_t cylpa_nullko_ol_init;
static ol_deinit_t cylpa_nullko_ol_deinit;
static ol_pm_t cylpa_nullko_ol_pm;

/*********************************************************************************************
*
*  Global Declarations
*
*********************************************************************************************/
const ol_fns_t nullko_ol_fns =
{
    .init   = cylpa_nullko_ol_init,
    .deinit = cylpa_nullko_ol_deinit,
    .pm     = cylpa_nullko_ol_pm,
};

/*******************************************************************************
 * Function Name: cylpa_nullko_ol_init
 ****************************************************************************//**
 *
 * NULL Keepalive offload init function.
 *
 * \param ol
 * The pointer to the ol structure.
 *
 * \param info
 * The pointer to the ol_info_t structure \ref ol_info_t.
 *
 * \param cfg
 * The pointer to the configuration.
 *
 * \return
 * Returns the execution result
 *
 ********************************************************************************/
static int cylpa_nullko_ol_init(void *ol, ol_info_t *info, const void *cfg)
{
    int32_t result;
    whd_keep_alive_t keep_alive;
    nullko_ol_t *ctxt = (nullko_ol_t *)ol;

    OL_LOG_NULLKA(LOG_OLA_LVL_DEBUG, "%s\n", __func__);

    memset(ctxt, 0, sizeof(nullko_ol_t));

    if ( cfg == NULL || info == NULL )
    {
        OL_LOG_NULLKA(LOG_OLA_LVL_ERR, "NULL Keep Alive offload not configured!\n");
        return RESULT_OK;
    }

    ctxt->cfg           = (cy_nullko_ol_cfg_t *)cfg;
    ctxt->whd           = info->whd;
    ctxt->ol_info_ptr   = info;

    if ( ctxt->ol_info_ptr->fw_new_infra == 0 )
    {
        OL_LOG_NULLKA(LOG_OLA_LVL_INFO, "NULL keep alive offload is not supported\n");
        return RESULT_OK;
    }

    keep_alive.period_msec = (ctxt->cfg->interval * MILLISEC_TO_SEC);
    keep_alive.data        = NULL;
    keep_alive.len_bytes   = 0;

    result = whd_wifi_keepalive_config( ctxt->whd, &keep_alive, WHD_KEEPALIVE_NULL );
    if ( WHD_SUCCESS != result )
    {
        OL_LOG_NULLKA(LOG_OLA_LVL_ERR, "NULLKO: Failed to Enable NULL Keepalive!\n");
        return result;
    }
    else
    {
        OL_LOG_NULLKA(LOG_OLA_LVL_INFO, "NULLKO: Keepalive Enabled!\n");
    }

    return RESULT_OK;
}

/*******************************************************************************
 * Function Name: cylpa_nullko_ol_deinit
 ****************************************************************************//**
 *
 * Remove NULL keep-alive offload
 *
 * \param ol
 * The pointer to the ol structure.
 *
 ********************************************************************************/
static void cylpa_nullko_ol_deinit(void *ol)
{
    nullko_ol_t *ctxt = (nullko_ol_t *)ol;

    if ( (ctxt == NULL) || (ctxt->whd == NULL) )
    {
        OL_LOG_NULLKA(LOG_OLA_LVL_ERR, "%s : Bad Args!\n", __func__);
        return;
    }

    if ( ctxt->ol_info_ptr->fw_new_infra == 0 )
    {
        OL_LOG_NULLKA(LOG_OLA_LVL_INFO, "Null Keep alive offload is not supported\n");
        return;
    }

    return;
}

/*******************************************************************************
 * Function Name: cylpa_nullko_ol_pm
 ****************************************************************************//**
 *
 * NULL keep-alive offload Power Management notification callback
 *
 * \param ol
 * The pointer to the ol structure.
 *
 * \param st
 * see \ref ol_pm_st_t.
 *
 * Note : NULL KeepAlive will be send automatically irrespective of the power state
 *        Hence this function will not do anything.
 ********************************************************************************/
static void cylpa_nullko_ol_pm(void *ol, ol_pm_st_t st)
{
    OL_LOG_NULLKA(LOG_OLA_LVL_DEBUG, "%s\n", __func__);
    return;
}

#ifdef __cplusplus
}
#endif

