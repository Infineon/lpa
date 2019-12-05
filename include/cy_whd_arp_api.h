/***************************************************************************//**
* \file cy_whd_arp_api.h
* \version 1.0
*
*
* WHD ARP Offload API.
*
********************************************************************************
* \copyright
* Copyright 2019, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#ifndef WHD_ARP_API_H__
#define WHD_ARP_API_H__  (1)

#include "cy_lpa_wifi_ol_common.h"
/*****************************************************************************/
/** @addtogroup whdarp       WHD ARP Offload Assist
 *
 * Abstraction for ARP Offload capabilities
 *
 *  @{
 */
/*****************************************************************************/

/* We have a non-hardware build that we need to handle - "OLM_NO_HARDWARE" defined in app/olm-sanity/Makefile */
#if defined(OLM_NO_HARDWARE)

#ifndef WHD_SUCCESS
typedef uint32_t whd_result_t;
#define WHD_SUCCESS         (whd_result_t)0
#define WHD_ERROR           (whd_result_t)4
#define WHD_BADARG          (whd_result_t)5
#define WHD_IOCTL_FAIL      (whd_result_t)1008
#endif

#else
/* We have hardware here */
#include "whd.h"
#include "whd_wlioctl.h"

#endif /* defined(OLM_NO_HARDWARE) */

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************************************
*
*  Enums
*
*********************************************************************************************/

/*********************************************************************************************
*
*  Structures
*
*********************************************************************************************/

/** ARP offload statistics */
typedef struct whd_arp_stats_s
{
    uint32_t version;               /**< WLAN version                                           */
    uint32_t peerage;               /**< Current peer age-out time                              */
    uint32_t arpoe;                 /**< Agent enabled state                                    */
    uint32_t features_enabled;      /**< ARP Feature Flags @ref CY_ARP_OL_AGENT_ENABLE, etc.    */

    struct arp_ol_stats_t stats;    /**< Current stats                                          */

    uint32_t host_ip_list[ARP_MULTIHOMING_MAX];     /**< host_ip addresses in one call          */
} whd_arp_stats_t;


/*********************************************************************************************
*
*  Functions
*
*********************************************************************************************/

/** Get version of Device (WLAN) Firmware
 *
 * @param[in]    whd            : whd Instance
 * @param[out]   version        :pointer to store version #
 *
 * @return whd_result_t
 */
whd_result_t whd_arp_version(whd_t *whd, uint32_t *version);

/** Get ARP Offload Peer Age from Device (WLAN)
 *    Length of time in seconds before aging out an entry in the WLAN processor ARP table.
 *
 * @param[in]    whd            : whd Instance
 * @param[out]   seconds        : pointer to store value
 *
 * @return whd_result_t
 */
whd_result_t whd_arp_peerage_get(whd_t *whd, uint32_t *seconds);

/** Set ARP Offload Peer Age in Device (WLAN)
 *    Length of time in seconds before aging out an entry in the WLAN processor ARP table.
 *
 * @param[in]    whd            : whd Instance
 * @param[in]    seconds        : Seconds to age out IP
 *
 * @return whd_result_t
 */
whd_result_t whd_arp_peerage_set(whd_t *whd, uint32_t seconds);

/** Get ARP Offload Agent Enable from Device (WLAN)
 *
 * @param[in]    whd            : whd Instance
 * @param[out]   agent_enable   : pointer to store value
 *
 * @return whd_result_t
 */
whd_result_t whd_arp_arpoe_get(whd_t *whd, uint32_t *agent_enable);

/** Set ARP Offload Agent Enable in Device (WLAN)
 *    Set Enable/Disable of ARP Offload Agent
 *
 * @param[in]    whd            : whd Instance
 * @param[in]    agent_enable   : Enable=1 / Disable=0
 *
 * @return whd_result_t
 */
whd_result_t whd_arp_arpoe_set(whd_t *whd, uint32_t agent_enable);

/** Clear ARP Offload cache in Device (WLAN)
 *
 * @param[in]    whd            : whd Instance
 *
 * @return whd_result_t
 */
whd_result_t whd_arp_cache_clear(whd_t *whd);

/** Get ARP Offload Feature Flags from Device (WLAN)
 *
 * @param[in]    whd        : whd Instance
 * @param[out]   features   : ptr to store currently set features - bit flags @ref CY_ARP_OL_AGENT_ENABLE, etc.
 *                            ARL_OL_AGENT | ARL_OL_SNOOP | ARP_OL_HOST_AUTO_REPLY | ARP_OL_PEER_AUTO_REPLY
 *
 * @return whd_result_t
 */
whd_result_t whd_arp_features_get(whd_t *whd, uint32_t *features);

