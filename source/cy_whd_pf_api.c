/***************************************************************************//**
* \file cy_whd_pf_api.c
* \version 1.0
*
* \brief
* WHD Packet Filter Offload API
*
********************************************************************************
* \copyright
* Copyright 2019, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#include "whd_types.h"
#include "cy_lpa_wifi_ol_common.h"
#include "cy_lpa_wifi_ol.h"
#include "cy_lpa_wifi_pf_ol.h"
#include "cy_whd_pf_api.h"
#include "whd_cdc_bdc.h"
#include "whd_network_types.h"
#include "whd_buffer_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************
 *
 *  Macros
 *
 *********************************************************************************************/

#define MIN(x, y) ( (x <= y) ? x : y )
#define MAX(x, y) ( (x <= y) ? y : x )
#define IFP_TO_DRIVER(ifp)  ( ( (whd_interface_t)ifp )->whd_driver )

/*********************************************************************************************
*
*  Data
*
*********************************************************************************************/

/*********************************************************************************************
*
*  Functions
*
*********************************************************************************************/

whd_result_t
whd_wifi_toggle_packet_filter(whd_t *whd, uint8_t filter_id, whd_bool_t enable)
{
    whd_buffer_t buffer;
    wl_pkt_filter_enable_t *data = (wl_pkt_filter_enable_t *)whd_cdc_get_iovar_buffer(IFP_TO_DRIVER(
                                                                                          whd), &buffer,
                                                                                      sizeof(wl_pkt_filter_enable_t),
                                                                                      IOVAR_STR_PKT_FILTER_ENABLE);
    CHECK_IOCTL_BUFFER(data);
    data->id     = (uint32_t)filter_id;
    data->enable = (uint32_t)enable;
    RETURN_WITH_ASSERT(whd_cdc_send_iovar(whd, CDC_SET, buffer, NULL) );
}

whd_result_t
whd_pf_enable_packet_filter(whd_t *whd, uint8_t filter_id)
{
    return whd_wifi_toggle_packet_filter(whd, filter_id, WHD_TRUE);
}

whd_result_t
whd_pf_disable_packet_filter(whd_t *whd, uint8_t filter_id)
{
    return whd_wifi_toggle_packet_filter(whd, filter_id, WHD_FALSE);
}

whd_result_t
whd_pf_add_packet_filter(whd_t *whd, const whd_packet_filter_t *settings)
{
    wl_pkt_filter_t *packet_filter;
    whd_buffer_t buffer;
    uint32_t buffer_length =
        (uint32_t)( (2 * (uint32_t)settings->mask_size) + WL_PKT_FILTER_FIXED_LEN + WL_PKT_FILTER_PATTERN_FIXED_LEN );

    packet_filter = (wl_pkt_filter_t *)whd_cdc_get_iovar_buffer(IFP_TO_DRIVER(
                                                                    whd), &buffer, (uint16_t)buffer_length,
                                                                "pkt_filter_add");
    CHECK_IOCTL_BUFFER(packet_filter);

    /* Copy filter entries */
    packet_filter->id                   = settings->id;
    packet_filter->type                 = 0;
    packet_filter->negate_match         = settings->rule;
    packet_filter->u.pattern.offset     = (uint32_t)settings->offset;
    packet_filter->u.pattern.size_bytes = settings->mask_size;

    /* Copy mask */
    memcpy(packet_filter->u.pattern.mask_and_pattern, settings->mask, settings->mask_size);

    /* Copy filter pattern */
    memcpy(packet_filter->u.pattern.mask_and_pattern + settings->mask_size, settings->pattern, settings->mask_size);

    RETURN_WITH_ASSERT(whd_cdc_send_iovar(whd, CDC_SET, buffer, NULL) );
}

whd_result_t
whd_pf_remove_packet_filter(whd_t *whd, uint8_t filter_id)
{
    whd_buffer_t buffer;

    uint32_t *data = (uint32_t *)whd_cdc_get_iovar_buffer(IFP_TO_DRIVER(
                                                              whd), &buffer, sizeof(uint32_t), "pkt_filter_delete");
    CHECK_IOCTL_BUFFER(data);
    *data = (uint32_t)filter_id;
    RETURN_WITH_ASSERT(whd_cdc_send_iovar(whd, CDC_SET, buffer, NULL) );
}

