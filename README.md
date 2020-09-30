# Low Power Assistant Middleware(LPA) Library

## Overview

The LPA middleware provides an easy way to make the low-power features available to developers in the form of a portable configuration layer.
LPA middleware consists of the following components:
* Configurator tool (using a personality), which makes the low-power features of the system easy to use (ModusToolbox Device Configurator Tool Guide here  https://www.cypress.com/file/504376/download ). This personality writes data structures and once the configuration is saved it generates cycfg_connectivity_wifi.c and cycfg_connectivity_wifi.h files based on the choices made in the personality.
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
- [ModusToolbox™ software](https://www.cypress.com/products/modustoolbox-software-environment) v2.2
- Programming Language: C

## Supported Platforms
This library and it's features are supported on following Cypress platforms:

##### MBEDOS
* [PSoC 6 Wi-Fi BT Prototyping Kit](https://www.cypress.com/CY8CPROTO-062-4343W) (CY8CPROTO-062-4343W) 
* [PSoC 62S2 Wi-Fi BT Pioneer Kit](https://www.cypress.com/CY8CKIT-062S2-43012) (CY8CKIT-062S2-43012)
* [PSoC 6 Wi-Fi BT Pioneer Kit](https://www.cypress.com/CY8CKIT-062-WiFi-BT) (CY8CKIT-062-WIFI-BT)

##### FREERTOS
* [PSoC 6 Wi-Fi BT Prototyping Kit](https://www.cypress.com/CY8CPROTO-062-4343W) (CY8CPROTO-062-4343W) 
* [PSoC 62S2 Wi-Fi BT Pioneer Kit](https://www.cypress.com/CY8CKIT-062S2-43012) (CY8CKIT-062S2-43012)
* [PSoC 6 Wi-Fi BT Pioneer Kit](https://www.cypress.com/CY8CKIT-062-WiFi-BT) (CY8CKIT-062-WIFI-BT)

##### AMAZON FREERTOS
* [PSoC 6 Wi-Fi BT Prototyping Kit](https://www.cypress.com/CY8CPROTO-062-4343W) (CY8CPROTO-062-4343W)
* [PSoC 62S2 Wi-Fi BT Pioneer Kit](https://www.cypress.com/CY8CKIT-062S2-43012) (CY8CKIT-062S2-43012)
* [PSoC 6 Wi-Fi BT Pioneer Kit](https://www.cypress.com/CY8CKIT-062-WiFi-BT) (CY8CKIT-062-WIFI-BT)

## Limitations
Refer to the [Low Power Assistant Middleware Library Limitations](https://cypresssemiconductorco.github.io/lpa/lpa_api_reference_manual/html/index.html#group_lpa_p2_cc_limitations).

## Dependent libraries
This section provides the list of dependent libraries required for this middleware library.

##### FreeRTOS
* [Wi-Fi Connection Manager](https://github.com/cypresssemiconductorco/wifi-connection-manager)
* [Wi-Fi middleware core](https://github.com/cypresssemiconductorco/wifi-mw-core)
* [Connectivity middleware utilities](https://github.com/cypresssemiconductorco/connectivity-utilities)

##### MBEDOS
* [Connectivity middleware utilities](https://github.com/cypresssemiconductorco/connectivity-utilities)

##### Amazon FreeRTOS
* [Connectivity middleware utilities](https://github.com/cypresssemiconductorco/connectivity-utilities)

## Supported Software and Tools
```
ToolChain : GCC_ARM and IAR (FreeRTOS)
            GCC_ARM and ARMC6 (MBEDOS)
            GCC_ARM, IAR and ARMC6 (Amazon FreeRTOS)
OS        : MBEDOS , FreeRTOS and Amazon FreeRTOS
```
Refer to the [Low Power Assistant Middleware Library Software and Tools](https://cypresssemiconductorco.github.io/lpa/lpa_api_reference_manual/html/index.html#section_lpa_toolchain).

## Quick Start
The LPA could be configured by the ModusToolbox MCU, WiFi and BT personalities. Refer to the [Low Power Assistant Middleware Library Configuration Considerations](https://cypresssemiconductorco.github.io/lpa/lpa_api_reference_manual/html/index.html#group_lpa_p2_cc).

##### FreeRTOS
Refer to [Wi-Fi middleware core README](https://github.com/cypresssemiconductorco/wifi-mw-core/blob/master/README.md) for basic makefile changes required .

MCU Deep Sleep Functionality can be enabled by adding below changes to FreeRTOSConfig.h

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
##### Amazon FreeRTOS
MCU Deep Sleep Functionality can be enabled by adding below changes to FreeRTOSConfig.h
```
#include <cycfg_system.h>
#if defined(__STDC__) || defined(__cplusplus__)
#if (CY_CFG_PWR_SYS_IDLE_MODE == CY_CFG_PWR_MODE_SLEEP) || (CY_CFG_PWR_SYS_IDLE_MODE == CY_CFG_PWR_MODE_DEEPSLEEP)
extern void vApplicationSleep( uint32_t xExpectedIdleTime );  //This is to prevent IAR toolchain Compile error with CMake
#endif
#endif

#if (CY_CFG_PWR_SYS_IDLE_MODE == CY_CFG_PWR_MODE_SLEEP) || (CY_CFG_PWR_SYS_IDLE_MODE == CY_CFG_PWR_MODE_DEEPSLEEP)
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
* [Low Power Assistant Middleware Library](https://cypresssemiconductorco.github.io/lpa/lpa_api_reference_manual/html/index.html)
* [ModusToolbox Software Environment, Quick Start Guide, Documentation, and Videos](https://www.cypress.com/products/modustoolbox-software-environment)
* [LPA Middleware Code Example for MBED OS](https://github.com/cypresssemiconductorco/mbed-os-example-wlan-lowpower)
* [LPA Middleware Code Example for FREERTOS](https://github.com/cypresssemiconductorco/mtb-example-anycloud-wlan-lowpower)
* [LPA Middleware Code Example for AMAZON FREERTOS](https://github.com/cypresssemiconductorco/afr-example-wlan-offloads)
* [ModusToolbox Device Configurator Tool Guide](https://www.cypress.com/ModusToolboxDeviceConfig)
* [PSoC 6 Technical Reference Manual](https://www.cypress.com/documentation/technical-reference-manuals/psoc-6-mcu-psoc-63-ble-architecture-technical-reference)
* [PSoC 63 with BLE Datasheet Programmable System-on-Chip datasheet](http://www.cypress.com/ds218787)

---

------

All other trademarks or registered trademarks referenced herein are the property of their respective owners.

![Banner](images/Banner.png)

-------------------------------------------------------------------------------

© Cypress Semiconductor Corporation, 2020. This document is the property of Cypress Semiconductor Corporation and its subsidiaries ("Cypress"). This document, including any software or firmware included or referenced in this document ("Software"), is owned by Cypress under the intellectual property laws and treaties of the United States and other countries worldwide. Cypress reserves all rights under such laws and treaties and does not, except as specifically stated in this paragraph, grant any license under its patents, copyrights, trademarks, or other intellectual property rights. If the Software is not accompanied by a license agreement and you do not otherwise have a written agreement with Cypress governing the use of the Software, then Cypress hereby grants you a personal, non-exclusive, nontransferable license (without the right to sublicense) (1) under its copyright rights in the Software (a) for Software provided in source code form, to modify and reproduce the Software solely for use with Cypress hardware products, only internally within your organization, and (b) to distribute the Software in binary code form externally to end users (either directly or indirectly through resellers and distributors), solely for use on Cypress hardware product units, and (2) under those claims of Cypress's patents that are infringed by the Software (as provided by Cypress, unmodified) to make, use, distribute, and import the Software solely for use with Cypress hardware products. Any other use, reproduction, modification, translation, or compilation of the Software is prohibited.<br/>
TO THE EXTENT PERMITTED BY APPLICABLE LAW, CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH REGARD TO THIS DOCUMENT OR ANY SOFTWARE OR ACCOMPANYING HARDWARE, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. No computing device can be absolutely secure. Therefore, despite security measures implemented in Cypress hardware or software products, Cypress shall have no liability arising out of any security breach, such as unauthorized access to or use of a Cypress product. CYPRESS DOES NOT REPRESENT, WARRANT, OR GUARANTEE THAT CYPRESS PRODUCTS, OR SYSTEMS CREATED USING CYPRESS PRODUCTS, WILL BE FREE FROM CORRUPTION, ATTACK, VIRUSES, INTERFERENCE, HACKING, DATA LOSS OR THEFT, OR OTHER SECURITY INTRUSION (collectively, "Security Breach"). Cypress disclaims any liability relating to any Security Breach, and you shall and hereby do release Cypress from any claim, damage, or other liability arising from any Security Breach. In addition, the products described in these materials may contain design defects or errors known as errata which may cause the product to deviate from published specifications. To the extent permitted by applicable law, Cypress reserves the right to make changes to this document without further notice. Cypress does not assume any liability arising out of the application or use of any product or circuit described in this document. Any information provided in this document, including any sample design information or programming code, is provided only for reference purposes. It is the responsibility of the user of this document to properly design, program, and test the functionality and safety of any application made of this information and any resulting product. "High-Risk Device" means any device or system whose failure could cause personal injury, death, or property damage. Examples of High-Risk Devices are weapons, nuclear installations, surgical implants, and other medical devices. "Critical Component" means any component of a High-Risk Device whose failure to perform can be reasonably expected to cause, directly or indirectly, the failure of the High-Risk Device, or to affect its safety or effectiveness. Cypress is not liable, in whole or in part, and you shall and hereby do release Cypress from any claim, damage, or other liability arising from any use of a Cypress product as a Critical Component in a High-Risk Device. You shall indemnify and hold Cypress, its directors, officers, employees, agents, affiliates, distributors, and assigns harmless from and against all claims, costs, damages, and expenses, arising out of any claim, including claims for product liability, personal injury or death, or property damage arising from any use of a Cypress product as a Critical Component in a High-Risk Device. Cypress products are not intended or authorized for use as a Critical Component in any High-Risk Device except to the limited extent that (i) Cypress's published data sheet for the product explicitly states Cypress has qualified the product for use in a specific High-Risk Device, or (ii) Cypress has given you advance written authorization to use the product as a Critical Component in the specific High-Risk Device and you have signed a separate indemnification agreement.<br/>
Cypress, the Cypress logo, Spansion, the Spansion logo, and combinations thereof, WICED, PSoC, CapSense, EZ-USB, F-RAM, and Traveo are trademarks or registered trademarks of Cypress in the United States and other countries. For a more complete list of Cypress trademarks, visit cypress.com. Other names and brands may be claimed as property of their respective owners.

