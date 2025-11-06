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
* @file  : cy_lpa_wifi_nko_ol.c
* @brief : Handles NAT Keep Alive offload
*/

#include "string.h"
#include "cy_lpa_wifi_ol_debug.h"
#include "cy_lpa_common_priv.h"
#include "cy_lpa_wifi_ol.h"
#include "cy_lpa_wifi_ol_priv.h"
#include "cy_lpa_wifi_nko_ol.h"
#include "cy_lpa_wifi_result.h"

#include "whd_wifi_api.h"
#include "whd_wlioctl.h"
#include "whd_endian.h"
#include "whd_types.h"
#include "cy_nw_helper.h"
#include "cy_wcm.h"
#include "cy_secure_sockets.h"
#include "cy_nw_lpa_helper.h"

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************
*
*  Defines
*
********************************************************************/
/* IP packet */
#define CY_NKO_OL_ETHER_TYPE            0x0800

#ifdef NKO_OL_DEBUG
#define NKO_ERROR_PRINTF(x)             printf x
#define NKO_DEBUG_PRINTF(x)             printf x
#else
#define NKO_ERROR_PRINTF(x)
#define NKO_DEBUG_PRINTF(x)
#endif

#define NKO_ERROR                       0xffee

#define NAT_KEEPALIVE_FIXED_LEN         (sizeof(whd_ether_header_t) + sizeof(ipv4_hdr_t)  + sizeof(udp_hdr_t))
#define NAT_KEEPALIVE_MAX_LEN           (NAT_KEEPALIVE_FIXED_LEN + CYLPA_NAT_KEEPALIVE_MAX_PAYLOAD)


#define CY_NKO_SUPPORTED(ctx)           ( ctx->ol_info_ptr && ctx->ol_info_ptr->fw_new_infra )

/********************************************************************
 *
 *  Structures
 *
 *********************************************************************/
typedef struct nat_keepalive
{
    whd_mac_t   src_mac;                                    /**< local mac address      */
    whd_mac_t   dst_mac;                                    /**< remote mac address     */
    uint32_t    srcip;                                      /**< local IP address       */
    uint32_t    dstip;                                      /**< destination IP address */
    uint16_t    srcport;                                    /**< Local UDP port         */
    uint16_t    dstport;                                    /**< Remote UDP port        */
    uint8_t     payload[CYLPA_NAT_KEEPALIVE_MAX_PAYLOAD];   /**< UDP payload            */
    uint16_t    payload_len;                                /**< UDP payload length     */
} nat_keepalive_t;

/*********************************************************************************************
*
*  Forward Declarations
*
*********************************************************************************************/
static ol_init_t cylpa_nko_ol_init;
static ol_deinit_t cylpa_nko_ol_deinit;
static ol_pm_t cylpa_nko_ol_pm;

/*********************************************************************************************
*
*  Global Declarations
*
*********************************************************************************************/
static cy_lpa_nko_ol_cfg_t cy_nko_ol_cfg = {
    .interval = 0,
    .server   = "",
    .s_port   = 0,
    .d_port   = 0,
    .payload  = "",
};

const ol_fns_t nko_ol_fns =
{
    .init   = cylpa_nko_ol_init,
    .deinit = cylpa_nko_ol_deinit,
    .pm     = cylpa_nko_ol_pm,
};

static nko_ol_t *nko_ctx = NULL;
static cylpa_nw_ip_status_change_callback_t cy_nko_ol_cb;

