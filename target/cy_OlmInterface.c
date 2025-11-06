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
* @file cy_OlmInterface.c
* @brief Offload Manager Interface
*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "cy_OlmInterface.h"
#include "cy_lpa_compat.h"
#include "cy_lpa_wifi_ol.h"
#include "cy_lpa_wifi_ol_common.h"
#include "cy_lpa_wifi_pf_ol.h"
#include "cy_lpa_wifi_olm.h"
#include "cy_lpa_wifi_arp_ol.h"
#include "cy_result_mw.h"
#include "network_activity_handler.h"

olm_t cy_olm;
whd_interface_t cylpa_iface = NULL;

CYPRESS_WEAK const struct ol_desc *cycfg_get_default_ol_list()
{
    return NULL;
}

CYPRESS_WEAK const struct ol_desc *get_default_ol_list()
{
    return cycfg_get_default_ol_list();
}


void *cy_get_olm_instance()
{
    return &cy_olm;
}

cy_rslt_t cy_olm_create(void *ifp, ol_desc_t *oflds_list)
{
    ol_desc_t *olm_desc;
    cy_rslt_t result = CY_RSLT_SUCCESS;

    // Get Offload configuration from device configurator
    olm_desc = (ol_desc_t *)get_default_ol_list();
    if (olm_desc == NULL)
    {
        printf("Offloads not configured \n");
		olm_desc = oflds_list;
    }

    /* Offload Manager init */
    cy_olm.ol_info.whd = ifp;
    cylpa_iface = ifp;
    cylpa_olm_init(&cy_olm, olm_desc);

    return result;
}

cy_rslt_t cy_olm_init_ols(olm_t *olm, void *whd, void *ip)
{
    return cylpa_olm_init_ols(olm, whd, ip);
}

void cy_olm_deinit_ols(olm_t *olm)
{
    cylpa_olm_deinit_ols(olm);
}

void cy_olm_pm_notification_callback(olm_t *olm, ol_pm_st_t st)
{
    cylpa_olm_dispatch_pm_notification(olm, st);
}

whd_interface_t cy_olm_get_whd_interface ( void )
{
	return cylpa_iface;
}
