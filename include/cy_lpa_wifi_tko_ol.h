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
* @file cy_lpa_wifi_pf_ol.h
* @brief Defines the API into PktFilter offloads from personality.
*/

#ifndef PF_TKO_H__
#define PF_TKO_H__  (1)

#include <stdint.h>
#include "cy_lpa_compat.h"
#include "cy_lpa_wifi_ol_common.h"
#include "whd.h"
#include "whd_wlioctl.h"
#define MAX_TKO MAX_TKO_CONN

#ifdef __cplusplus
extern "C" {
#endif


/** User uses configurator to set these */
typedef struct cy_tko_connect
{
    uint16_t    local_port;         /**< Local port num for this socket */
    uint16_t    remote_port;        /**< Remote port num for this socket */
    char        remote_ip[16];      /**< IP address of remote side */
} cy_tko_ol_connect_t;

/** User uses configurator to set these */
typedef struct cy_tko_cfg
{
    uint16_t interval;                  /**< Interval (in seconds) between keepalives */
    uint16_t retry_interval;            /**< If a keepalive is not Acked, retry after this many seconds */
    uint16_t retry_count;               /**< Retry up to this many times */
    cy_tko_ol_connect_t ports[MAX_TKO]; /**< Port and IP address ofr each connection */
} cy_tko_ol_cfg_t;

/** Keep pointers to config space, system handle, etc */
typedef struct tko
{
    cy_tko_ol_cfg_t  *cfg;          /**< Pointer to config space */
    void             *whd;          /**< Pointer to system handle */
    ol_info_t        *ol_info_ptr;  /**< Offload Manager Info structure  \ref ol_info_t */
} tko_ol_t;


extern const ol_fns_t tko_ol_fns;

/** @addtogroup lpautilities LPA Utilities API
 * \{
 *
 *
 */

/**
 * Configure the user provided TCP keep alive parameters to LPA middleware.
 *
 * @param[in] remote_ip   Pointer to the destination IP address.
 * @param[in] remote_port Destination port number.
 * @param[in] local_port  Source port number.
 * @param[in] cfg         Structure containing user provided parameters.
 *
 * @return 0 on success; negative on failure.
 *
 */
int cylpa_tko_ol_update_config(const char *remote_ip, uint16_t remote_port, uint16_t local_port, cy_tko_ol_cfg_t *cfg );
/** \} */
#ifdef __cplusplus
}
#endif

#endif /* !TKO_OL_H__ */

