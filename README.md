# Cypress Low Power Assistant Middleware Library

## Overview

The LPA middleware provides an easy way to make the low-power features of Cypress devices available to developers in the form of a portable configuration layer.
LPA consists of the following components:
* Configurator tool (using a personality), which makes the low-power features of the system easy to use (ModusToolbox Device Configurator Tool Guide). This personality writes data structures that are processed by firmware and implement the choices made in the personality.
* The above mentioned firmware is another component of the LPA feature. The firmware is used at system initialization and does not require user interaction.
* The final component of this feature is a small firmware module provides the integration between the low-power firmware and the IOT framework (most notably the RTOS) used in the system. This final piece of firmware will be part of the IOT framework you are using.

## Features
* MCU Low Power
* Wi-Fi and Bluetooth Low Power
* Wi-Fi Address Resolution Protocol (ARP) Offload
* Wi-Fi Packet Filter Offload
* Wi-Fi TCP Keepalive Offload

## Requirements
- [ModusToolbox™ software](https://www.cypress.com/products/modustoolbox-software-environment) v2.1
- Programming Language: C

## Supported Platforms
This library and it's features are supported on following Cypress platforms:

##### MBEDOS
* [PSoC6 WiFi-BT Prototyping Kit (CY8CPROTO-062-4343W)](https://www.cypress.com/documentation/development-kitsboards/psoc-6-wi-fi-bt-prototyping-kit-cy8cproto-062-4343w)
* [PSoC 62S2 Wi-Fi BT Pioneer Kit (CY8CKIT-062S2-43012)](https://www.cypress.com/documentation/development-kitsboards/psoc-62s2-wi-fi-bt-pioneer-kit-cy8ckit-062s2-43012)
* [PSoC® 6 WiFi-BT Pioneer Kit (CY8CKIT-062-WiFi-BT)](https://www.cypress.com/documentation/development-kitsboards/psoc-6-wifi-bt-pioneer-kit-cy8ckit-062-wifi-bt)

##### FREERTOS
* [PSoC6 WiFi-BT Prototyping Kit (CY8CPROTO-062-4343W)](https://www.cypress.com/documentation/development-kitsboards/psoc-6-wi-fi-bt-prototyping-kit-cy8cproto-062-4343w)
* [PSoC 62S2 Wi-Fi BT Pioneer Kit (CY8CKIT-062S2-43012)](https://www.cypress.com/documentation/development-kitsboards/psoc-62s2-wi-fi-bt-pioneer-kit-cy8ckit-062s2-43012)

## Limitations
Refer to the [API Reference Guide Limitations](https://cypresssemiconductorco.github.io/lpa/lpa_api_reference_manual/html/index.html#group_lpa_p2_cc_limitations).

## Dependent libraries
This Low Power Assistant Middleware library depends on the following:

##### FreeRTOS
* [Wi-Fi Connection Manager](https://github.com/cypresssemiconductorco/wifi-connection-manager)
* [Wi-Fi middleware core](https://github.com/cypresssemiconductorco/wifi-mw-core)
* [Connectivity middleware utilities](https://github.com/cypresssemiconductorco/connectivity-utilities)

##### MBEDOS
* [Connectivity middleware utilities](https://github.com/cypresssemiconductorco/connectivity-utilities)

## Supported Software and Tools
```
ToolChain : GCC_ARM and IAR (FreeRTOS)
            GCC_ARM, IAR and ARMC6 (MBEDOS)
OS        : MBEDOS and FreeRTOS
```
Refer to the [API Reference Guide Supported Software and Tools](https://cypresssemiconductorco.github.io/lpa/lpa_api_reference_manual/html/index.html#section_lpa_toolchain).

## Quick Start
The LPA could be configured by the ModusToolbox MCU, WiFi and BT personalities. Refer to the [API Reference Guide Configuration Considerations](https://cypresssemiconductorco.github.io/lpa/lpa_api_reference_manual/html/index.html#group_lpa_p2_cc).

##### FreeRTOS
Refer to Wi-Fi Middleware-core README for basic makefile changes Required [Wi-Fi middleware core README](https://github.com/cypresssemiconductorco/wifi-mw-core/blob/master/README.md).

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
* [LPA Middleware API Reference Guide](https://cypresssemiconductorco.github.io/lpa/lpa_api_reference_manual/html/index.html)
* [ModusToolbox Software Environment, Quick Start Guide, Documentation, and Videos](https://www.cypress.com/products/modustoolbox-software-environment)
* [LPA Middleware Code Example for MBED OS](https://github.com/cypresssemiconductorco/mbed-os-example-wlan-lowpower)
* [LPA Middleware Code Example for FREERTOS](https://github.com/cypresssemiconductorco/mtb-example-anycloud-wlan-lowpower)
* [ModusToolbox Device Configurator Tool Guide](https://www.cypress.com/ModusToolboxDeviceConfig)
* [PSoC 6 Technical Reference Manual](https://www.cypress.com/documentation/technical-reference-manuals/psoc-6-mcu-psoc-63-ble-architecture-technical-reference)
* [PSoC 63 with BLE Datasheet Programmable System-on-Chip datasheet](http://www.cypress.com/ds218787)
* [Cypress Semiconductor](http://www.cypress.com)

---
© Cypress Semiconductor Corporation, 2019-2020.
