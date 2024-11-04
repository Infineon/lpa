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
* @file cy_lpa_wifi_tls_ol.c
* @brief Low Power Offload MQTT TLS Keepalive Assist Implementation.
*/

#include "string.h"
#include "cy_lpa_wifi_ol_debug.h"
#include "cy_lpa_wifi_ol.h"
#include "cy_lpa_wifi_ol_priv.h"
#include "cy_lpa_wifi_tls_ol.h"
#include "cy_lpa_wifi_result.h"
#ifdef COMPONENT_LWIP
#include "lwip/ip.h"
#endif
#ifdef COMPONENT_NETXDUO
#include "cy_nw_helper.h"
#endif
#include "whd_sdpcm.h"
#include "cy_whd_tls_api.h"
#include "whd_wifi_api.h"
#include "whd_wlioctl.h"
#include "whd_endian.h"
#include "whd_types.h"

#ifdef CYCFG_WIFI_MQTT_OL_SUPPORT

#ifdef __cplusplus
extern "C" {
#endif

#define MQTT_PINGREQ_DATA 0xc000 /* Standard Ping request data as per MQTT specification */
#define MQTT_HEADER_SIZE 4       /* MQTT header size */

static ol_init_t cylpa_tlsoe_ol_init;
static ol_deinit_t cylpa_tlsoe_ol_deinit;
static ol_pm_t cylpa_tlsoe_ol_pm;

const ol_fns_t tlsoe_ol_fns =
{
    .init = cylpa_tlsoe_ol_init,
    .deinit = cylpa_tlsoe_ol_deinit,
    .pm = cylpa_tlsoe_ol_pm,
};

/*******************************************************************************
 * Global Declarations
*******************************************************************************/
cy_tlsoe_ol_cfg_t cy_tlsoe_ol_cfg = {
   {
       {
          .interval = 0,
          .local_port = 0,
          .remote_port = 0, 
          .remote_ip = "0.0.0.0",
          .data_len = 0,
          .data = "",
          .tls_socket = (void*)0,
       }
   }
};

/* cy_tlsoe_ol_cfg index points to an index to the TLS connection parameters
 * upto maximum of MAX_TLSOE TCP connections supported.
 */
uint8_t cy_tlsoe_ol_cfg_index = 0;

/*********************************************************************************************
*
*  Forward Declarations
*
*********************************************************************************************/


/*******************************************************************************
* Function Prototypes
*******************************************************************************/


/*******************************************************************************
 * Function Name: cylpa_tlsoe_ol_init
 ****************************************************************************//**
 *
 * TLS Keepalive offload filter init function.
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
static int cylpa_tlsoe_ol_init( void *ol, ol_info_t *info, const void *cfg )
{
    tlsoe_ol_t *ctxt = (tlsoe_ol_t *)ol;

    OL_LOG_TLSOE( LOG_OLA_LVL_DEBUG, "%s\n", __func__ );
    memset( ctxt, 0, sizeof(tlsoe_ol_t) );

    if ( cfg == NULL )
    {
        OL_LOG_TLSOE( LOG_OLA_LVL_ERR, "TLS Keep Alive offload not configured!!\n", __func__ );
        return RESULT_BADARGS;
    }

    ctxt->cfg = (cy_tlsoe_ol_cfg_t *)cfg;
    ctxt->whd  = info->whd;

    memcpy( &cy_tlsoe_ol_cfg, ctxt->cfg, sizeof( cy_tlsoe_ol_cfg ) );

    OL_LOG_TLSOE( LOG_OLA_LVL_DEBUG, "%d local %d, ", cy_tlsoe_ol_cfg.ports[0].local_port );
    OL_LOG_TLSOE( LOG_OLA_LVL_DEBUG, "   remote %d, ", cy_tlsoe_ol_cfg.ports[0].remote_port );
    OL_LOG_TLSOE( LOG_OLA_LVL_DEBUG, "   IP %s\n", cy_tlsoe_ol_cfg.ports[0].remote_ip );

    return RESULT_OK;
}

/*******************************************************************************
 * Function Name: cylpa_tlsoe_ol_deinit
 ****************************************************************************//**
 *
 * Remove TLS Keepalive offload filters. No need to disable prior to removal.
 *
 * \param ol
 * The pointer to the ol structure.
 *
 ********************************************************************************/
static void cylpa_tlsoe_ol_deinit( void *ol )
{
    tlsoe_ol_t *ctxt = ( tlsoe_ol_t * )ol;
    if ( ( ctxt == NULL ) || ( ctxt->whd == NULL ) )
    {
        return;
    }
}

/*******************************************************************************
 * Function Name: cylpa_tlsoe_ol_pm
 ****************************************************************************//**
 *
 * TLS offload Keepalive Power Management notification callback
 *
 * \param ol
 * The pointer to the ol structure.
 *
 * \param st
 * see \ref ol_pm_st_t.
 *
 ********************************************************************************/
static void cylpa_tlsoe_ol_pm( void *ol, ol_pm_st_t st )
{
    tlsoe_ol_t *ctxt = ( tlsoe_ol_t * )ol;
    cy_tlsoe_ol_cfg_t *tlsoe_cfg = &cy_tlsoe_ol_cfg;
    whd_result_t result;
    int found = 0;
    cy_tlsoe_ol_connect_t *port;

    if ( ( ctxt == NULL ) || ( ctxt->whd == NULL ) )
    {
        OL_LOG_TLSOE( LOG_OLA_LVL_ERR, "%s : Bad Args!\n", __func__ );
        return;
    }

    if ( st == OL_PM_ST_GOING_TO_SLEEP )
    {
        /* Sleeping case */
        OL_LOG_TLSOE( LOG_OLA_LVL_INFO, "TLSOE: Enter Sleep\n" );
        for ( int i = 0; i < MAX_TLSOE; i++ )
        {
            port = &tlsoe_cfg->ports[i];

            port->patt_offset = MQTT_HEADER_SIZE;

            result = whd_tlsoe_set_wowl_pattern( ctxt->whd, (uint8_t *)port->wakepatt, port->patt_len, WOWL_PATTERN_TYPE_SECWOWL, port->patt_offset );
            if ( result == WHD_SUCCESS )
            {
                port->data[0] = ( ( MQTT_PINGREQ_DATA >> 8 ) & 0xFF);
                port->data[1] = ( MQTT_PINGREQ_DATA & 0xFF );
                port->data_len = MAX_PAYLOAD_LEN;

                result = whd_tlsoe_activate( ctxt->whd, port->local_port, port->remote_port, port->remote_ip, port->interval, (uint8_t *)port->data, port->data_len, port->tls_socket );
                if ( result != WHD_SUCCESS )
                {
                    OL_LOG_TLSOE( LOG_OLA_LVL_ERR, "%s: No such connection: %s local %d remote %d\n", __func__,
                               port->remote_ip, port->local_port, port->remote_port );
                    result = whd_tlsoe_del_wowl_pattern( ctxt->whd, (uint8_t *)port->wakepatt, port->patt_len, WOWL_PATTERN_TYPE_SECWOWL, port->patt_offset );
                    if ( result == WHD_SUCCESS )
                    {
                        OL_LOG_TLSOE( LOG_OLA_LVL_ERR, "%s: Deleting the wowl pattern when WHD activate fails \n", __func__ );
                    }
                }
                else
                {
                    found = 1;
                }
            }
        }
        if ( !found )
        {
            OL_LOG_TLSOE(LOG_OLA_LVL_ERR, "TLSOE PM: No connections to enable.\n");
        }
    }
    else
    {
        /* Wake case */
        OL_LOG_TLSOE( LOG_OLA_LVL_DEBUG, "%s: Wakeup and disable TLSOE\n", __func__);
        
        for ( int i = 0; i < MAX_TLSOE; i++ )
        {
            port = &tlsoe_cfg->ports[i];
            result = whd_tlsoe_del_wowl_pattern( ctxt->whd, (uint8_t *)port->wakepatt, port->patt_len, WOWL_PATTERN_TYPE_SECWOWL, port->patt_offset );
            if ( result == WHD_SUCCESS )
            {
                result = whd_tlsoe_disable( ctxt->whd, port->local_port, port->remote_port, port->remote_ip, port->tls_socket );
                if ( result != WHD_SUCCESS )
                {
                    OL_LOG_TLSOE( LOG_OLA_LVL_ERR, "%s: whd_tlsoe_disable returned failure\n", __func__ );
                }
            }
            else
            {
                OL_LOG_TLSOE( LOG_OLA_LVL_INFO, "%s: TLSOE_DISABLE failure\n", __func__ );
            }
        }
    }
}

/*******************************************************************************
 * Function Name: cylpa_tlsoe_ol_update_config
 ****************************************************************************//**
 *
 * Update the TLS keep-alive offload configuration list
 *
 * \param socket
 * The pointer to the tls socket.
 *
 * \param cfg 
 * The pointer to MQTT keep-alive offload parameters. \ref cy_tlsoe_ol_cfg_t
 *
 * \return
 * Returns the execution result
 *
 ********************************************************************************/
int cylpa_tlsoe_ol_update_config( int index, cy_tlsoe_ol_cfg_t *cfg, void* socket )
{
    cy_tlsoe_ol_connect_t *tlsoe_ol_connect_params = NULL;

    if ( index>= MAX_TLSOE )
    {
       return RESULT_BADARGS;
    }

    tlsoe_ol_connect_params = &cy_tlsoe_ol_cfg.ports[index];
    memset( tlsoe_ol_connect_params, 0, sizeof ( cy_tlsoe_ol_cfg.ports[index] ) );
    memcpy( &cy_tlsoe_ol_cfg.ports[index],&cfg->ports[index], sizeof ( cy_tlsoe_ol_connect_t ) );
    cy_tlsoe_ol_cfg.ports[index].tls_socket = socket;
 
    OL_LOG_TLSOE( LOG_OLA_LVL_DEBUG, "\nUpdating...\n" );
    OL_LOG_TLSOE( LOG_OLA_LVL_DEBUG, "%d: %s, ", index, cy_tlsoe_ol_cfg.ports[index].remote_ip );
    OL_LOG_TLSOE( LOG_OLA_LVL_DEBUG, "Local %d, ", cy_tlsoe_ol_cfg.ports[index].local_port );
    OL_LOG_TLSOE( LOG_OLA_LVL_DEBUG, "Remote %d\n", cy_tlsoe_ol_cfg.ports[index].remote_port );
    OL_LOG_TLSOE( LOG_OLA_LVL_DEBUG, "Remote %d\n", cy_tlsoe_ol_cfg.ports[index].remote_port );
    OL_LOG_TLSOE( LOG_OLA_LVL_DEBUG, "Interval:%d \n",  cy_tlsoe_ol_cfg.ports[index].interval );
    OL_LOG_TLSOE( LOG_OLA_LVL_DEBUG, "data :: %d %s, ", cy_tlsoe_ol_cfg.ports[index].data_len, cy_tlsoe_ol_cfg.ports[index].data );
    OL_LOG_TLSOE( LOG_OLA_LVL_DEBUG, "wakeup ::%s, ",  cy_tlsoe_ol_cfg.ports[index].wakepatt );
    OL_LOG_TLSOE( LOG_OLA_LVL_DEBUG, "Done \n\n" );

    return RESULT_OK;
}

int cylpa_tlsoe_ol_set_socket_config( int index, uint16_t local_port, uint16_t remote_port, uint32_t* remote_ip, uint16_t interval )
{
    cy_tlsoe_ol_connect_t *tlsoe_ol_connect_params = NULL;

    if ( index>= MAX_TLSOE || remote_ip == NULL )
    {
       return RESULT_BADARGS;
    }

    tlsoe_ol_connect_params = &cy_tlsoe_ol_cfg.ports[index];
    tlsoe_ol_connect_params->local_port = local_port;
    tlsoe_ol_connect_params->remote_port = remote_port;
    tlsoe_ol_connect_params->interval = interval;

    if( *remote_ip != 0 )
    {
#ifdef COMPONENT_LWIP
        strcpy( tlsoe_ol_connect_params->remote_ip, ip4addr_ntoa( ( const ip4_addr_t *) remote_ip ) );
#endif
#ifdef COMPONENT_NETXDUO
        char ip_str[16] = {0};
        cy_nw_ip_address_t addr;
        addr.version = NW_IP_IPV4;
        addr.ip.v4 = *remote_ip;
        cy_nw_ntoa (&addr, ip_str);
        strcpy( tlsoe_ol_connect_params->remote_ip, ip_str );
#endif
    }

    cy_tlsoe_ol_cfg_index++;

    return RESULT_OK;
}

#ifdef __cplusplus
}
#endif
#endif //CYCFG_WIFI_MQTT_OL_SUPPORT
