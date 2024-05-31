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
* @file cy_lpa_wifi_wowlpf_ol.c
* @brief WOWLPF Low Power Offload Assist Implementation
*/


#include <stdlib.h>
#include "string.h"
#include "cycfg.h"
#include "cy_lpa_wifi_ol_debug.h"
#include "cy_lpa_wifi_ol.h"
#include "cy_lpa_wifi_ol_priv.h"
#include "cy_lpa_wifi_wowlpf_ol.h"
#include "cy_lpa_wifi_result.h"

#include "whd_sdpcm.h"
#include "whd_wifi_api.h"
#include "whd_wlioctl.h"
#include "whd_endian.h"

#ifdef CYCFG_WOWL_MAGIC_PKT_ENABLED

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
* Static Declarations
*******************************************************************************/
static uint8_t pattern_array[MAX_PATTERN_LEN];
static uint8_t mask_array[MAX_MASK_LEN];

static wowlpf_ol_t cylpa_wowl_ol;
static ol_init_t cylpa_wowlpf_ol_init;
static ol_deinit_t cylpa_wowlpf_ol_deinit;
static ol_pm_t cylpa_wowlpf_ol_pm;

const ol_fns_t wowlpf_ol_fns =
{
    .init = cylpa_wowlpf_ol_init,
    .deinit = cylpa_wowlpf_ol_deinit,
    .pm = cylpa_wowlpf_ol_pm,
};

/*******************************************************************************
* Function Prototypes
*******************************************************************************/
static int cylpa_set_wowl_pattern( whd_t *whd, bool mode, uint8_t* patt, uint16_t pattern_offset, uint8_t *mask );
static whd_result_t whd_wowl_set_pattern( whd_t *whd, uint8_t* pattern, uint16_t pattern_offset, uint8_t* mask, bool set_pattern );

extern whd_result_t whd_set_wowl_pattern(whd_interface_t ifp, char* opt, uint32_t offset, uint8_t mask_size,
                   uint8_t * mask, uint8_t pattern_size, uint8_t * pattern, uint8_t rule);
extern whd_result_t whd_get_wowl_cap(whd_interface_t ifp, uint32_t *value);
extern whd_result_t whd_set_wowl_cap(whd_interface_t ifp, uint32_t value);
extern whd_result_t whd_wowl_clear(whd_interface_t ifp);
extern whd_result_t whd_wowl_activate(whd_interface_t ifp, uint32_t value);
#ifdef DEBUG
extern whd_result_t whd_get_wowl_pattern(whd_interface_t ifp,uint32_t pattern_num,  wl_wowl_pattern_t* pattern);
#endif
/*******************************************************************************
 * Function Name: cylpa_wowlpf_ol_init
 ******************************************************************************
 *
 * Wake On WLAN Packet filter init function.
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
static int cylpa_wowlpf_ol_init(void *ol, ol_info_t *info, const void *cfg)
{
    wowlpf_ol_t *ctxt = (wowlpf_ol_t *)ol;

    OL_LOG_WOWLPF( LOG_OLA_LVL_DEBUG, "%s\n", __func__ );
    memset(ctxt, 0, sizeof(wowlpf_ol_t) );

    if ( cfg == NULL || info == NULL )
    {
        OL_LOG_WOWLPF( LOG_OLA_LVL_ERR, "WOWL offload not configured!!\n" );
        return RESULT_BADARGS;
    }

    ctxt->cfg  = (cy_wowlpf_ol_cfg_t *)cfg;
    ctxt->whd  = info->whd;
    ctxt->ol_info_ptr = info;

    if ( ctxt->ol_info_ptr->fw_new_infra == 0 )
    {
        OL_LOG_WOWLPF(LOG_OLA_LVL_INFO, "WOWL offload is not supported\n");
        return RESULT_OK;
    }

    memcpy( &cylpa_wowl_ol, ctxt, sizeof( wowlpf_ol_t ) );

    return RESULT_OK;
}

/*******************************************************************************
 * Function Name: cylpa_wowlpf_ol_deinit
 *******************************************************************************
 *
 * Remove all wowl filters.
 *
 * \param ol
 * The pointer to the ol structure.
 *
 ********************************************************************************/
