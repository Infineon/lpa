/***************************************************************************//**
* \file cy_whd_arp_api.c
* \version 1.0
*
* \brief
* WHD ARP Offload API
*
********************************************************************************
* \copyright
* Copyright 2019, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#include <string.h>
#include <stdio.h>
#include "whd.h"
#include "whd_sdpcm.h"
#include "whd_endian.h"
#include "ip4string.h"
#include "cy_lpa_wifi_ol.h"
#include "cy_lpa_wifi_ol_common.h"
#include "cy_lpa_wifi_arp_ol.h"             // really need whd_wliocl.h for arp stats struct
#include "cy_whd_arp_api.h"

#ifdef __cplusplus
extern "C" {
#endif


#ifdef ARP_OL_DEBUG
#define    ARP_OL_PRINT_DEBUG(arg)    { printf arg; }
#define    ARP_OL_PRINT_ERROR(arg)    { printf arg; }
#else
#define    ARP_OL_PRINT_DEBUG(arg)
#define    ARP_OL_PRINT_ERROR(arg)
#endif

/*********************************************************************************************
*
*  Macros
*
*********************************************************************************************/

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

/*
 * ARP Offload version
 *    ARP version in the WLAN Firmware
 *
 * @param[in]    ifp            - whd interface Instance
 * @param[out]    version        - pointer to store version #
 *
 * @return @ref whd_result_t
 */
whd_result_t whd_arp_version(whd_t *ifp, uint32_t *value)
{
    if (ifp == NULL)
    {
        return WHD_BADARG;
    }
    return whd_wifi_get_iovar_value(ifp, "arp_version", value);
}

/*
 * Get ARP Offload Peer Age
 *    ARP Peer Age-out value
 *
 * @param[in]    ifp            - whd interface Instance
 * @param[out]    value        - pointer to store value
 *
 * @return @ref whd_result_t
 */
whd_result_t whd_arp_peerage_get(whd_t *ifp, uint32_t *value)
{
    if (ifp == NULL)
    {
        return WHD_BADARG;
    }
    return whd_wifi_get_iovar_value(ifp, "arp_peerage", value);
}

/*
 * Set ARP Offload Peer Age
 *    ARP Peer Age-out value
 *
 * @param[in]    ifp            - whd interface Instance
 * @param[in]    value        - Seconds to age out IP
 *
 * @return @ref whd_result_t
 */
whd_result_t whd_arp_peerage_set(whd_t *ifp, uint32_t value)
{
    if (ifp == NULL)
    {
        return WHD_BADARG;
    }
    return whd_wifi_set_iovar_value(ifp, "arp_peerage", value);
}

/*
 * Get ARP Offload Agent Enable
 *
 * @param[in]    ifp            - whd interface Instance
 * @param[out]    value        - pointer to store value
 *
 * @return @ref whd_result_t
 */
whd_result_t whd_arp_arpoe_get(whd_t *ifp, uint32_t *value)
{
    if (ifp == NULL)
    {
        return WHD_BADARG;
    }
    return whd_wifi_get_iovar_value(ifp, "arpoe", value);
}

/*
 * Set ARP Offload Agent Enable
 *    Set Enable/Disable of ARP Offload Agent
 *
 * @param[in]    ifp            - whd interface Instance
 * @param[in]    value        - Enable=1 / Disable=0
 *
 * @return @ref whd_result_t
 */
whd_result_t whd_arp_arpoe_set(whd_t *ifp, uint32_t value)
{
    if (ifp == NULL)
    {
        return WHD_BADARG;
    }
    return whd_wifi_set_iovar_value(ifp, "arpoe", value);
}

/*
 * Clear ARP Offload cache
 *
 * @param[in]    ifp            - whd interface Instance
 *
 * @return @ref whd_result_t
 */
whd_result_t whd_arp_cache_clear(whd_t *ifp)
{
    whd_result_t whd_ret;
    if (ifp == NULL)
    {
        return WHD_BADARG;
    }
    whd_ret = whd_wifi_set_iovar_void(ifp, "arp_table_clear");
    return whd_ret;
}

