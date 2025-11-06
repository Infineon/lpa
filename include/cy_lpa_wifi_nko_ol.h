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
* @file   : cy_lpa_wifi_nko_ol.h
* @brief  : Defines the API into NAT Keepalive offloads from personality.
*/

#ifndef PF_NKO_H__
#define PF_NKO_H__  (1)

#include <stdint.h>
#include "cy_lpa_compat.h"
#include "cy_lpa_wifi_ol_common.h"
#include "whd.h"
#include "whd_wlioctl.h"


#ifdef __cplusplus
extern "C" {
#endif

/* NAT server address len */
#ifndef CYLPA_NAT_SERVER_NAME_LEN
#define CYLPA_NAT_SERVER_NAME_LEN                         (127)
#endif

/* Max NAT Keep alive payload length */
#ifndef CYLPA_NAT_KEEPALIVE_MAX_PAYLOAD
#define CYLPA_NAT_KEEPALIVE_MAX_PAYLOAD                   (127)
#endif

/** User uses configurator to set these */
typedef struct cy_lpa_nko_cfg
{
    uint32_t interval;                                  /**< NAT KeepAlive interval in milliseconds     */
    char     server[CYLPA_NAT_SERVER_NAME_LEN+1];       /**< Server Name / IP address                   */
    uint16_t d_port;                                    /**< UDP Destination Port                       */
    uint16_t s_port;                                    /**< UDP Source Port                            */
    char     payload[CYLPA_NAT_KEEPALIVE_MAX_PAYLOAD+1];/**< Keepalive Payload                          */
} cy_lpa_nko_ol_cfg_t;

/** Keep pointers to config space, system handle, etc */
typedef struct nko_ol
{
    cy_lpa_nko_ol_cfg_t  *cfg;                          /**< Pointer to config space                        */
    void                 *whd;                          /**< Pointer to system handle                       */
    ol_info_t            *ol_info_ptr;                  /**< Offload Manager Info structure  \ref ol_info_t */
} nko_ol_t;


extern const ol_fns_t nko_ol_fns;

/*********************************************************************************************
*
*  Functions
*
*********************************************************************************************/
int32_t cylpa_nko_ol_update_config(cy_lpa_nko_ol_cfg_t *cfg);

#ifdef __cplusplus
}
#endif

#endif /* !TKO_OL_H__ */

