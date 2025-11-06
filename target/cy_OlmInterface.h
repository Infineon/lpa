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
 * 
 */

/**
* @file cy_OlmInterface.h
* @brief Offload Manager Interface
*/

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