static void cylpa_wowlpf_ol_deinit(void *ol)
{
    wowlpf_ol_t *ctxt = (wowlpf_ol_t *)ol;
    whd_result_t result;

    OL_LOG_WOWLPF( LOG_OLA_LVL_DEBUG, "%s\n", __func__ );

    if ( ( ctxt == NULL ) || ( ctxt->whd == NULL ) )
    {
        return;
    }

    if ( ctxt->ol_info_ptr->fw_new_infra == 0 )
    {
        OL_LOG_WOWLPF(LOG_OLA_LVL_INFO, "NULL offload is not supported\n");
        return;
    }

    result = whd_wowl_clear( (whd_t *)ctxt->whd );
    if ( result != WHD_SUCCESS )
    {
        OL_LOG_WOWLPF(LOG_OLA_LVL_ERR, "wowl clear failed. result = %d\n", result);
    }

    memset( &cylpa_wowl_ol, 0, sizeof( wowlpf_ol_t ) );
}

/*******************************************************************************
 * Function Name: cylpa_wowlpf_ol_pm
 *******************************************************************************
 *
 * Add or remove all the filters based on state.
 *
 * \param ol
 * The pointer to the ol structure.
 *
 * \param st
 * see \ref ol_pm_st_t.
 *
 ********************************************************************************/