/*******************************************************************************
* Function Name: cylpa_nko_get_details
****************************************************************************//**
*
* \param ctxt
* Pointer to nko_ol_t structure.
*
* \param pkt
* Pointer to NAT Keepalive configuration
*
* \param cfg
* NAT KeepAlive config
*
* Retrieves all the informations required to generate NAT KeepAlive packet
*******************************************************************************/
static int32_t cylpa_nko_get_details(nko_ol_t *ctxt,  nat_keepalive_t *pkt, cy_lpa_nko_ol_cfg_t *cfg)
{
    whd_result_t result = WHD_SUCCESS;
    cy_rslt_t res;
    cy_wcm_ip_address_t ipaddr;
    cy_socket_ip_address_t dest_addr;
    cy_wcm_mac_t mac_addr;

    /* src ip */
    res = cy_wcm_get_ip_addr(CY_WCM_INTERFACE_TYPE_STA,  &ipaddr);
    if ( res != CY_RSLT_SUCCESS )
    {
        NKO_ERROR_PRINTF( ("NKO: get src ip failed\n") );
        return RESULT_ERROR;
    }
    pkt->srcip = ipaddr.ip.v4;

    /* dest ip */
    res = cy_socket_gethostbyname(cfg->server, CY_SOCKET_IP_VER_V4, &dest_addr);
    if( res != CY_RSLT_SUCCESS )
    {
        res = cy_wcm_get_gateway_ip_address(CY_WCM_INTERFACE_TYPE_STA,  &ipaddr);
        if ( res != CY_RSLT_SUCCESS )
        {
            NKO_ERROR_PRINTF( ("NKO: get destination ip failed\n") );
            return RESULT_ERROR;
        }
        pkt->dstip = ipaddr.ip.v4;
    }
    else
    {
        pkt->dstip = dest_addr.ip.v4;
    }

    /* Local mac address */
    result =  whd_wifi_get_mac_address(ctxt->whd, &pkt->src_mac);
    if ( result != WHD_SUCCESS )
    {
        NKO_ERROR_PRINTF( ("NKO: Get_mac_address failed\n") );
        return RESULT_ERROR;
    }

    /* Remote mac address */
    result = cy_wcm_get_gateway_mac_address(&mac_addr);
    if(result != CY_RSLT_SUCCESS)
    {
        NKO_ERROR_PRINTF( ("NKO: Unable to get gateway mac address\n") );
        return WHD_BADARG;
    }
    memcpy(pkt->dst_mac.octet, mac_addr, sizeof(pkt->dst_mac.octet));

    NKO_DEBUG_PRINTF( ("Local Mac Addr: %02X:%02X:%02X:%02X:%02X:%02X\n",
            pkt->src_mac.octet[0], pkt->src_mac.octet[1], pkt->src_mac.octet[2],
            pkt->src_mac.octet[3], pkt->src_mac.octet[4], pkt->src_mac.octet[5]) );
    NKO_DEBUG_PRINTF( ("Remote Mac addr: %02X:%02X:%02X:%02X:%02X:%02X\n",
            pkt->dst_mac.octet[0], pkt->dst_mac.octet[1], pkt->dst_mac.octet[2],
            pkt->dst_mac.octet[3], pkt->dst_mac.octet[4], pkt->dst_mac.octet[5]) );


    pkt->srcport = cfg->s_port;
    pkt->dstport = cfg->d_port;

    pkt->payload_len = strlen(cfg->payload);
    if ( pkt->payload_len > 0 )
    {
        if ( pkt->payload_len > CYLPA_NAT_KEEPALIVE_MAX_PAYLOAD )
        {
            pkt->payload_len = CYLPA_NAT_KEEPALIVE_MAX_PAYLOAD;
        }
        memcpy(pkt->payload, cfg->payload, pkt->payload_len);
    }
    return RESULT_OK;
}