whd_result_t
whd_pf_get_packet_filter_stats(whd_t *whd, uint8_t filter_id, wl_pkt_filter_stats_t *stats)
{
    whd_buffer_t buffer;
    whd_buffer_t response;

    uint32_t *data =
        (uint32_t *)whd_cdc_get_iovar_buffer(IFP_TO_DRIVER(
                                                 whd), &buffer, sizeof(uint32_t) + sizeof(wl_pkt_filter_stats_t),
                                             "pkt_filter_stats");
    CHECK_IOCTL_BUFFER(data);

    memset(data, 0, sizeof(uint32_t) + sizeof(wl_pkt_filter_stats_t) );
    *data = (uint32_t)filter_id;

    CHECK_RETURN(whd_cdc_send_iovar(whd, CDC_GET, buffer, &response) );

    memcpy( (char *)stats, (char *)whd_buffer_get_current_piece_data_pointer(IFP_TO_DRIVER(whd), response),
            (sizeof(wl_pkt_filter_stats_t) ) );

    whd_buffer_release(IFP_TO_DRIVER(whd), response, WHD_NETWORK_TX);

    return WHD_SUCCESS;
}

whd_result_t
whd_wifi_clear_packet_filter_stats(whd_t *whd, uint32_t filter_id)
{
    RETURN_WITH_ASSERT(whd_wifi_set_iovar_value(whd, IOVAR_STR_PKT_FILTER_CLEAR_STATS, (uint32_t)filter_id) );
}

#define PACKET_FILTER_LIST_BUFFER_MAX_LEN 1000

whd_result_t
whd_pf_get_packet_filter_mask_and_pattern(whd_t *whd, uint8_t filter_id, uint32_t max_size, uint8_t *mask,
                                          uint8_t *pattern, uint32_t *size_out)
{
    whd_bool_t enabled_list;

    for (enabled_list = WHD_FALSE; enabled_list <= WHD_TRUE; enabled_list++)
    {

        whd_buffer_t buffer;
        whd_buffer_t response;
        uint32_t *data;
        wl_pkt_filter_list_t *filter_list;
        wl_pkt_filter_t *filter_ptr;
        uint32_t i;
        wl_pkt_filter_t *in_filter;

        data = whd_cdc_get_iovar_buffer(IFP_TO_DRIVER(
                                            whd), &buffer, PACKET_FILTER_LIST_BUFFER_MAX_LEN,
                                        IOVAR_STR_PKT_FILTER_LIST);
        CHECK_IOCTL_BUFFER(data);
        *data = (uint32_t)enabled_list;

        CHECK_RETURN(whd_cdc_send_iovar(whd, CDC_GET, buffer, &response) );

        filter_list  = (wl_pkt_filter_list_t *)whd_buffer_get_current_piece_data_pointer(IFP_TO_DRIVER(whd), response);
        filter_ptr   = filter_list->filter;
        for (i = 0; i < filter_list->num; i++)
        {
            in_filter  = filter_ptr;

            if (in_filter->id == filter_id)
            {
                *size_out = MIN(in_filter->u.pattern.size_bytes, max_size);
                memcpy (mask,    in_filter->u.pattern.mask_and_pattern, *size_out);
                memcpy (pattern, in_filter->u.pattern.mask_and_pattern + in_filter->u.pattern.size_bytes, *size_out);
                whd_buffer_release(IFP_TO_DRIVER(whd), response, WHD_NETWORK_RX);
                if (*size_out < in_filter->u.pattern.size_bytes)
                {
                    return WHD_PARTIAL_RESULTS;
                }
                return WHD_SUCCESS;
            }

            /* Update WL filter pointer */
            filter_ptr =
                (wl_pkt_filter_t *)( (char *)filter_ptr +
                                     (WL_PKT_FILTER_FIXED_LEN + WL_PKT_FILTER_PATTERN_FIXED_LEN + 2 *
                                      in_filter->u.pattern.size_bytes) );

            /* WLAN returns word-aligned filter list */
            filter_ptr = (wl_pkt_filter_t *)ROUND_UP( (unsigned long)filter_ptr, 4 );
        }
    }
    return WHD_FILTER_NOT_FOUND;
}

#ifdef __cplusplus
}
#endif