/*
 * Get ARP Offload Feature Flags
 *
 * @param[in]    ifp        - whd interface Instance
 * @param[out]    features- ptr to store currently set features - bit flags
 *                           ARL_OL_AGENT | ARL_OL_SNOOP | ARP_OL_HOST_AUTO_REPLY | ARP_OL_PEER_AUTO_REPLY
 *
 * @return @ref whd_result_t
 */
whd_result_t whd_arp_features_get(whd_t *ifp, uint32_t *features)
{
    if ( (ifp == NULL) || (features == NULL) )
    {
        return WHD_BADARG;
    }

    if (whd_wifi_get_iovar_buffer(ifp, "arp_ol", (uint8_t *)features, sizeof(uint32_t) ) != WHD_SUCCESS)
    {
        ARP_OL_PRINT_ERROR( ("%s() failed to get arp_ol for features\n", __func__) );
        return WHD_IOCTL_FAIL;
    }

    return WHD_SUCCESS;
}

/*
 * Set ARP Offload Feature Flags
 *
 * @param[in]    ifp        - whd interfaceInstance
 * @param[in]    features- feature flags to set (you can OR '|' multiple flags)
 *                           ARL_OL_AGENT | ARL_OL_SNOOP | ARP_OL_HOST_AUTO_REPLY | ARP_OL_PEER_AUTO_REPLY
 *
 * @return @ref whd_result_t
 */
whd_result_t whd_arp_features_set(whd_t *ifp, uint32_t features)
{
    if (ifp == NULL)
    {
        return WHD_BADARG;
    }

    return whd_wifi_set_iovar_buffer(ifp, "arp_ol", (uint8_t *)&features, sizeof(features) );
}

/*
 * Print ARP Offload Feature Flags
 *
 * @param[in]    features- feature flags to set (you can OR '|' multiple flags)
 *                           ARL_OL_AGENT | ARL_OL_SNOOP | ARP_OL_HOST_AUTO_REPLY | ARP_OL_PEER_AUTO_REPLY
 * @param[in]    title   - ptr to title string
 *
 * @return @ref whd_result_t
 */
whd_result_t whd_arp_features_print(uint32_t features, const char *title)
{
    if (title != NULL)
    {
        printf("%s\n", title);
    }
    printf("            features     : 0x%x\n", (int)features);
    printf("            agent_enabled: (0x%x) %s\n", (int)(features & ARP_OL_AGENT),
           (features & ARP_OL_AGENT) ? "Enabled" : "  disabled");
    printf("            snoop_enabled: (0x%x) %s\n", (int)(features & ARP_OL_SNOOP),
           (features & ARP_OL_SNOOP) ? "Enabled" : "  disabled");
    printf("  host_auto_reply_enabled: (0x%x) %s\n", (int)(features & ARP_OL_HOST_AUTO_REPLY),
           (features & ARP_OL_HOST_AUTO_REPLY) ? "Enabled" : "  disabled");
    printf("  peer_auto_reply_enabled: (0x%x) %s\n", (int)(features & ARP_OL_PEER_AUTO_REPLY),
           (features & ARP_OL_PEER_AUTO_REPLY) ? "Enabled" : "  disabled");

    return WHD_SUCCESS;
}

/*
 * Add ARP Offload Host IP Identifier(s) to HostIP List (mbed-style IP string)
 *
 * @param[in]    ifp        - whd interface Instance
 * @param[in]    host_ip    - pointer to host_ip data
 * @param[in]    count   - Number of array elements in host_ip
 *
 * @return @ref whd_result_t
 */
