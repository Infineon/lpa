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
* @file cy_lpa_wifi_tko_ol.c
* @brief Low Power Offload TCP keep-alive Assist Implementation
*/

#include "string.h"
#include "cy_lpa_wifi_ol_debug.h"
#include "cy_lpa_wifi_ol.h"
#include "cy_lpa_wifi_ol_priv.h"
#include "cy_lpa_wifi_tko_ol.h"
#include "cy_lpa_wifi_result.h"

#ifdef COMPONENT_LWIP
#include "lwip/ip.h"
#endif
#include "whd_sdpcm.h"
#include "cy_whd_tko_api.h"
#include "whd_wifi_api.h"
#include "whd_wlioctl.h"
#include "whd_endian.h"
#include "whd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

static ol_init_t cylpa_tko_ol_init;
static ol_deinit_t cylpa_tko_ol_deinit;
static ol_pm_t cylpa_tko_ol_pm;

const ol_fns_t tko_ol_fns =
{
    .init = cylpa_tko_ol_init,
    .deinit = cylpa_tko_ol_deinit,
    .pm = cylpa_tko_ol_pm,
};


/*******************************************************************************
 * Global Declarations
*******************************************************************************/
cy_tko_ol_cfg_t cy_tko_ol_cfg = {

   .interval = 0,
   .retry_interval = 0,
   .retry_count = 0,
   {
       {
          .local_port = 0,
          .remote_port = 0,
          "0.0.0.0"
       }
   }
};

/* cy_tko_ol_cfg index points to an index to the TCP connection parameters
 * upto maximum of MAX_TKO TCP connections supported.
 */
uint8_t cy_tko_ol_cfg_index = 0;

/*******************************************************************************
* Function Prototypes
*******************************************************************************/


/*******************************************************************************
 * Function Name: cylpa_tko_ol_init
 ****************************************************************************//**
 *
 * TCP Keepalive offload filter init function.
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
static int cylpa_tko_ol_init(void *ol, ol_info_t *info, const void *cfg)
{
    tko_ol_t *ctxt = (tko_ol_t *)ol;
    cy_tko_ol_cfg_t *tko_cfg;
    int i;
    whd_result_t result;
    uint8_t max;
    whd_tko_retry_t retry;

    OL_LOG_TKO(LOG_OLA_LVL_DEBUG, "%s\n", __func__);

    memset(ctxt, 0, sizeof(tko_ol_t) );

    if ( cfg == NULL )
    {
        OL_LOG_TKO(LOG_OLA_LVL_ERR, "TCP Keep Alive offload not configured!!\n", __func__);
        return RESULT_OK;
    }

    ctxt->ol_info_ptr = info;
    ctxt->cfg = (cy_tko_ol_cfg_t *)cfg;
    tko_cfg = &cy_tko_ol_cfg;
    ctxt->whd  = info->whd;

    memcpy(&cy_tko_ol_cfg, ctxt->cfg, sizeof(cy_tko_ol_cfg));

    for (i = 0; i < MAX_TKO; i++)
    {
        OL_LOG_TKO(LOG_OLA_LVL_DEBUG, "%d local %d, ", i, tko_cfg->ports[i].local_port);
        OL_LOG_TKO(LOG_OLA_LVL_DEBUG, "   remote %d, ", tko_cfg->ports[i].remote_port);
        OL_LOG_TKO(LOG_OLA_LVL_DEBUG, "   IP %s\n", tko_cfg->ports[i].remote_ip);
    }

    result = whd_tko_max_assoc(ctxt->whd, &max);
    if (result != WHD_SUCCESS)
    {
        OL_LOG_TKO(LOG_OLA_LVL_ERR, "tko_max_assoc returned failure\n");
        return RESULT_OK;
    }
    OL_LOG_TKO(LOG_OLA_LVL_INFO, "Max connection: %d\n", max);
    if (max != MAX_TKO)
    {
        OL_LOG_TKO(LOG_OLA_LVL_ERR, "%s: Runtime max connections (%d) != precompiled connections (%d)\n",
                   __func__, max, MAX_TKO);
    }

    /* Do not configure TKO parameters for new infra */
    if( ctxt->ol_info_ptr->fw_new_infra == 0 )
    {
        retry.tko_interval = tko_cfg->interval;
        retry.tko_retry_interval = tko_cfg->retry_interval;
        retry.tko_retry_count = tko_cfg->retry_count;

        /* Set params */
        result = whd_tko_param(ctxt->whd, &retry, 1);
        if (result != WHD_SUCCESS)
        {
            OL_LOG_TKO(LOG_OLA_LVL_ERR, "Set whd_tko_param returned failure\n");
            return RESULT_OK;
        }

        memset(&retry, 0, sizeof(retry) );
        /* Get params */
        result = whd_tko_param(ctxt->whd, &retry, 0);
        if (result != WHD_SUCCESS)
        {
            OL_LOG_TKO(LOG_OLA_LVL_ERR, "Get whd_tko_param returned failure\n");
            return RESULT_OK;
        }
    }

    return RESULT_OK;
}

