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
* @file cy_lpa_wifi_arp_ol.h
* @brief Low Power Offload ARP Assist.
*/

#ifndef ARP_OL_H__
#define ARP_OL_H__  (1)

#include "cy_lpa_wifi_ol_common.h"
#include "whd.h"
#include "whd_wlioctl.h"

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
* Public definitions
*******************************************************************************/

/******************************************************************************/
/** \addtogroup group_lpa_macros_arp *//** \{ */
/******************************************************************************/

#define ARP_ID                              (0xaaccooddUL)          /**< Unique ID for ARP packet */

#define CY_ARP_OL_HOSTIP_MAX_ENTRIES        ARP_MULTIHOMING_MAX     /**< Maximum Number of Host IP addresses in WLAN Device (8) */

#define CY_ARP_OL_DEFAULT_PEERAGE_VALUE     (1200UL)                /**< Default lifetime of an ARP table entry in seconds (agent only). */

#define CY_ARP_OL_AGENT_ENABLE              ARP_OL_AGENT            /**< Flag to enable the ARP Offload          (0x00000001) */
#define CY_ARP_OL_SNOOP_ENABLE              ARP_OL_SNOOP            /**< Flag to enable Snooping Host IP address (0x00000002) */
#define CY_ARP_OL_HOST_AUTO_REPLY_ENABLE    ARP_OL_HOST_AUTO_REPLY  /**< Flag to enable Host Auto Reply          (0x00000004) */
#define CY_ARP_OL_PEER_AUTO_REPLY_ENABLE    ARP_OL_PEER_AUTO_REPLY  /**< Flag to enable Peer Auto Reply          (0x00000008) */
#define CY_ARP_OL_ALL_FEATURE_ENABLE        (CY_ARP_OL_AGENT_ENABLE | CY_ARP_OL_SNOOP_ENABLE | \
                                             CY_ARP_OL_HOST_AUTO_REPLY_ENABLE | CY_ARP_OL_PEER_AUTO_REPLY_ENABLE)  /**< All ARP OL Flags (0x0000000F) */


typedef uint32_t arp_ol_enable_mask_t;      /**< ARP OL Feature is enabled when bit is set (see CY_ARP_OL_XXXX_ENABLE flags) */

/** \} */

/*******************************************************************************
* LPA Enumerated Types
*******************************************************************************/

/******************************************************************************/
/** \addtogroup group_lpa_enums *//** \{ */
/******************************************************************************/

/** States for configuration to use */
typedef enum
{
    ARP_OL_STATE_UNINITIALIZED = 0,     /**< ARP OL is Uninitialized         */
    ARP_OL_STATE_AWAKE,                 /**< ARP OL is set for AWAKE state   */
    ARP_OL_STATE_GOING_TO_SLEEP         /**< ARP OL is set for ASLEEP state  */
} arp_ol_config_state_t;

/** \} */


/*******************************************************************************
* LPA Data Structures
*******************************************************************************/

/******************************************************************************/
/** \addtogroup group_lpa_structures *//** \{ */
/******************************************************************************/

/** ARP Offload configuration.
 * NOTE: There may be multiple configurations - one per BSS interface
 * WWD_STA_INTERFACE, WWD_AP_INTERFACE, WWD_P2P_INTERFACE, WWD_ETHERNET_INTERFACE */
typedef struct arp_ol_cfg_s
{
    arp_ol_enable_mask_t awake_enable_mask; /**< Enable mask for AWAKE state \ref ARP_OL_STATE_AWAKE */
    arp_ol_enable_mask_t sleep_enable_mask; /**< Enable mask for SLEEP state \ref ARP_OL_STATE_GOING_TO_SLEEP */
    uint32_t peerage;                       /**< ARP table entry expiration time in seconds (Device ARP cache) \ref CY_ARP_OL_DEFAULT_PEERAGE_VALUE */
} arp_ol_cfg_t;

/** ARP Offload context - this is a private structure; visible to allow for static definition. */
typedef struct arp_ol_s
{
    char name[4];                               /**< ARP */
    const arp_ol_cfg_t      *config;            /**< pointer to configuration from Configurator \ref arp_ol_cfg_t */
    ol_info_t               *ol_info_ptr;       /**< Offload Manager Info structure  \ref ol_info_t */
    uint32_t ip_address;                        /**< IP Address for this interface */
    arp_ol_config_state_t state;                /**< Currently written state in Device \ref arp_ol_config_state_t */
} arp_ol_t;

/** \} */

extern const ol_fns_t arp_ol_fns;   /**< ARP Offload function table */


#ifdef __cplusplus
}
#endif

#endif     /* !ARP_OL_H__ */