/*******************************************************************************
* Function Name: cylpa_nko_prep_packet
****************************************************************************//**
*
* \param ctxt
* Pointer to nko_ol_t structure.
*
* \param pkt
* Pointer to NAT Keepalive configuration
*
* \param buf
* Buffer to store the Keepalive packet
*
* Create the NAT Keepalive packet
*******************************************************************************/
static int32_t cylpa_nko_prep_packet(nko_ol_t *ctxt,  nat_keepalive_t *pkt, uint8_t *buf)
{
    /* Ethernet header */
    whd_ether_header_t *eth;
    /* ip header */
    ipv4_hdr_t *ip;
    /* UDP header */
    udp_hdr_t *udp;

    if ( buf == NULL )
    {
        NKO_ERROR_PRINTF( ("NKO: ERROR: NULL buf\n") );
        return NKO_ERROR;
    }

    eth = (whd_ether_header_t *)buf;

    /* Copy mac addresses */
    memcpy(eth->ether_dhost, pkt->dst_mac.octet, ETHER_ADDR_LEN);
    memcpy(eth->ether_shost, pkt->src_mac.octet, ETHER_ADDR_LEN);

    eth->ether_type = hton16(ETHER_TYPE_IP);

    NKO_DEBUG_PRINTF( ("dhost : %02X:%02X:%02X:%02X:%02X:%02X\n", eth->ether_dhost[0], eth->ether_dhost[1],
                       eth->ether_dhost[2], eth->ether_dhost[3], eth->ether_dhost[4], eth->ether_dhost[5]) );
    NKO_DEBUG_PRINTF( ("shost : %02X:%02X:%02X:%02X:%02X:%02X\n", eth->ether_shost[0], eth->ether_shost[1],
                       eth->ether_shost[2], eth->ether_shost[3], eth->ether_shost[4], eth->ether_shost[5]) );

    /* IP Header */
    ip = (ipv4_hdr_t *)(buf + sizeof(whd_ether_header_t) );

    memcpy(ip->dst_ip, &pkt->dstip, CYLPA_IPV4_ADDR_LEN);
    memcpy(ip->src_ip, &pkt->srcip, CYLPA_IPV4_ADDR_LEN);
    ip->version_ihl = (CYLPA_IP_VER_4 << CYLPA_IPV4_VER_SHIFT) | (CYLPA_IPV4_MIN_HEADER_LEN / 4);
    ip->tos         = 0;
    ip->id          = hton16(1);
    ip->frag        = 0;
    ip->ttl         = 225;
    ip->prot        = CYLPA_IP_PROTO_UDP;

    /* UDP Header */
    udp = (udp_hdr_t *)(buf + sizeof(whd_ether_header_t) + sizeof(ipv4_hdr_t) );
    udp->src_port = hton16(pkt->srcport);
    udp->dst_port = hton16(pkt->dstport);
    udp->chksum   = 0x0000;
    udp->len      = hton16(CYLPA_UDP_HLEN + pkt->payload_len);

    if ( pkt->payload_len )
    {
        uint8_t *payload = buf + NAT_KEEPALIVE_FIXED_LEN;
        memcpy(payload, pkt->payload, pkt->payload_len);
    }

    /* Calculate UDP checksum */
    udp->chksum   = hton16(cylpa_udp_cksum(CYLPA_UDP_HLEN + pkt->payload_len, (uint8_t*)&pkt->srcip, (uint8_t*)udp));

    /* Calculate IP checksum */
    ip->tot_len     = hton16(sizeof(ipv4_hdr_t) + CYLPA_UDP_HLEN + pkt->payload_len);
    ip->hdr_chksum  = cylpa_ipv4_hdr_cksum( (uint8_t *)ip, CYLPA_IPV4_HLEN(ip) );

    return RESULT_OK;
}

/*******************************************************************************
* Function Name: cylpa_nko_ol_disable_nat_keep_alive
****************************************************************************//**
*
* \param ctxt
* Pointer to nko_ol_t structure.
*
* Disable NAT KeepAlive offload
*******************************************************************************/
static int32_t cylpa_nko_ol_disable_nat_keep_alive(nko_ol_t *ctxt)
{
    whd_keep_alive_t *pkt;
    uint8_t buf[sizeof(whd_keep_alive_t) + 10];
    whd_result_t  reslt;

    memset(buf, 0, sizeof(buf));

    pkt = (whd_keep_alive_t*)buf;
    pkt->data = buf + sizeof(whd_keep_alive_t);

    /* set interval 0 for disabling */
    pkt->period_msec = 0;
    pkt->len_bytes = 1;

    reslt = whd_wifi_keepalive_config( ctxt->whd, pkt, WHD_KEEPALIVE_NAT );
    if ( WHD_SUCCESS != reslt )
    {
        OL_LOG_NKO(LOG_OLA_LVL_ERR, "NKO: Failed to Disable NAT Keepalive\n");
    }
    return ((WHD_SUCCESS != reslt) ? RESULT_ERROR : RESULT_OK);
}