/*******************************************************************************
 * Function Name: cylpa_tko_ol_deinit
 ****************************************************************************//**
 *
 * Remove TCP keep-alive offload filters. No need to disable prior to removal.
 *
 * \param ol
 * The pointer to the ol structure.
 *
 ********************************************************************************/
static void cylpa_tko_ol_deinit(void *ol)
{
    tko_ol_t *ctxt = (tko_ol_t *)ol;
    if ((ctxt == NULL) || (ctxt->whd == NULL))
    {
        return;
    }

    whd_tko_disable(ctxt->whd);
}

#define print_ip4(ipaddr)  \
    printf("%lu.%lu.%lu.%lu\n", (ipaddr) & 0xff, (ipaddr) >> 8 & 0xff, (ipaddr) >> 16 & 0xff, (ipaddr) >> 24 & 0xff)

/*******************************************************************************
 * Function Name: cylpa_tko_ol_pm
 ****************************************************************************//**
 *
 * TCP keep-alive offload Power Management notification callback
 *
 * \param ol
 * The pointer to the ol structure.
 *
 * \param st
 * see \ref ol_pm_st_t.
 *
 ********************************************************************************/
static void cylpa_tko_ol_pm(void *ol, ol_pm_st_t st)
{
    tko_ol_t *ctxt = (tko_ol_t *)ol;
    cy_tko_ol_cfg_t *tko_cfg = &cy_tko_ol_cfg;
    whd_result_t result;
    int found = 0;

    if ((ctxt == NULL) || (ctxt->whd == NULL))
    {
        OL_LOG_TKO(LOG_OLA_LVL_ERR, "%s : Bad Args!\n", __func__);
        return;
    }

    if (st == OL_PM_ST_GOING_TO_SLEEP)
    {
        /* Sleeping case */
        OL_LOG_TKO(LOG_OLA_LVL_INFO, "TKO: Enter Sleep\n");

        /* Do not configure parameters to WHD for new infra */
        if( ctxt->ol_info_ptr->fw_new_infra == 0)
        {
            for (int i = 0; i < MAX_TKO; i++)
            {
                result = whd_tko_activate(ctxt->whd, i, tko_cfg->ports[i].local_port, tko_cfg->ports[i].remote_port,
                                          tko_cfg->ports[i].remote_ip);
                if (result != WHD_SUCCESS)
                {
                    OL_LOG_TKO(LOG_OLA_LVL_ERR, "%s: No such connection: %s local %d remote %d\n", __func__,
                               tko_cfg->ports[i].remote_ip, tko_cfg->ports[i].local_port, tko_cfg->ports[i].remote_port);
                }
                else
                {
                    found++;
                    OL_LOG_TKO(LOG_OLA_LVL_WARNING, "%s: Activated: %s local %d remote %d\n", __func__,
                               tko_cfg->ports[i].remote_ip, tko_cfg->ports[i].local_port, tko_cfg->ports[i].remote_port);
                }
            }
        }
        else
        {
            found = 1;
        }

        if (!found)
        {
            OL_LOG_TKO(LOG_OLA_LVL_ERR, "TKO PM: No connections to enable.\n");
        }
        else
        {
            result = whd_tko_enable(ctxt->whd);
            if (result != WHD_SUCCESS)
            {
                OL_LOG_TKO(LOG_OLA_LVL_ERR, "%s: whd_tko_enable returns failure\n", __func__);
            }
            else
            {
                OL_LOG_TKO(LOG_OLA_LVL_DEBUG, "%s: Sleep with TKO_ENABLED\n", __func__);
            }
        }
    }
    else
    {
        /* Waking case */
        if (1)
        {
            OL_LOG_TKO(LOG_OLA_LVL_DEBUG, "%s: Wakeup and disable TKO\n", __func__);
            result = whd_tko_disable(ctxt->whd);
            if (result != WHD_SUCCESS)
            {
                OL_LOG_TKO(LOG_OLA_LVL_ERR, "%s: whd_tko_disable returned failure\n", __func__);
            }
            else
            {
                OL_LOG_TKO(LOG_OLA_LVL_INFO, "%s: TKO_DISABLE success\n", __func__);
            }
        }
        else
        {
            OL_LOG_TKO(LOG_OLA_LVL_ERR, "Leaving Enabled\n");
        }
    }
}

