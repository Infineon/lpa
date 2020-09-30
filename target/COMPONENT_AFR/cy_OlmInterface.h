/***************************************************************************//**
* \file OlmInterface.c
* \version 1.0
*
* \brief
* Offload Manager Interface
*
********************************************************************************
* \copyright
* Copyright 2020, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#include "cy_result.h"
#include "cy_lpa_wifi_ol.h"
#include "cy_lpa_wifi_ol_priv.h"

/*********************************************************************************************
*
*  Functions
*
*********************************************************************************************/

/** Get default offload list (Weak function: This is called if Configurator doesnot have offloads)
 *
 * @param[out]   ol_desc        :pointer to Offload Descriptor
 *
 * @return whd_result_t
 */
extern const struct ol_desc *get_default_ol_list();

/*********************************************************************************************
*
*  Functions
*
*********************************************************************************************/

/** Offload Manager Configuration 
 *
 * @param[in]    ifp            : interface to whd
 * @param[in]    oflds_list     : Pointer to offload list
 * @param[out]   cy_rslt_t      : Return Success or Failure based on offload configuration in WLAN
 *                                Firmware
 *
 * @return cy_rslt_t
 */
extern cy_rslt_t cy_olm_create(void *ifp, ol_desc_t *oflds_list);

/** Offload Manager init 
 *
 * @param[in]    olm            : OLM instance pointer
 * @param[in]    whd            : interface to whd
 * @param[int]   ip             : The pointer to ip
 *
 * @return cy_rslt_t
 */
extern cy_rslt_t cy_olm_init_ols(olm_t *olm, void *whd, void *ip);

/** Offload Manager deinit
 *
 * @param[in]    olm            : OLM instance pointer
 */
void cy_olm_deinit_ols(olm_t *olm);

/** Offload Manager Configuration 
 *
 * @param[in]    olm            : OLM instance pointer
 * @param[out]   st             : Offload host power-mode status 
 *
 */
extern void cy_olm_pm_notification_callback(olm_t *olm, ol_pm_st_t st);

#ifdef __cplusplus
}
#endif