/*******************************************************************************
* Function Name: cylpa_configure_nat_keepalive
****************************************************************************//**
*
* \param ctxt
* Pointer to nko_ol_t structure.
*
* \param cfg
* Pointer to configuration.
*
* Prepare the NAT KeepAlive packet and configure the radio
*******************************************************************************/
static int32_t cylpa_configure_nat_keepalive(nko_ol_t *ctxt, cy_lpa_nko_ol_cfg_t *cfg)
{
    int32_t result;
    nat_keepalive_t pkt;
    uint8_t pkt_buf[NAT_KEEPALIVE_MAX_LEN];
    whd_keep_alive_t keep_alive;

    /* Retrieve the details required for packet creation */
    result = cylpa_nko_get_details(ctxt, &pkt, cfg);
    if ( result != RESULT_OK )
    {
        OL_LOG_NKO(LOG_OLA_LVL_ERR, "NKO: Unable to get NAT packet details\n");
        return result;
    }

    /* Prepare the keepalive packet */
    result = cylpa_nko_prep_packet(ctxt, &pkt, pkt_buf);
    if ( result != RESULT_OK )
    {
        OL_LOG_NKO(LOG_OLA_LVL_ERR, "NKO: Unable to prepare NAT packet\n");
        return result;
    }

    keep_alive.period_msec = cfg->interval;
    keep_alive.data        = pkt_buf;
    keep_alive.len_bytes   = NAT_KEEPALIVE_FIXED_LEN + pkt.payload_len;

    result = whd_wifi_keepalive_config( ctxt->whd, &keep_alive, WHD_KEEPALIVE_NAT );
    if ( WHD_SUCCESS != result )
    {
        OL_LOG_NKO(LOG_OLA_LVL_ERR, "NKO: Failed to Enable NAT Keepalive!\n");
        return result;
    }
    else
    {
        OL_LOG_NKO(LOG_OLA_LVL_INFO, "NKO: NAT Keepalive Enabled!\n");
    }
    return RESULT_OK;
}

/*******************************************************************************
* Function Name: cylpa_nko_ol_nw_ip_change_callback
****************************************************************************//**
*
* \param iface
* Opaque pointer use for NetworkStack.
*
* \param arg
* Pointer to nko_ol_t structure.
*
* Configures the NAT Keepalive to Radio
*******************************************************************************/
static void cylpa_nko_ol_nw_ip_change_callback(cy_nw_ip_interface_t iface, void *arg)
{
    nko_ol_t *ctxt = (nko_ol_t *)arg;
    if ( ctxt != NULL )
    {
        cylpa_configure_nat_keepalive(ctxt, &cy_nko_ol_cfg);
    }
}

/*******************************************************************************
* Function Name: cylpa_nko_is_valid_config
****************************************************************************//**
*
* \param cfg
* NAT KeepAlive config
*
* Checks the validity of the Configuration
*******************************************************************************/
static int32_t cylpa_nko_is_valid_config(cy_lpa_nko_ol_cfg_t *cfg)
{
    if ( (cfg != NULL) && (cfg->d_port > 0) && (cfg->s_port > 0) )
    {
        return RESULT_OK;
    }
    return RESULT_ERROR;
}


/*******************************************************************************
 * Function Name: cylpa_nko_ol_init
 ****************************************************************************//**
 *
 * NAT Keepalive offload init function.
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
static int cylpa_nko_ol_init(void *ol, ol_info_t *info, const void *cfg)
{
    int32_t res;
    nko_ol_t *ctxt = (nko_ol_t *)ol;

    OL_LOG_NKO(LOG_OLA_LVL_DEBUG, "%s\n", __func__);

    memset(ctxt, 0, sizeof(nko_ol_t));

    if ( cfg == NULL )
    {
        OL_LOG_NKO(LOG_OLA_LVL_ERR, "NKO: NAT Keep Alive offload not configured!\n");
        return RESULT_OK;
    }

    ctxt->cfg           = (cy_lpa_nko_ol_cfg_t *)cfg;
    ctxt->whd           = info->whd;
    ctxt->ol_info_ptr   = info;

    if ( !CY_NKO_SUPPORTED(ctxt) )
    {
        OL_LOG_ARP(LOG_OLA_LVL_INFO, "NKO: NAT offload is not supported\n");
        return RESULT_OK;
    }

    res = cylpa_nko_is_valid_config(ctxt->cfg);
    if ( res != RESULT_OK )
    {
        OL_LOG_NKO(LOG_OLA_LVL_ERR, "NKO: Invalid NAT Keepalive Configuration\n");
        return res;
    }

    memcpy(&cy_nko_ol_cfg, ctxt->cfg, sizeof(cy_nko_ol_cfg));

    /* Register for IP change */
    cylpa_nw_ip_initialize_status_change_callback(&cy_nko_ol_cb, cylpa_nko_ol_nw_ip_change_callback, ctxt);
    cylpa_nw_ip_register_status_change_callback((uintptr_t)ctxt->ol_info_ptr->ip, &cy_nko_ol_cb);

    nko_ctx = ctxt;

    return RESULT_OK;
}