/** Set ARP Offload Feature Flags in Device (WLAN)
 *
 * @param[in]    whd        : whd Instance
 * @param[in]    features   : features to set value (you can OR ('|') multiple flags) @ref CY_ARP_OL_AGENT_ENABLE, etc.
 *                           ARL_OL_AGENT | ARL_OL_SNOOP | ARP_OL_HOST_AUTO_REPLY | ARP_OL_PEER_AUTO_REPLY
 *
 * @return whd_result_t
 */
whd_result_t whd_arp_features_set(whd_t *whd, uint32_t features);

/** Print ARP Offload Feature Flags in Human readable form to console
 *
 * @param[in]    features   : feature flags to set (you can OR '|' multiple flags) @ref CY_ARP_OL_AGENT_ENABLE, etc.
 *                            ARL_OL_AGENT | ARL_OL_SNOOP | ARP_OL_HOST_AUTO_REPLY | ARP_OL_PEER_AUTO_REPLY
 * @param[in]    title      : Optional: Title for output (NULL == no title)
 *
 * @return whd_result_t
 */
whd_result_t whd_arp_features_print(uint32_t features, const char *title);

/** Add ARP Offload Host IP Identifier(s) to HostIP List to Device (WLAN)
 *
 * @param[in]    whd            : whd Instance
 * @param[in]    host_ipv4_list : pointer to host_ip data (IPv4, 1 uint32_t per ip addr)
 * @param[in]    count          : Number of array elements in host_ip
 *
 * @return whd_result_t
 */
whd_result_t whd_arp_hostip_list_add(whd_t *whd, uint32_t *host_ipv4_list, uint32_t count);

/** Add One ARP Offload Host IP Identifier to HostIP List (mbed-style IP string) to Device (WLAN)
 *
 * @param[in]    whd        : whd Instance
 * @param[in]    ip_addr    : pointer to ip string (as returned from mbedos calls)
 *
 * @return whd_result_t
 */
whd_result_t whd_arp_hostip_list_add_string(whd_t *whd, const char *ip_addr);

/** Clear One ARP Offload Host IP Identifier from Host IP List in Device (WLAN)
 *
 * @param[in]    whd        : whd Instance
 * @param[in]    ipv4_addr  : ip addr expressed as a uint32_t
 *
 * @return whd_result_t
 */
whd_result_t whd_arp_hostip_list_clear_id(whd_t *whd, uint32_t ipv4_addr);

/** Clear One ARP Offload Host IP Identifier from Host IP List (mbed-style IP string) in Device (WLAN)
 *
 * @param[in]    whd        : whd Instance
 * @param[in]    ip_addr    : pointer to ip string (as returned from mbedos calls)
 *
 * @return whd_result_t
 */
whd_result_t whd_arp_hostip_list_clear_id_string(whd_t *whd, const char *ip_addr);

/** Clear all ARP Offload Host IP Identifier List
 *
 * @param[in]    whd        : whd Instance
 *
 * @return whd_result_t
 */
whd_result_t whd_arp_hostip_list_clear(whd_t *whd);

/** Get ARP Offload Host IP Identifiers from Device (WLAN)
 *
 * @param[in]    whd            : whd Instance
 * @param[in]    count          : Number of array elements in host_ip
 * @param[out]   host_ipv4_list : Pointer to structure array to store host_ip data
 * @param[out]   filled         : Number of array elements filled by this routine
 *
 * @return whd_result_t
 */
whd_result_t whd_arp_hostip_list_get(whd_t *whd, uint32_t count, uint32_t *host_ipv4_list, uint32_t *filled);

/** Clear ARP Offload statistics in Device (WLAN)
 *
 * @param[in]    whd        : whd Instance
 *
 * @return whd_result_t
 */
whd_result_t whd_arp_stats_clear(whd_t *whd);

/** Get ARP Offload statistics from Device (WLAN)
 *
 * @param[in]    whd        : whd Instance
 * @param[out]   stats      : Ptr to store statistics @ref whd_arp_stats_t
 *
 * @return whd_result_t
 */
whd_result_t whd_arp_stats_get(whd_t *whd, whd_arp_stats_t *stats);

/** Print ARP Offload statistics
 *  NOTE: call whd_arp_stats_get(), then print them using this function.
 *
 * @param[in]    arp_stats  : Ptr to ARP statistics structure
 * @param[in]    title      : Optional: Title for output (NULL == no title)
 *
 * @return whd_result_t
 */
whd_result_t whd_arp_stats_print(whd_arp_stats_t *arp_stats, const char *title);

/** @} */

#ifdef __cplusplus
}
#endif

#endif     /* !WHD_ARP_API_H__ */

