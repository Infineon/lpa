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
* @file cy_lpa_wifi_wowlpf_ol.h
* @brief Defines the API into PktFilter offloads from personality.
*/

#ifndef WOWLPF_OL_H__
#define WOWLPF_OL_H__  (1)

#include <stdint.h>
#include "cy_lpa_compat.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* Macro Definition
*******************************************************************************/
#define MAX_PATTERN_LEN       128
#define MAX_MASK_LEN          16
/*******************************************************************************
* LPA Enumerated Types
*******************************************************************************/

/******************************************************************************/
/** \addtogroup group_lpa_enums *//** \{ */
/******************************************************************************/
/** WOWL packet filter type */
typedef enum cy_wowlpf_feature
{
    CY_WOWLPF_OL_FEAT_WAKE       = 0,        /**< wake filter type */
    CY_WOWLPF_OL_FEAT_LAST       = 1,        /**< Number of offload features (invalid feature id). */
} cy_wowlpf_feature_t;

/** \} */


/*******************************************************************************
* LPA Data Structures
*******************************************************************************/

/******************************************************************************/
/** \addtogroup group_lpa_structures *//** \{ */
/******************************************************************************/

/** Structure to describe all WOWL packet filters */
typedef struct cy_wowlpf_ol_cfg
{
    cy_wowlpf_feature_t feature;        /**< Type of filter */
    char     pattern[MAX_PATTERN_LEN];  /**< pattern 1-128 bytes */
    uint8_t  pattern_size;              /**< Size of pattern */
    char     mask[MAX_MASK_LEN];        /**< 1bit represents 1byte of pattern, so maximum mask value is 128 */
    uint8_t  mask_size;                 /**< Size of mask */
    uint16_t offset;                    /**< maximum offset 1500 bytes. */
    uint8_t id;                         /**< Each filter is given a unique 8 bit identifier. */
} cy_wowlpf_ol_cfg_t;


/** Keep pointers to config space, system handle, etc */
typedef struct wowlpf_ol
{
    cy_wowlpf_ol_cfg_t   *cfg;          /**< Pointer to config space */
    void                 *whd;          /**< Pointer to system handle */
    ol_info_t            *ol_info_ptr;  /**< Offload Manager Info structure  \ref ol_info_t */
} wowlpf_ol_t;

/** \} */

extern const ol_fns_t wowlpf_ol_fns;

#ifdef __cplusplus
}
#endif

#endif /* !WOWLPF_OL_H__ */