/*******************************************************************************
 * Function Name: cylpa_nko_ol_deinit
 ****************************************************************************//**
 *
 * Remove NAT keep-alive offload
 *
 * \param ol
 * The pointer to the ol structure.
 *
 ********************************************************************************/
static void cylpa_nko_ol_deinit(void *ol)
{
    nko_ol_t *ctxt = (nko_ol_t *)ol;

    if ( (ctxt == NULL) || (ctxt->whd == NULL) )
    {
        OL_LOG_NKO(LOG_OLA_LVL_ERR, "%s : Bad Args!\n", __func__);
        return;
    }

    if ( !CY_NKO_SUPPORTED(ctxt) )
    {
        return;
    }

    /* Deregister IP change callback */
    cylpa_nw_ip_unregister_status_change_callback((uintptr_t)ctxt->ol_info_ptr->ip, &cy_nko_ol_cb);

    /* Disable NAT Keep Alive */
    cylpa_nko_ol_disable_nat_keep_alive(ctxt);

    nko_ctx =  NULL;
}

/*******************************************************************************
 * Function Name: cylpa_nko_ol_pm
 ****************************************************************************//**
 *
 * NAT keep-alive offload Power Management notification callback
 *
 * \param ol
 * The pointer to the ol structure.
 *
 * \param st
 * see \ref ol_pm_st_t.
 *
 * Note : NAT KeepAlive will be send automatically irrespective of the power state
 *        Hence this function will not do anything.
 ********************************************************************************/
static void cylpa_nko_ol_pm(void *ol, ol_pm_st_t st)
{
    return;
}

/*******************************************************************************
 * Function Name: cylpa_nko_ol_update_config
 ****************************************************************************//**
 *
 * Update NAT Keep Alive Config.
 *
 * \param cfg
 * The pointer to the Keep Alive configuration.
 *
 * \return
 * Returns RESULT_OK if configuration applied successfully. Error, otherwise
 *
 ********************************************************************************/
int32_t cylpa_nko_ol_update_config(cy_lpa_nko_ol_cfg_t *cfg)
{
    int32_t res = RESULT_OK;

    if ( (nko_ctx == NULL) || (nko_ctx->whd == NULL) || (cfg == NULL) )
    {
        OL_LOG_NKO(LOG_OLA_LVL_ERR, "%s : Bad Args!\n", __func__);
        return RESULT_ERROR;;
    }

    if ( !CY_NKO_SUPPORTED(nko_ctx) )
    {
        OL_LOG_NKO(LOG_OLA_LVL_WARNING, "NKO: : Offload not supported!\n");
        return RESULT_NOT_IMPLEMENTED;
    }

    res = cylpa_nko_is_valid_config(cfg);
    if ( res != RESULT_OK )
    {
        OL_LOG_NKO(LOG_OLA_LVL_ERR, "NKO: Invalid NAT Keepalive Configuration\n");
        return res;
    }

    memcpy(&cy_nko_ol_cfg, cfg, sizeof(cy_nko_ol_cfg));

    res = cylpa_configure_nat_keepalive(nko_ctx, &cy_nko_ol_cfg);
    if ( res != RESULT_OK )
    {
        OL_LOG_NKO(LOG_OLA_LVL_ERR, "NKO: Unable to configure NAT keep alive\n");
    }
    return res;
}

#ifdef __cplusplus
}
#endif