/*******************************************************************************
 * Function Name: cylpa_tko_ol_update_config
 ****************************************************************************//**
 *
 * Update the TCP keep-alive offload configuration list
 *
 * \param remote_ip
 * The pointer to IP address of TCP server.
 *
 * \param remote_port
 * The destination port of TCP server
 *
 * \param local_port
 * The source port of the TCP client
 *
 * \param cfg 
 * The pointer to TCP keep-alive offload parameters. \ref cy_tko_ol_cfg_t
 *
 * \return
 * Returns the execution result
 *
 ********************************************************************************/
int cylpa_tko_ol_update_config(const char *remote_ip, uint16_t remote_port, uint16_t local_port, cy_tko_ol_cfg_t *cfg )
{
    cy_tko_ol_connect_t *tko_ol_connect_params = NULL;

    if ( ( remote_ip == NULL ) || ( remote_port == 0) || ( local_port == 0 ) || ( cfg == NULL ) )
    {
        OL_LOG_TKO(LOG_OLA_LVL_ERR, "TCP Keep Alive offload configuration is NULL!!\n", __func__);
        return RESULT_OK;
    }

    /*
     * if the API is called many times more than
     * max TCP connections then update the first index
     *
     */
    if ( cy_tko_ol_cfg_index == MAX_TKO )
    {
        cy_tko_ol_cfg_index = 0;
    }

    tko_ol_connect_params = &cy_tko_ol_cfg.ports[cy_tko_ol_cfg_index];
    memset(tko_ol_connect_params, 0, sizeof(cy_tko_ol_cfg.ports[cy_tko_ol_cfg_index]));
    memcpy(cy_tko_ol_cfg.ports[cy_tko_ol_cfg_index].remote_ip, remote_ip, strlen(remote_ip));
    cy_tko_ol_cfg.ports[cy_tko_ol_cfg_index].remote_port = remote_port;
    cy_tko_ol_cfg.ports[cy_tko_ol_cfg_index].local_port  = local_port;

    cy_tko_ol_cfg.interval       = cfg->interval;
    cy_tko_ol_cfg.retry_count    = cfg->retry_count;
    cy_tko_ol_cfg.retry_interval = cfg->retry_interval;

    OL_LOG_TKO(LOG_OLA_LVL_DEBUG, "\nUpdating...\n");
    OL_LOG_TKO(LOG_OLA_LVL_DEBUG, "%d: %s, ", cy_tko_ol_cfg_index, cy_tko_ol_cfg.ports[cy_tko_ol_cfg_index].remote_ip);
    OL_LOG_TKO(LOG_OLA_LVL_DEBUG, "Local %d, ", cy_tko_ol_cfg.ports[cy_tko_ol_cfg_index].local_port);
    OL_LOG_TKO(LOG_OLA_LVL_DEBUG, "Remote %d\n", cy_tko_ol_cfg.ports[cy_tko_ol_cfg_index].remote_port);
    OL_LOG_TKO(LOG_OLA_LVL_DEBUG, "Interval:%d Retry_interval:%d Retry_count:%d\n",
               cy_tko_ol_cfg.interval, cy_tko_ol_cfg.retry_interval, cy_tko_ol_cfg.retry_count );
    OL_LOG_TKO(LOG_OLA_LVL_DEBUG, "Done \n\n");

    cy_tko_ol_cfg_index++;

    return RESULT_OK;
}

#ifdef __cplusplus
}
#endif

