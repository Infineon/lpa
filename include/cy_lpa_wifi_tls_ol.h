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
* @file cy_lpa_wifi_tls_ol.h
* @brief Defines the API into MQTT TLS offloads from personality.
*/

#ifndef MQTT_TLS_OL_H__
#define MQTT_TLS_OL_H__  (1)

#include <stdint.h>
#include "cy_lpa_compat.h"
#include "cy_lpa_wifi_ol.h"
#include "whd.h"
#include "whd_wlioctl.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CYCFG_WIFI_MQTT_OL_SUPPORT
#define MAX_TLSOE MAX_TLS_CONN
#else
#define MAX_TLSOE 1
#endif

#define MAX_PAYLOAD_LEN 2
#define MAX_WAKEUP_PATTERN_LEN 128

/** Parameters needed for enabling offload and configurable by the user */
typedef struct cy_tlsoe_connect
{
    uint16_t    interval;           /**< Interval (in seconds) between keepalives */
    uint16_t    local_port;         /**< Local port num for this socket */
    uint16_t    remote_port;        /**< Remote port num for this socket */
    char        remote_ip[16];      /**< IP address of remote side */
    uint16_t    data_len;           /**< MQTT ping request data length */
    char        data[MAX_PAYLOAD_LEN];      /**< MQTT ping request data */
    void*       tls_socket;         /**< Socket used for the tls connection */
    uint16_t    patt_offset;                  /**< MQTT wakeup request data pattern offset */
    uint16_t    patt_len;                     /**< MQTT wakeup request data length */
    char        wakepatt[MAX_WAKEUP_PATTERN_LEN]; /**< MQTT wakeup request data */
} cy_tlsoe_ol_connect_t;

/** User configurations set in device configurator */
typedef struct cy_tlsoe_cfg
{
    cy_tlsoe_ol_connect_t ports[MAX_TLSOE]; /**< Port and IP address for each connection */
} cy_tlsoe_ol_cfg_t;

/** Keep pointers to config space, system handle, etc */
typedef struct tlsoe
{
    cy_tlsoe_ol_cfg_t  *cfg;   /**< Pointer to config space */
    void               *whd;   /**< Pointer to system handle */
} tlsoe_ol_t;

extern const ol_fns_t tlsoe_ol_fns;

/******************************************************
 *                      Function Prototypes
 ******************************************************/
 /** @addtogroup lpautilities LPA Utilities API
 * \{
 *
 *
 */

/**
 * Update TLS configuration parameters and socket handle to LPA TLS database.
 *
 * @param[in] index   Index of MQTT connection.
 * @param[in] cfg     Pointer to the structure containing the TLS configuration parameters.
 * @param[in] socket  Pointer to the socket handle used for MQTT connection.
 *
 * @return 0 on success; negative on failure.
 *
 */
int cylpa_tlsoe_ol_update_config(int index, cy_tlsoe_ol_cfg_t *cfg, void* socket);

/**
 * configure the TLS connection parameters like local and peer port, peer IP and keepalive interval.
 *
 * @param[in] index       Index of MQTT connection.
 * @param[in] local_port  Source port number.
 * @param[in] remote_port Destination port number.
 * @param[in] remote_ip   Pointer to the destination IP address.
 * @param[in] interval    Keep alive connection interval.
 *
 * @return 0 on success; negative on failure.
 *
 */
int cylpa_tlsoe_ol_set_socket_config(int index, uint16_t local_port, uint16_t remote_port, uint32_t* remote_ip, uint16_t interval);
/** \} */
#ifdef __cplusplus
}
#endif

#endif /* !MQTT_TLS_OL_H__ */
