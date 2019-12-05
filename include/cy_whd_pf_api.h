/***************************************************************************//**
* \file cy_whd_pf_api.h
* \version 1.0
*
*
* WHD Packet Filter Offload API.
*
********************************************************************************
* \copyright
* Copyright 2019, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#ifndef WHD_PF_API_H__
#define WHD_PF_API_H__  (1)

#if defined(OLM_NO_HARDWARE)
typedef struct wl_pkt_filter_stats
{
    uint32_t num_pkts_matched;
    uint32_t num_pkts_forwarded;
    uint32_t num_pkts_discarded;
} wl_pkt_filter_stats_t;

typedef bool whd_bool_t;
#else
#include "whd.h"
#include "whd_wlioctl.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************************
 *
 * Packet Filter Offload API
 *
 **************************************************************************************/
/*
 * Different types of filters are different lengths.  Port filters are the
 * longest and are of this length. All filters start at EthType in header.
 */
#define PORT_FILTER_LEN 26

/*********************************************************************************************
*
*  Enums
*
*********************************************************************************************/

/**
 * Enumeration of packet filter rules
 */
typedef enum
{
    WHD_PACKET_FILTER_RULE_POSITIVE_MATCHING  = 0, /**< Specifies that a filter should match a given pattern     */
    WHD_PACKET_FILTER_RULE_NEGATIVE_MATCHING  = 1  /**< Specifies that a filter should NOT match a given pattern */
} whd_packet_filter_rule_t;

/*********************************************************************************************
*
*  Structures
*
*********************************************************************************************/

/**
 * Structure describing a packet filter list item
 */
typedef struct
{
    uint32_t id;                                  /**< Unique identifier for a packet filter item                             */
    whd_packet_filter_rule_t rule;                /**< Filter matches are either POSITIVE or NEGATIVE matching */
    uint16_t offset;                              /**< Offset in bytes to start filtering (referenced to the start of the ethernet packet) */
    uint16_t mask_size;                           /**< Size of the mask in bytes */
    uint8_t *mask;                                /**< Pattern mask bytes to be ANDed with the pattern eg. "\xff00" (must be in network byte order) */
    uint8_t *pattern;                             /**< Pattern bytes used to filter eg. "\x0800"  (must be in network byte order) */
    whd_bool_t enabled_status;                     /**< When returned from wwd_wifi_get_packet_filters, indicates if the filter is enabled */
} whd_packet_filter_t;


/*********************************************************************************************
*
*  Functions
*
*********************************************************************************************/
/** @addtogroup whdpf       WHD Packet Filter Offload Assist
 *  @{
 */

/** A filter must be added (e.g. created) before it can be enabled.
 * @param[in]    whd        : whd Instance
 * @param[in]    settings   : Ptr to filter settings @ref whd_packet_filter_t
 * @return whd_result_t
 */
whd_result_t whd_pf_add_packet_filter(whd_t *whd, const whd_packet_filter_t *settings);

/** Remove a previously added filter.
 * @param[in]    whd        : whd Instance
 * @param[in]    filter_id  : filter to remove
 * @return whd_result_t
 */
whd_result_t whd_pf_remove_packet_filter(whd_t *whd, uint8_t filter_id);

/** After a filter has been added it can be enabled or disabled as needed.
 * @param[in]    whd        : whd Instance
 * @param[in]    filter_id  : filter to enable
 * @return whd_result_t
 */
whd_result_t whd_pf_enable_packet_filter(whd_t *whd, uint8_t filter_id);

/** After a filter has been added it can be enabled or disabled as needed.
 * @param[in]    whd        : whd Instance
 * @param[in]    filter_id  : filter to disable
 * @return whd_result_t
 */
whd_result_t whd_pf_disable_packet_filter(whd_t *whd, uint8_t filter_id);

/** Filters are implemented in WLAN subsystem as a bit pattern and matching bit mask that
 *  are applied to incoming packets.  This API retrieves the pattern and mask.
 * @param[in]    whd        : whd Instance
 * @param[in]    filter_id  : which filter to retrieve
 * @param[in]    max_size   : size of both mask and pattern buffers
 * @param[in]    mask       : mask for this filter
 * @param[in]    pattern    : pattern for this filter
 * @param[out]   size_out   : length of both mask and pattern
 * @return whd_result_t
 */
whd_result_t whd_pf_get_packet_filter_mask_and_pattern(whd_t *whd, uint8_t filter_id, uint32_t max_size, uint8_t *mask,
                                                       uint8_t *pattern, uint32_t *size_out);

/** Return the stats associated with a filter
 * @param[in]    whd        : whd Instance
 * @param[in]    filter_id  : which filter
 * @param[out]   stats      : Ptr to store statistics wl_pkt_filter_stats_t
 * @return whd_result_t
 */
whd_result_t whd_pf_get_packet_filter_stats(whd_t *whd, uint8_t filter_id, wl_pkt_filter_stats_t *stats);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* !WHD_PF_API_H__ */