whd_result_t whd_arp_hostip_list_add(whd_t *ifp, uint32_t *host_ipv4_list, uint32_t count)
{
    uint32_t filled = 0;
    uint32_t current_ipv4_list[ARP_MULTIHOMING_MAX];

    whd_result_t whd_ret = WHD_SUCCESS;
    if ( (ifp == NULL) || (host_ipv4_list == NULL) )
    {
        ARP_OL_PRINT_DEBUG( ("%s() BAD ARGS ifp:%p host_ipv4_list:%p count %ld\n", __func__, ifp, host_ipv4_list,
                             count) );
        return WHD_BADARG;
    }
    /* check if unique */
    whd_ret = whd_arp_hostip_list_get(ifp, ARP_MULTIHOMING_MAX, current_ipv4_list, &filled);
    if ( (whd_ret == WHD_SUCCESS) && (filled > 0) )
    {
        uint32_t curr_index;
        uint32_t new_index;

        for (curr_index = 0; curr_index < filled; curr_index++)
        {
            for (new_index = 0; new_index < count; new_index++)
            {
                ARP_OL_PRINT_DEBUG( ("%s() curr:%ld of %ld curr:0x%lx new:%ld of %ld:0x%lx\n", __func__, curr_index,
                                     filled, current_ipv4_list[curr_index],
                                     new_index, count, host_ipv4_list[new_index]) );
                if (current_ipv4_list[curr_index] == host_ipv4_list[new_index])
                {
                    /* decrement count */
                    count--;
                    if (new_index < count)
                    {
                        /* copy next one down */
                        ARP_OL_PRINT_DEBUG( ("move %ld (+1) of %ld \n", new_index, count) );
                        host_ipv4_list[new_index] = host_ipv4_list[new_index + 1];

                    }
                    break;
                }
            }
        }
    }
    else if (whd_ret != WHD_SUCCESS)
    {
        ARP_OL_PRINT_DEBUG( ("%s() whd_arp_hostip_list_get() failed:%d\n", __func__, (int)whd_ret) );
    }

    if (count > 0)
    {
        uint32_t new_index;
        ARP_OL_PRINT_DEBUG( ("%s() whd_wifi_set_iovar_buffer( %p, %lx)\n", __func__, host_ipv4_list, count) );
        for (new_index = 0; new_index < count; new_index++)
        {
            ARP_OL_PRINT_DEBUG( ("  0x%lx\n", host_ipv4_list[new_index]) );
        }
        whd_ret = whd_wifi_set_iovar_buffer(ifp, "arp_hostip", host_ipv4_list, (count * sizeof(uint32_t) ) );
        if (whd_ret != WHD_SUCCESS)
        {
            ARP_OL_PRINT_ERROR( ("Failed to set arp_hostip 0x%0lx error:%d\n", host_ipv4_list[0], (int)whd_ret) );
        }
    }
    return whd_ret;
}

/*
 * Add One ARP Offload Host IP Identifier to HostIP List (mbed-style IP string)
 *
 * @param[in]    ifp        - whd interface Instance
 * @param[in]    ip_addr    - pointer to ip string (as returned from mbedos calls)
 *
 * @return @ref whd_result_t
 */
whd_result_t whd_arp_hostip_list_add_string(whd_t *ifp, const char *ip_addr)
{
    /* convert string to uint32_t */
    uint32_t addr;
    stoip4(ip_addr, strlen(ip_addr), &addr);
    if (ifp == NULL)
    {
        return WHD_BADARG;
    }
    return whd_arp_hostip_list_add(ifp, &addr, 1);
}

/*
 * Clear One ARP Offload Host IP Identifier from HostIP List
 *
 * @param[in]    ifp            - whd interface Instance
 * @param[in]    ipv4_addr    - ip addr expressed as a uint32_t
 *
 * @return @ref whd_result_t
 */