static void cylpa_wowlpf_ol_pm(void *ol, ol_pm_st_t st)
{
    whd_result_t result;
    wowlpf_ol_t *ctxt = (wowlpf_ol_t *)ol;
    cy_wowlpf_ol_cfg_t *wowlpf_cfg;
    uint32_t set_wowl = 0;

    OL_LOG_WOWLPF( LOG_OLA_LVL_DEBUG, "%s\n", __func__ );

    if ((ctxt == NULL) || (ctxt->whd == NULL))
    {
        OL_LOG_WOWLPF(LOG_OLA_LVL_ERR, "%s : Bad Args!\n", __func__);
        return;
    }

    if ( ctxt->ol_info_ptr->fw_new_infra == 0 )
    {
        OL_LOG_WOWLPF(LOG_OLA_LVL_INFO, "WOWL offload is not supported\n");
        return;
    }

    if (st == OL_PM_ST_GOING_TO_SLEEP)
    {
        /* Sleeping case */
        OL_LOG_WOWLPF( LOG_OLA_LVL_INFO, "WOWLPF: Enter Sleep\n" );

#ifdef CYCFG_WOWL_MAGIC_PKT_ENABLED
        OL_LOG_WOWLPF(LOG_OLA_LVL_DEBUG, "Magic pattern configured by user\n");
        set_wowl |= WL_WOWL_MAGIC;
#endif

        /* Configure net pattern if it is configured */
        if (ctxt->cfg->feature == CY_WOWLPF_OL_FEAT_WAKE)
        {
            OL_LOG_WOWLPF(LOG_OLA_LVL_DEBUG, "Net pattern configured by user\n");
            set_wowl |= WL_WOWL_NET;
        }

        result = whd_configure_wowl( ctxt->whd, set_wowl );
        OL_LOG_WOWLPF(LOG_OLA_LVL_DEBUG, "whd_wifi_offload_enable result = %l\n", result);
        if ( result != WHD_SUCCESS )
        {
            OL_LOG_WOWLPF(LOG_OLA_LVL_ERR, "whd_configure_wowl failed. result = %d\n", __func__, result);
            return;
        }

        for (wowlpf_cfg = ctxt->cfg; wowlpf_cfg->feature != CY_WOWLPF_OL_FEAT_LAST; wowlpf_cfg++)
        {
            OL_LOG_WOWLPF(LOG_OLA_LVL_DEBUG, "%s: Enabling wowl filter %d\n", __func__, wowlpf_cfg->id);
            if ( cylpa_set_wowl_pattern( ctxt->whd, true, (uint8_t *)wowlpf_cfg->pattern, wowlpf_cfg->offset, (uint8_t *)wowlpf_cfg->mask ) != WHD_SUCCESS )
            {
                OL_LOG_WOWLPF(LOG_OLA_LVL_ERR, "%s: whd_pf_enable_packet_filter %d FAILED\n", __func__,
                        wowlpf_cfg->id);
            }
        }

        result = whd_wowl_activate( ctxt->whd, 1 );
        OL_LOG_WOWLPF(LOG_OLA_LVL_DEBUG, "whd_wowl_activate result = %l\n", result);
        if ( result != WHD_SUCCESS )
        {
            /* Remove pattern set in to the WLAN */
            cylpa_set_wowl_pattern( ctxt->whd, false, (uint8_t *)wowlpf_cfg->pattern, wowlpf_cfg->offset, (uint8_t *)wowlpf_cfg->mask );
            OL_LOG_WOWLPF(LOG_OLA_LVL_ERR, "whd_wowl_activate failed. result = %d\n", __func__, result);
            return;
        }

#ifdef OLM_LOG_ENABLED
        wl_wowl_pattern_t *getpattern = (wl_wowl_pattern_t *)malloc(sizeof(wl_wowl_pattern_t)+100);
        if( whd_get_wowl_pattern( ctxt->whd, 2, (wl_wowl_pattern_t *)getpattern ) == WHD_SUCCESS )
        {
            OL_LOG_WOWLPF(LOG_OLA_LVL_INFO, "whd_get_wowl_pattern() success\n");
        }
        free(getpattern);
#endif
    }
    else
    {
        /* Wake case */
    	OL_LOG_WOWLPF( LOG_OLA_LVL_DEBUG, "%s: Wakeup and disable WOWLPF\n", __func__);

        result = whd_wowl_activate( ctxt->whd, 0 );
        if ( result != WHD_SUCCESS )
        {
            OL_LOG_WOWLPF(LOG_OLA_LVL_ERR, "whd_wowl_activate failed. result = %d\n", result);
        }

        for (wowlpf_cfg = ctxt->cfg; wowlpf_cfg->feature != CY_WOWLPF_OL_FEAT_LAST; wowlpf_cfg++)
        {
            OL_LOG_WOWLPF(LOG_OLA_LVL_DEBUG, "%s: Enabling wowl filter %d\n", __func__, wowlpf_cfg->id);
            if ( cylpa_set_wowl_pattern( ctxt->whd, false, (uint8_t *)wowlpf_cfg->pattern, wowlpf_cfg->offset, (uint8_t *)wowlpf_cfg->mask ) != WHD_SUCCESS )
            {
                OL_LOG_WOWLPF(LOG_OLA_LVL_ERR, "%s: whd_pf_enable_packet_filter %d FAILED\n", __func__,
                        wowlpf_cfg->id);
            }
        }

        result = whd_wowl_clear( ctxt->whd );

    }  /* for each configuration */
    return;
}

static int cylpa_set_wowl_pattern( whd_t *whd, bool mode, uint8_t* pattern, uint16_t pattern_offset, uint8_t* mask )
{
    whd_result_t result = WHD_SUCCESS;
    uint32_t wowl;

    /* If pattern present, then only set it to WLAN */
    if( pattern != NULL )
    {
        if  (mode)
        {
           whd_get_wowl_cap( whd, &wowl );

           wowl &= WL_WOWL_NET;
           if( wowl != WL_WOWL_NET )
           {
               wowl |= WL_WOWL_NET;
               result = whd_set_wowl_cap( whd, wowl );
            }
            result |= whd_wowl_set_pattern( whd, pattern, pattern_offset, mask, 1 );
        }
        else
        {
            result = whd_wowl_set_pattern( whd, pattern, pattern_offset, mask, 0 );
        }
    }

    return result;
}

