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