whd_result_t whd_arp_hostip_list_clear_id(whd_t *ifp, uint32_t ipv4_addr)
{
    whd_result_t whd_ret;
    uint32_t filled;
    uint32_t host_ipv4_list[ARP_MULTIHOMING_MAX];
    if ( (ifp == NULL) || (ipv4_addr == 0x00l) )
    {
        return WHD_BADARG;
    }
    memset(host_ipv4_list, 0x00, sizeof(host_ipv4_list) );
    whd_ret = whd_arp_hostip_list_get(ifp, ARP_MULTIHOMING_MAX, host_ipv4_list, &filled);
    if ( (whd_ret == WHD_SUCCESS) && (filled > 0) )
    {
        uint32_t index;

        /* clear the list in the WLAN processor */
        whd_ret = whd_wifi_set_iovar_void(ifp, "arp_hostip_clear");
        if (whd_ret != WHD_SUCCESS)
        {
            ARP_OL_PRINT_ERROR( ("%d %s() whd_wifi_set_iovar_void() failed:%d\n", __LINE__, __func__, (int)whd_ret) );
            return whd_ret;
        }

        /* remove the one address from the list and re-write arp_hostip list */
        for (index = 0; index < filled; index++)
        {
            ARP_OL_PRINT_DEBUG( ("%d %s() drop() 0x%lx == 0x%lx ? %s\n", __LINE__, __func__, host_ipv4_list[index],
                                 ipv4_addr, (host_ipv4_list[index] == ipv4_addr) ? "DROP" : "") );
            if (host_ipv4_list[index] == ipv4_addr)
            {
                uint32_t drop;
                /* drop this one, move rest up */
                for (drop = index; drop < (filled - 1); drop++)
                {
                    host_ipv4_list[drop] = host_ipv4_list[drop + 1];
                }
                filled--;
                /* IP addresses must be added one at a time */
                for (drop = 0; drop < filled; drop++)
                {
                    whd_ret = whd_arp_hostip_list_add(ifp, &host_ipv4_list[drop], sizeof(uint32_t) );
                }
                break;
            }
        }
    }
    else if (whd_ret != WHD_SUCCESS)
    {
        ARP_OL_PRINT_DEBUG( ("%s() whd_arp_hostip_list_get() failed:%d\n", __func__, (int)whd_ret) );
    }
    return WHD_SUCCESS;
}

/*
 * Clear One ARP Offload Host IP Identifier from HostIP List (mbed-style IP string)
 *
 * @param[in]    ifp        - whd interface Instance
 * @param[in]    ip_addr    - pointer to ip string (as returned from mbedos calls)
 *
 * @return @ref whd_result_t
 */
whd_result_t whd_arp_hostip_list_clear_id_string(whd_t *ifp, const char *ip_addr)
{
    /* convert string to uint32_t */
    uint32_t addr;
    stoip4(ip_addr, strlen(ip_addr), &addr);
    if (ifp == NULL)
    {
        return WHD_BADARG;
    }
    return whd_arp_hostip_list_clear_id(ifp, addr);
}

/*
 * Clear all ARP Offload Host IP Identifiers
 *
 * @param        N/A
 *
 * @return @ref whd_result_t
 */
whd_result_t whd_arp_hostip_list_clear(whd_t *ifp)
{
    if (ifp == NULL)
    {
        return WHD_BADARG;
    }
    return whd_wifi_set_iovar_void(ifp, "arp_hostip_clear");
}

/*
 * Get ARP Offload Host IP Identifiers
 *
 * @param[in]    ifp        - whd interface Instance
 * @param[in]    count   - Number of array elements in host_ipv4_list
 * @param[out]    host_ip    - pointer to structure array to store host_ip data
 * @param[out]    count   - Number of array elements filled by this routine
 *
 * @return @ref whd_result_t
 */
whd_result_t whd_arp_hostip_list_get(whd_t *ifp, uint32_t count, uint32_t *host_ipv4_list, uint32_t *filled)
{
    whd_result_t whd_ret = WHD_SUCCESS;
    uint32_t temp[ARP_MULTIHOMING_MAX];
    struct arp_ol_stats_t arp_stats;               /* WL struct, not ours! */

    if ( (ifp == NULL) || (host_ipv4_list == NULL) || (filled == NULL) )
    {
        return WHD_BADARG;
    }

    /* set up the buffer to retrieve the stats data */
    memset(&arp_stats, 0x00, sizeof(struct arp_ol_stats_t) );
    whd_ret = whd_wifi_get_iovar_buffer(ifp, "arp_stats", (uint8_t *)&arp_stats, sizeof(struct arp_ol_stats_t) );
    if (whd_ret != WHD_SUCCESS)
    {
        ARP_OL_PRINT_ERROR( ("%s() failed to get arp_stats\n", __func__) );
        return WHD_IOCTL_FAIL;
    }

    *filled = 0;
    whd_ret =  whd_wifi_get_iovar_buffer(ifp, "arp_hostip", (uint8_t *)&temp, sizeof(temp) );
    /* transfer the info */
    if (whd_ret == WHD_SUCCESS)
    {
        uint32_t index;
        for (index = 0; (index < count) && (index < arp_stats.host_ip_entries); index++)
        {
            /* only IPv4 !!! */
            if (htod32(temp[index]) != 0L)
            {
                host_ipv4_list[*filled] =  temp[index];
                *filled = *filled + 1;
            }
        }
    }
    return whd_ret;
}

