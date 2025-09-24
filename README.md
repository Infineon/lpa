# Low Power Assistant (LPA) middleware library

## Overview

The LPA middleware for Wi-Fi provides an easy way to make the low-power features available to developers in the form of a portable configuration layer. It provides features implementing low power functionality for MCUs, Wi-Fi, and Bluetooth&reg;; however, the LPA library only needs to be included in applications that use low-power Wi-Fi operation.

The LPA middleware is essentially a Wi-Fi offload manager that is instantiated when Wi-Fi Connection Manager (WCM) initialization is done. The offload manager manages the offload configurations which are created using the Device Configurator. The Device Configurator consists of pin configurations for `CYBSP_WIFI_HOST_WAKE`, `CYBSP_WIFI_DEVICE_WAKE`, `CYBSP_BT_HOST_WAKE`, and `CYBSP_BT_DEVICE_WAKE` and the offload configurations.

**Note:** Pin configurations are not applicable for CYW955913EVK-01.

The LPA middleware library consists of the following components:

- Configurator tool (using a personality), which makes the low-power features of the system easy to use. This personality writes data structures; after the configuration is saved, it generates *cycfg_connectivity_wifi.h*,*cycfg_connectivity_wifi.c* for PSoC&trade; 6 and *cycfg_peripherals.h*, *cycfg_peripherals.c* for CYW955913EVK-01 based on the settings made in the Device Configurator. See the [ModusToolbox&trade; Device Configurator tool guide](https://www.infineon.com/dgdl/Infineon-ModusToolbox_Device_Configurator_4.20_User_Guide-UserManual-v01_00-EN.pdf?fileId=8ac78c8c8d2fe47b018e0ea9a6727916&redirId=180683).

- The generated source files are compiled and linked in the executable. The API in the generated source will be invoked at system initialization.

## Features

- MCU low power
- Wi-Fi and Bluetooth&reg; low power
- Wi-Fi Address Resolution Protocol (ARP) offload
- Wi-Fi packet filter offload
- Wi-Fi TCP keepalive offload
- DHCP Lease Time Renew Offload
- ICMP Offload
- Neighbor Discovery Offload
- NULL Keppalive Offload
- NAT Keepalive Offload
- Wake on Wireless LAN
- MQTT Keepalive Offload
- Bluetooth&reg; Low Power

For more release-specific information, see [RELEASE.md](./RELEASE.md)

## Requirements

- [ModusToolbox&trade; software](https://www.infineon.com/modustoolbox) v3.2
- Programming Language: C

## Supported platforms

This library and its features are supported on the following Infineon platforms:

- [PSoC&trade; 6 Wi-Fi Bluetooth&reg; Prototyping Kit](https://www.infineon.com/cms/en/product/evaluation-boards/cy8cproto-062-4343w/) (CY8CPROTO-062-4343W)

- [PSoC&trade; 62S2 Wi-Fi Bluetooth&reg; Pioneer Kit](https://www.infineon.com/cms/en/product/evaluation-boards/cy8ckit-062s2-43012/) (CY8CKIT-062S2-43012)

- [PSoC&trade; 6 Wi-Fi Bluetooth&reg; Pioneer Kit](https://www.infineon.com/cms/en/product/evaluation-boards/cy8ckit-062-wifi-bt/) (CY8CKIT-062-WIFI-BT)

- [CY8CEVAL-062S2](https://www.infineon.com/cms/en/product/evaluation-boards/cy8ceval-062s2/) + [Sterling LWB5Plus](https://www.mouser.com/new/laird-connectivity/laird-connectivity-sterling-lwb5plus) (CY8CEVAL-062S2-LAI-4373M2)

- [CY8CEVAL-062S2](https://www.infineon.com/cms/en/product/evaluation-boards/cy8ceval-062s2/) + [1YN M.2 module](https://www.embeddedartists.com/products/1yn-m-2-module) (CY8CEVAL-062S2-MUR-43439M2)

- [PSoC&trade; 62S2 evaluation kit (CY8CEVAL-062S2-CYW43022CUB)](https://www.infineon.com/cms/en/product/evaluation-boards/cy8ceval-062s2/)

- [CYW955913EVK-01 Wi-Fi Bluetooth&reg; Prototyping Kit (CYW955913EVK-01)](https://www.infineon.com/CYW955913EVK-01)

- [PSoC&trade; 62S2 evaluation kit (CY8CEVAL-062S2-CYW955513SDM2WLIPA)](https://www.infineon.com/cms/en/product/evaluation-boards/cy8ceval-062s2/)

- PSOC&trade; Edge E84 Evaluation Kit

## Limitations

See the [Low Power Assistant Middleware Library Limitations](https://infineon.github.io/lpa/api_reference_manual/html/index.html).

## Dependent libraries

- [wifi-core-freertos-lwip-mbedtls](https://github.com/Infineon/wifi-core-freertos-lwip-mbedtls): To use the LPA library with FreeRTOS, the application should pull the wifi-core-freertos-lwip-mbedtls library which will internally pull wifi-connection-manager, FreeRTOS, lwIP, mbed TLS, and other dependent modules.

- [wifi-core-threadx-cat5](https://github.com/Infineon/wifi-core-threadx-cat5): To use the LPA library with ThreadX (Currently supported only on CYW955913EVK-01), the application should pull the wifi-core-threadx-cat5 library which will internally pull wifi-connection-manager and other dependent modules.

## Quick start

The LPA could be configured using the ModusToolbox&trade; MCU, Wi-Fi, and Bluetooth&reg; personalities. See the [Low Power Assistant middleware library configuration considerations](https://infineon.github.io/lpa/api_reference_manual/html/index.html).


### FreeRTOS

See [wifi-core-freertos-lwip-mbedtls README](https://github.com/Infineon/wifi-core-freertos-lwip-mbedtls/blob/master/README.md#quick-start) Quick start section for basic Makefile changes required.

MCU Deep Sleep Functionality is enabled by default in *FreeRTOSConfig.h* as follows:

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

**Note:** Currently LPA only supports wifi-core-freertos-lwip-mbedtls version latest-v1.X.

### ThreadX

Currently ThreadX is supported only on CYW955913EVK-01.

See [wifi-core-threadx-cat5 README](https://github.com/Infineon/wifi-core-threadx-cat5/blob/master/README.md#quick-start) Quick start section for basic Makefile changes required .

MCU Deep Sleep Functionality is enabled by default in CYW955913EVK-01

## Debugging

For Debugging, the application may enable debug, log, and error log messages by updating `ol_log_level[]` with `LOG_OLA_LVL_DEBUG`.

```
ol_log_level[LOG_OLA_OLM] = LOG_OLA_LVL_DEBUG;
ol_log_level[LOG_OLA_ARP] = LOG_OLA_LVL_DEBUG;
ol_log_level[LOG_OLA_PF]  = LOG_OLA_LVL_DEBUG;
ol_log_level[LOG_OLA_TKO] = LOG_OLA_LVL_DEBUG;
```

## More information

The following resources contain more information:
- [LPA middleware RELEASE.md](./RELEASE.md)
- [Low Power Assistant middleware library](https://infineon.github.io/lpa/api_reference_manual/html/index.html)
- [ModusToolbox&trade; software environment, quick start guide, documentation, and videos](https://www.infineon.com/cms/en/design-support/tools/sdk/modustoolbox-software/?redirId=178597)
- [LPA middleware code example for FreeRTOS](https://github.com/Infineon/mtb-example-wifi-wlan-lowpower)
- [ModusToolbox&trade; Device Configurator tool guide](https://www.infineon.com/dgdl/Infineon-ModusToolbox_Device_Configurator_4.20_User_Guide-UserManual-v01_00-EN.pdf?fileId=8ac78c8c8d2fe47b018e0ea9a6727916&redirId=180683)
- [PSoC&trade; 6 technical reference manual](https://www.infineon.com/dgdl/Infineon-PSoC_6_MCU_PSoC_63_with_BLE_Architecture_Technical_Reference_Manual-AdditionalTechnicalInformation-v11_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0f946fea01ca&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-technical_reference_manual&redirId=TRM148)
- [PSoC&trade; 63 with Bluetooth&reg; datasheet](https://www.infineon.com/dgdl/Infineon-PSoC_6_MCU_PSoC_63_with_BLE_Datasheet_Programmable_System-on-Chip_(PSoC)-DataSheet-v16_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0ee4efe46c37&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-datasheet&redirId=VL4079)

-------

All other trademarks or registered trademarks referenced herein are the property of their respective owners.

The Bluetooth&reg; word mark and logos are registered trademarks owned by Bluetooth SIG, Inc., and any use of such marks by Infineon is under license.


-------------------------------------------------------------------------------
