# Low Power Assistant Middleware(LPA) Library

## Overview

The LPA middleware provides an easy way to make the low-power features available to developers in the form of a portable configuration layer. The LPA library functions are only used by Wi-Fi. LPA provides features for MCU Low Power, Wi-Fi Low Power and Bluetooth Low Power but the LPA library only needs to be included in applications that use Wi-Fi low power.

The LPA middleware is essentailly an WiFi offload manager which is instantiated when WiFi-Connection-Manager initialization is done.
The offload manager gets created and manages the offloads based on the configured offloads using CUSTOM DESIGN MODUS via device configurator created personality in PDL. The PDL personality consists of Pin Configuration for CYBSP_WIFI_HOST_WAKE, CYBSP_WIFI_DEVICE_WAKE, CYBSP_BT_HOST_WAKE and CYBSP_BT_DEVICE_WAKE. The saved design.modus generates the sources (cycfg_connectivity_wifi.h, cycfg_connectivity_wifi.c) which are compiled and linked in the executable for configured Wi-Fi Offloads.


LPA middleware consists of the following components:

* Configurator tool (using a personality), which makes the low-power features of the system easy to use [ModusToolbox Device Configurator Tool Guide](https://www.infineon.com/dgdl/Infineon-ModusToolbox_Device_Configurator_4.0_User_Guide-UserManual-v01_00-EN.pdf?fileId=8ac78c8c8386267f0183a960bd41598f&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-files&redirId=180683&redirId=VL144). This personality writes data structures and once the configuration is saved it generates cycfg_connectivity_wifi.c and cycfg_connectivity_wifi.h files based on the choices made in the personality.
* The above generated sources (cycfg_connectivity_wifi.h, cycfg_connectivity_wifi.c) are compiled and linked in the executable. The API in the generated source will be invoked at system initialization.
* The LPA configuration is applied from host PSOC6 MCU middleware s/w to IoT wifi-bt Firmware during initialization such as BT low power , ARPOL (ARP Offload) and some of the features such as PFOL(Packet Filter Offload) are applied when host WiFi connection is established with WiFi Access point. TKOL ( TCP Keep-alive offload ) is applied when an TCP connection is established with a remote TCP server.

## Features
* MCU Low Power
* Wi-Fi and Bluetooth Low Power
* Wi-Fi Address Resolution Protocol (ARP) Offload
* Wi-Fi Packet Filter Offload
* Wi-Fi TCP Keepalive Offload

For More Release specific information, refer to [RELEASE.md](./RELEASE.md)

## Requirements
- [ModusToolbox™ software](https://www.infineon.com/cms/en/design-support/tools/sdk/modustoolbox-software/) v3.1
- Programming Language: C

## Supported Platforms
This library and it's features are supported on following Infineon platforms:

* [PSoC 6 Wi-Fi BT Prototyping Kit](https://www.infineon.com/cms/en/product/evaluation-boards/cy8cproto-062-4343w/) (CY8CPROTO-062-4343W) 
* [PSoC 62S2 Wi-Fi BT Pioneer Kit](https://www.infineon.com/cms/en/product/evaluation-boards/cy8ckit-062s2-43012/) (CY8CKIT-062S2-43012)
* [PSoC 6 Wi-Fi BT Pioneer Kit](https://www.infineon.com/cms/en/product/evaluation-boards/cy8ckit-062-wifi-bt/) (CY8CKIT-062-WIFI-BT)
* [CY8CEVAL-062S2](https://www.infineon.com/cms/en/product/evaluation-boards/cy8ceval-062s2/) + [Sterling LWB5Plus](https://www.mouser.com/new/laird-connectivity/laird-connectivity-sterling-lwb5plus) (CY8CEVAL-062S2-LAI-4373M2)
* [CY8CEVAL-062S2](https://www.infineon.com/cms/en/product/evaluation-boards/cy8ceval-062s2/) + [1YN M.2 Module](https://www.embeddedartists.com/products/1yn-m-2-module) (CY8CEVAL-062S2-MUR-43439M2)

## Limitations
Refer to the [Low Power Assistant Middleware Library Limitations](https://infineon.github.io/lpa/lpa_api_reference_manual/html/index.html).

## Dependent libraries
This section provides the list of dependent libraries required for this middleware library.

* [wifi-core-freertos-lwip-mbedtls](https://github.com/Infineon/wifi-core-freertos-lwip-mbedtls): To use LPA library, the application should pull the wifi-core-freertos-lwip-mbedtls library which will internally pull wifi-connection-manager, FreeRTOS, lwIP, mbed TLS, and other dependent modules.

## Quick Start
The LPA could be configured by the ModusToolbox MCU, WiFi and BT personalities. Refer to the [Low Power Assistant Middleware Library Configuration Considerations](https://infineon.github.io/lpa/lpa_api_reference_manual/html/index.html).

##### FreeRTOS
Refer to [wifi-core-freertos-lwip-mbedtls README](https://github.com/Infineon/wifi-core-freertos-lwip-mbedtls/blob/master/README.md#quick-start) Quick start section for basic makefile changes required .

MCU Deep Sleep Functionality is enabled by default in FreeRTOSConfig.h as below 

```
#include <cycfg_system.h>
#if (CY_CFG_PWR_SYS_IDLE_MODE == CY_CFG_PWR_MODE_SLEEP) || (CY_CFG_PWR_SYS_IDLE_MODE == CY_CFG_PWR_MODE_DEEPSLEEP)
extern void vApplicationSleep( uint32_t xExpectedIdleTime );
#define portSUPPRESS_TICKS_AND_SLEEP( xIdleTime ) vApplicationSleep( xIdleTime )
#define configUSE_TICKLESS_IDLE  2
#endif

/* Deep Sleep Latency Configuration */
#if CY_CFG_PWR_DEEPSLEEP_LATENCY > 0
#define configEXPECTED_IDLE_TIME_BEFORE_SLEEP   CY_CFG_PWR_DEEPSLEEP_LATENCY
#endif
```

## Debugging
For Debugging purposes, the application may enable debug, log and error log messages by updating ol_log_level[] with LOG_OLA_LVL_DEBUG.
```
eg:
ol_log_level[LOG_OLA_OLM] = LOG_OLA_LVL_DEBUG;
ol_log_level[LOG_OLA_ARP] = LOG_OLA_LVL_DEBUG;
ol_log_level[LOG_OLA_PF]  = LOG_OLA_LVL_DEBUG;
ol_log_level[LOG_OLA_TKO] = LOG_OLA_LVL_DEBUG;
```

## More information
The following resources contain more information:
* [LPA Middleware RELEASE.md](./RELEASE.md)
* [Low Power Assistant Middleware Library](https://infineon.github.io/lpa/lpa_api_reference_manual/html/index.html)
* [ModusToolbox Software Environment, Quick Start Guide, Documentation, and Videos](https://www.infineon.com/cms/en/design-support/tools/sdk/modustoolbox-software/)
* [LPA Middleware Code Example for FREERTOS](https://github.com/Infineon/mtb-example-anycloud-wlan-lowpower)
* [ModusToolbox Device Configurator Tool Guide](https://www.infineon.com/dgdl/Infineon-ModusToolbox_Device_Configurator_4.0_User_Guide-UserManual-v01_00-EN.pdf?fileId=8ac78c8c8386267f0183a960bd41598f&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-files&redirId=180683&redirId=VL144)
* [PSoC 6 Technical Reference Manual](https://www.infineon.com/dgdl/Infineon-PSoC_6_MCU_PSoC_63_with_BLE_Architecture_Technical_Reference_Manual-AdditionalTechnicalInformation-v11_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0f946fea01ca&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-technical_reference_manual&redirId=TRM148)
* [PSoC 63 with BLE Datasheet Programmable System-on-Chip datasheet](https://www.infineon.com/dgdl/Infineon-PSoC_6_MCU_PSoC_63_with_BLE_Datasheet_Programmable_System-on-Chip_(PSoC)-DataSheet-v16_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0ee4efe46c37&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-datasheet&redirId=VL4079)

------

All other trademarks or registered trademarks referenced herein are the property of their respective owners.

-------------------------------------------------------------------------------

© 2021, Cypress Semiconductor Corporation (an Infineon company) or an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
This software, associated documentation and materials ("Software") is owned by Cypress Semiconductor Corporation or one of its affiliates ("Cypress") and is protected by and subject to worldwide patent protection (United States and foreign), United States copyright laws and international treaty provisions. Therefore, you may use this Software only as provided in the license agreement accompanying the software package from which you obtained this Software ("EULA"). If no EULA applies, then any reproduction, modification, translation, compilation, or representation of this Software is prohibited without the express written permission of Cypress.
Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress reserves the right to make changes to the Software without notice. Cypress does not assume any liability arising out of the application or use of the Software or any product or circuit described in the Software. Cypress does not authorize its products for use in any products where a malfunction or failure of the Cypress product may reasonably be expected to result in significant property damage, injury or death ("High Risk Product"). By including Cypress's product in a High Risk Product, the manufacturer of such system or application assumes all risk of such use and in doing so agrees to indemnify Cypress against all liability.