/*
 * Clear ARP Offload statistics
 *
 * @param[in]    ifp            - whd interface Instance
 *
 * @return @ref whd_result_t
 */
whd_result_t whd_arp_stats_clear(whd_t *ifp)
{
    whd_result_t whd_ret;
    if (ifp == NULL)
    {
        return WHD_BADARG;
    }
    whd_ret = whd_wifi_set_iovar_void(ifp, "arp_stats_clear");
    return whd_ret;
}

/*
 * Get ARP Offload statistics
 * NOTE: Will read stats multiple times until 2 sequential reads match
 *
 * @param[in]    ifp            - whd interface Instance
 * @param[out]    Ptr to ARP statistics structure
 *
 * @return @ref whd_result_t
 */

whd_result_t whd_arp_stats_get(whd_t *ifp, whd_arp_stats_t *arp_stats)
{
    whd_result_t whd_ret;
    uint32_t filled;
    static whd_arp_stats_t arp_stats_test;  /* read twice to make sure we match */

    if ( (ifp == NULL) || (arp_stats == NULL) )
    {
        return WHD_BADARG;
    }

    /* set up the buffer to retreive the data */
    memcpy(&arp_stats_test, arp_stats, sizeof(whd_arp_stats_t) );
    memset(arp_stats, 0xFF, sizeof(whd_arp_stats_t) );

    /* read multiple times to make sure we got valid data */
    do
    {
        /* get them until they match */
        whd_ret =
            whd_wifi_get_iovar_buffer(ifp, "arp_stats",   (uint8_t *)&arp_stats->stats, sizeof(struct arp_ol_stats_t) );
        if (whd_ret != WHD_SUCCESS)
        {
            ARP_OL_PRINT_ERROR( ("%s() failed to get arp_stats\n", __func__) );
            return WHD_IOCTL_FAIL;
        }
        /* get all feature info in one call */
        whd_ret =
            whd_wifi_get_iovar_buffer(ifp, "arp_ol", (uint8_t *)&arp_stats->features_enabled,
                                      sizeof(arp_stats->features_enabled) );
        if (whd_ret != WHD_SUCCESS)
        {
            ARP_OL_PRINT_ERROR( ("%s() failed to get arp_ol\n", __func__) );
            return WHD_IOCTL_FAIL;
        }
        whd_ret = whd_wifi_get_iovar_value(ifp, "arp_version", &(arp_stats->version) );
        if (whd_ret != WHD_SUCCESS)
        {
            ARP_OL_PRINT_ERROR( ("%s() failed to get arp_version\n", __func__) );
            return WHD_IOCTL_FAIL;
        }
        whd_ret = whd_wifi_get_iovar_value(ifp, "arp_peerage", &(arp_stats->peerage) );
        if (whd_ret != WHD_SUCCESS)
        {
            ARP_OL_PRINT_ERROR( ("%s() failed to get arp_peerage\n", __func__) );
            return WHD_IOCTL_FAIL;
        }
        whd_ret = whd_wifi_get_iovar_value(ifp, "arpoe", &(arp_stats->arpoe) );
        if (whd_ret != WHD_SUCCESS)
        {
            ARP_OL_PRINT_ERROR( ("%s() failed to get some settings\n", __func__) );
            return WHD_IOCTL_FAIL;
        }

        /* set endian correctly */
        arp_stats->stats.host_ip_entries     = dtoh32(arp_stats->stats.host_ip_entries);
        arp_stats->stats.host_ip_overflow     = dtoh32(arp_stats->stats.host_ip_overflow);
        arp_stats->stats.arp_table_entries     = dtoh32(arp_stats->stats.arp_table_entries);
        arp_stats->stats.arp_table_overflow = dtoh32(arp_stats->stats.arp_table_overflow);
        arp_stats->stats.host_request         = dtoh32(arp_stats->stats.host_request);
        arp_stats->stats.host_reply         = dtoh32(arp_stats->stats.host_reply);
        arp_stats->stats.host_service         = dtoh32(arp_stats->stats.host_service);
        arp_stats->stats.peer_request         = dtoh32(arp_stats->stats.peer_request);
        arp_stats->stats.peer_request_drop     = dtoh32(arp_stats->stats.peer_request_drop);
        arp_stats->stats.peer_reply         = dtoh32(arp_stats->stats.peer_reply);
        arp_stats->stats.peer_reply_drop     = dtoh32(arp_stats->stats.peer_reply_drop);
        arp_stats->stats.peer_service         = dtoh32(arp_stats->stats.peer_service);

        whd_ret = whd_arp_hostip_list_get(ifp, ARP_MULTIHOMING_MAX, arp_stats->host_ip_list, &filled);
        if (whd_ret != WHD_SUCCESS)
        {
            ARP_OL_PRINT_ERROR( ("%s() failed to get host_ip_list\n", __func__) );
            return WHD_IOCTL_FAIL;
        }

        if (memcmp(&arp_stats_test, arp_stats, sizeof(whd_arp_stats_t) ) == 0)
        {
            break;
        }

        memcpy(&arp_stats_test, arp_stats, sizeof(whd_arp_stats_t) );
    } while (1);

    return whd_ret;
}