static int lower(int c)
{
    if (c >= 'A' && c <= 'Z')
    {
        return c +'a'-'A';
    }
    else
    {
        return c;
    }
}
static int hexCharToInt(char c)
{
    if ((c >= '0' && c <= '9') ? 1 : 0)
    {
        return c - '0';
    }
    else
    {
        return lower(c) - 'a' + 10;
    }
}

whd_result_t
whd_wowl_set_pattern( whd_t *whd, uint8_t* pattern, uint16_t pattern_offset, uint8_t *mask, bool set_pattern )
{
    whd_result_t result = WHD_SUCCESS;
    uint8_t pattern_size;
    uint8_t mask_size;
    char *mode;

    OL_LOG_WOWLPF(LOG_OLA_LVL_INFO, "pattern : %s\n", pattern);

    /* Pattern conversion */
    /* Calculate the length of the pattern. Exclude "0x" from the pattern */
    pattern_size = (strlen((const char *)pattern)/2) - 1;


    /* Convert each pair of hexadecimal characters to an unsigned char */
    for (int i = 0, j = 1; i < pattern_size; ++i, ++j)
    {
        pattern_array[i] = (hexCharToInt(pattern[2 * j]) << 4) | hexCharToInt(pattern[2 * j + 1]);
    }

    OL_LOG_WOWLPF(LOG_OLA_LVL_INFO, "\nmask : %s\n", mask);
    /* mask conversion */
    if ( (strlen((const char *)mask) % 2) != 0 )
    {
        /* mask size should be in even numbers */
        OL_LOG_WOWLPF(LOG_OLA_LVL_ERR, "invalid mask_size : %d\n", (mask_size % 2));
        return 1;
    }

    /* Calculate the length of the mask. Exclude "0x" from the mask */
    mask_size = (strlen((const char *)mask)/2) - 1;

    /* Convert each pair of hexadecimal characters to an unsigned char */
    for (int i = 0, j = 1; i < mask_size; ++i, ++j)
    {
        mask_array[i] = (hexCharToInt(mask[2 * j]) << 4) | hexCharToInt(mask[2 * j + 1]);

        /* Reverse bits for each byte in the mask */
        for (int k = 0; k < 4; k++)
        {
            if (((mask_array[i] >> k) & 1) != ((mask_array[i] >> (7 - k)) & 1))
            {
                /* Toggle bit k */
                mask_array[i] ^= (1 << k);
                /* Toggle bit (7 - k) */
                mask_array[i] ^= (1 << (7 - k));
            }
        }
    }

#ifdef OLM_LOG_ENABLED
    OL_LOG_WOWLPF(LOG_OLA_LVL_INFO, "\nConverted pattern array: \n");
    for (int i = 0; i < pattern_size; ++i)
    {
        OL_LOG_WOWLPF(LOG_OLA_LVL_DEBUG, "%02x ", pattern_array[i]);
    }

    OL_LOG_WOWLPF(LOG_OLA_LVL_INFO, "\nConverted mask array: \n");
    for (int i = 0; i < mask_size; ++i)
    {
        OL_LOG_WOWLPF(LOG_OLA_LVL_DEBUG, "%02x ", mask_array[i]);
    }
#endif

    if( set_pattern )
    {
        mode = "add";
    }
    else
    {
        mode = "del";
    }

    result = whd_set_wowl_pattern( whd, mode, pattern_offset, mask_size, mask_array, pattern_size, pattern_array, WOWL_PATTERN_TYPE_BITMAP);
    if ( result !=  WHD_SUCCESS)
    {
        OL_LOG_WOWLPF(LOG_OLA_LVL_ERR, "%s: add pattern failed \n", __func__);
        return 1;
    }
    OL_LOG_WOWLPF(LOG_OLA_LVL_INFO, "whd_set_wowl_pattern:%d \n",result);

    return result;
}

#ifdef __cplusplus
}
#endif

#endif //CYCFG_WOWL_MAGIC_PKT_ENABLED