/*
 * Print ARP Offload statistics
 *
 * @param[in]    Ptr to ARP statistics structure
 *
 * @return @ref whd_result_t
 */
whd_result_t whd_arp_stats_print(whd_arp_stats_t *arp_stats, const char *title)
{
    uint32_t index;

    if (arp_stats == NULL)
    {
        return WHD_BADARG;
    }

    if (title != NULL)
    {
        printf("%s\n", title);
    }
    printf("                  version: 0x%lx\n", (unsigned long int)arp_stats->version);
    printf("          host_ip_entries: %d\n", (int)arp_stats->stats.host_ip_entries);
    printf("         host_ip_overflow: %d\n", (int)arp_stats->stats.host_ip_overflow);
    printf("        arp_table_entries: %d\n", (int)arp_stats->stats.arp_table_entries);
    printf("       arp_table_overflow: %d\n", (int)arp_stats->stats.arp_table_overflow);
    printf("             host_request: %d\n", (int)arp_stats->stats.host_request);
    printf("               host_reply: %d\n", (int)arp_stats->stats.host_reply);
    printf("             host_service: %d\n", (int)arp_stats->stats.host_service);
    printf("             peer_request: %d\n", (int)arp_stats->stats.peer_request);
    printf("        peer_request_drop: %d\n", (int)arp_stats->stats.peer_request_drop);
    printf("               peer_reply: %d\n", (int)arp_stats->stats.peer_reply);
    printf("          peer_reply_drop: %d\n", (int)arp_stats->stats.peer_reply_drop);
    printf("             peer_service: %d\n", (int)arp_stats->stats.peer_service);
    printf("                  peerage: %d\n", (int)arp_stats->peerage);
    printf("                    arpoe: %d %s\n", (int)arp_stats->arpoe,
           (arp_stats->arpoe != 0) ? "Enabled" : "  disabled");

    whd_arp_features_print(arp_stats->features_enabled, NULL);

    if (arp_stats->stats.host_ip_entries > 0)
    {
        printf ("WLAN Device Host IP entries\n");
        for (index = 0; index < arp_stats->stats.host_ip_entries; index++)
        {
            uint32_t ipv4_addr = arp_stats->host_ip_list[index];
            char ipv4_string[32];
            ip4tos(&ipv4_addr, ipv4_string);
            printf("  %d of %d IPV4: 0x%x %s\n", (int)index, (int)arp_stats->stats.host_ip_entries,
                   (int)arp_stats->host_ip_list[index], ipv4_string);
        }
    }
    return WHD_SUCCESS;
}

#ifdef __cplusplus
}
#endif

