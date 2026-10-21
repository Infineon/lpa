# Low Power Assistant (LPA) middleware library - Wi-Fi, Bluetooth&reg;, and MCU low-power configuration for Infineon devices

# Overview

The Low Power Assistant (LPA) middleware provides an easy, portable configuration layer that makes the low-power features of Infineon MCUs, Wi-Fi, and Bluetooth&reg; radios available to developers. It lets you configure MCU sleep and System Deep Sleep, Wi-Fi WLAN firmware offloads (ARP, packet filters, TCP keepalive, and more), and Bluetooth&reg; low-power wake signaling — reducing host power consumption by keeping the host MCU asleep while the WLAN device handles routine network traffic on its behalf.

LPA is essentially a Wi-Fi offload manager (OLM) that is instantiated when the Wi-Fi Connection Manager (WCM) is initialized. The offload manager applies the offload configurations that you create with the ModusToolbox&trade; Device Configurator. The LPA library only needs to be included in applications that use low-power Wi-Fi operation.

The LPA middleware consists of the following components:

- A Configurator personality that makes the system's low-power features easy to use. This personality writes the data structures; after the configuration is saved, it generates `cycfg_connectivity_wifi.h` / `cycfg_connectivity_wifi.c` for PSOC&trade; 6, and `cycfg_peripherals.h` / `cycfg_peripherals.c` for CYW955913EVK-01, based on the settings made in the Device Configurator.
- The generated source files, which are compiled and linked into the executable and invoked at system initialization.

The Device Configurator manages the pin configurations for `CYBSP_WIFI_HOST_WAKE`, `CYBSP_WIFI_DEVICE_WAKE`, `CYBSP_BT_HOST_WAKE`, and `CYBSP_BT_DEVICE_WAKE`, together with the offload configurations.

**Note:** Pin configurations are not applicable for CYW955913EVK-01.

For complete configuration details, per-offload quick start guides, and power-measurement instructions, see the [Low Power Assistant (LPA) Datasheet](https://infineon.github.io/lpa/api_reference_manual/html/group_lpa_datasheet.html).

# Features

- MCU low power (CPU Sleep and System Deep Sleep)
- Wi-Fi and Bluetooth&reg; low power
- Wi-Fi host-wake signal (out-of-band host wake)
- Wi-Fi Address Resolution Protocol (ARP) offload
- Wi-Fi packet filter offload
- Wi-Fi TCP keepalive offload
- DHCP Lease Time Renew offload (DLTRO)
- ICMP offload
- Neighbor Discovery offload (NDOE)
- NULL keepalive offload
- NAT keepalive offload
- Wake on Wireless LAN (WOWLPF)
- MQTT keepalive offload
- Bluetooth&reg; low power

# When to Use

Use the LPA middleware when you are building an RTOS-based (FreeRTOS or ThreadX) connected application on a supported Infineon device and need to minimize host power consumption. Typical scenarios include:

- Battery-powered or energy-harvesting IoT end nodes that must stay associated with a Wi-Fi network while the host MCU spends most of its time in System Deep Sleep.
- Applications that must remain reachable (ARP, ping/ICMP, neighbor discovery) or keep TCP/MQTT/NAT sessions alive without waking the host for every packet.
- Designs that want the WLAN firmware to filter unwanted broadcast/multicast traffic so the host is woken only for relevant packets.
- Designs that need the WLAN device or Bluetooth&reg; controller to wake the host from low power through a dedicated host-wake pin.

The LPA provides features for MCU, Wi-Fi, and Bluetooth&reg; low power; however, the library only needs to be included in applications that use Wi-Fi low power.

# Prerequisites

## Hardware Requirements

- One of the supported Infineon kits (see [Supported platforms](#supported-platforms)).
- For host-wake / device-wake functionality, the board must route the WLAN and Bluetooth&reg; wake signals to the host through `CYBSP_WIFI_HOST_WAKE`, `CYBSP_WIFI_DEVICE_WAKE`, `CYBSP_BT_HOST_WAKE`, and `CYBSP_BT_DEVICE_WAKE`. These pins are configured in the Device Configurator.

  **Note:** Pin configurations are not applicable for CYW955913EVK-01.
- The CY8CKIT-062S2-43012 kit is recommended when following the power-measurement instructions in the Reference Manual, because those instructions are documented for that kit.

## Software Requirements

This version of the LPA middleware was validated for compatibility with the following software and tools:

| Software and tools                       | Version |
| :---                                     | :---    |
| ModusToolbox&trade; software environment | 3.6     |
| ModusToolbox&trade; Device Configurator  | 5.50    |
| GCC compiler                             | 14.2.1  |
| IAR compiler                             | 9.50.2  |
| Arm&reg; compiler 6                      | 6.22    |
| LLVM compiler                            | 19.1.5  |

Programming language: C

## Supported platforms

This library and its features are supported on the following Infineon device families. For the specific validated kits per release, see [RELEASE.md](https://github.com/Infineon/lpa/blob/master/RELEASE.md).

- PSOC&trade; 6 with CYW4343W / CYW43012 / CYW43439 / CYW43022 Wi-Fi + Bluetooth&reg; connectivity
- PSOC&trade; 6 with CYW955513 connectivity (CY8CEVAL-062S2-CYW955513SDM2WLIPA)
- PSOC&trade; Edge E84 (PSE84)
- CYW955913 (CYW955913EVK-01)

## Read Documentation First

- [Low Power Assistant (LPA) Datasheet](https://infineon.github.io/lpa/api_reference_manual/html/group_lpa_datasheet.html) - Complete LPA configuration guide, per-offload quick start guides, and power-measurement instructions (this is the detailed datasheet page of the LPA Reference Manual)
- [ModusToolbox&trade; Device Configurator tool guide](https://www.infineon.com/dgdl/Infineon-ModusToolbox_Device_Configurator_4.20_User_Guide-UserManual-v01_00-EN.pdf?fileId=8ac78c8c8d2fe47b018e0ea9a6727916&redirId=180683) - How to configure the LPA personalities and generate initialization code
- [PSOC&trade; 6 technical reference manual](https://www.infineon.com/dgdl/Infineon-PSoC_6_MCU_PSoC_63_with_BLE_Architecture_Technical_Reference_Manual-AdditionalTechnicalInformation-v11_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0f946fea01ca&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-technical_reference_manual&redirId=TRM148) - Device architecture and power-management details
- [PSOC&trade; 63 with Bluetooth&reg; datasheet](https://www.infineon.com/dgdl/Infineon-PSoC_6_MCU_PSoC_63_with_BLE_Datasheet_Programmable_System-on-Chip_(PSoC)-DataSheet-v16_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0ee4efe46c37&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-datasheet&redirId=VL4079) - Electrical characteristics and pinout

## Choose Starting Template

Start from an Empty application (always prefer starting from an Empty template):
- PSOC&trade; 6: [mtb-example-psoc6-empty-app](https://github.com/Infineon/mtb-example-psoc6-empty-app)
- CYW955913EVK-01 (ThreadX): [mtb-example-threadx-cyw5591x-low-power](https://github.com/Infineon/mtb-example-threadx-cyw5591x-low-power)

Alternatively, start from one of the [Related Code Examples](#related-code-examples), which already include the required libraries and Device Configurator settings.

## Add Required Libraries

Add the required middleware using the ModusToolbox&trade; Library Manager (available in the ModusToolbox&trade; for VS Code extension and the Eclipse IDE) or by creating the corresponding `.mtb` files in the application's `deps` folder and running `make getlibs`.

- For FreeRTOS-based applications, add [wifi-core-freertos-lwip-mbedtls](https://github.com/Infineon/wifi-core-freertos-lwip-mbedtls). This internally pulls in wifi-connection-manager, FreeRTOS, lwIP, mbed TLS, and the other dependent modules.

  **Note:** LPA currently supports wifi-core-freertos-lwip-mbedtls latest-v1.X only.
- For ThreadX-based applications (currently supported only on CYW955913EVK-01), add [wifi-core-threadx-cat5](https://github.com/Infineon/wifi-core-threadx-cat5). This internally pulls in wifi-connection-manager and the other dependent modules.

The LPA library itself is pulled in as a dependency of these Wi-Fi core libraries; the ModusToolbox&trade; Wi-Fi low-power code examples download it automatically.

## Configure Makefile (if applicable)

MCU Deep Sleep functionality is enabled by default. For FreeRTOS on PSOC&trade; 6, confirm tickless idle is enabled in `FreeRTOSConfig.h`:

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

On CYW955913EVK-01, MCU Deep Sleep is enabled by default and no `FreeRTOSConfig.h` change is required.

## Configure in the Tool (if applicable)

Configure LPA using the ModusToolbox&trade; MCU/Power, Wi-Fi, and Bluetooth&reg; personalities in the Device Configurator. Open it with `make device-configurator` (or from the IDE), enable the required personality, set the offload and wake-pin parameters, and save to generate the initialization code. For the complete per-offload parameter reference, see the [Configuration Reference](#configuration-reference), the [Low Power Assistant (LPA) Datasheet](https://infineon.github.io/lpa/api_reference_manual/html/group_lpa_datasheet.html), and the [ModusToolbox&trade; Device Configurator tool guide](https://www.infineon.com/dgdl/Infineon-ModusToolbox_Device_Configurator_4.20_User_Guide-UserManual-v01_00-EN.pdf?fileId=8ac78c8c8d2fe47b018e0ea9a6727916&redirId=180683).

## Add Headers

LPA is configured primarily through the Device Configurator-generated sources, which the BSP includes automatically; the offload APIs are pulled in through the Wi-Fi core libraries, so no additional header is required for basic use. To control LPA log verbosity from application code, include the offload debug header:

```c
#include "cy_lpa_wifi_ol_debug.h"
```

# Quick Start

This quick start demonstrates the most common use case: enabling MCU and Wi-Fi low power so the host enters System Deep Sleep while connected to a Wi-Fi network and wakes on relevant traffic through the WLAN host-wake signal. Follow these steps to get from an Empty template to a working example:

**Step 1:** Create an Empty application for your kit (for example, mtb-example-psoc6-empty-app for CY8CKIT-062S2-43012), or start from the WLAN low-power code example.

**Step 2:** Add the Wi-Fi core library for your RTOS (see [Add Required Libraries](#add-required-libraries)) using the Library Manager or by adding the `.mtb` files and running `make getlibs`.

**Step 3:** In the Device Configurator, enable the Power personality, set System Idle Power Mode to System Deep Sleep, and enable the Wi-Fi host-wake configuration (set the host device interrupt pin to `CYBSP_WIFI_HOST_WAKE`). Save to generate the initialization code.

**Step 4:** Copy the latest `FreeRTOSConfig.h` into the application root and confirm tickless idle is enabled (see [Configure Makefile](#configure-makefile-if-applicable)).

**Step 5:** Add the following code to your `main.c` (see [Sample Code](#sample-code) for the complete example), then build and program.

**Expected Outcome:** The host connects to the configured Wi-Fi AP and enters System Deep Sleep during idle. Incoming traffic (for example, a ping) asserts the WLAN host-wake line and wakes the host; the offloads keep the connection alive while the host sleeps. Full details in [Expected Outcome](#expected-outcome).

## Sample Code

### FreeRTOS Example (main.c)

This example demonstrates MCU low power in a FreeRTOS application. The system enters System Deep Sleep while idle; the user LED indicates the Active (LED on) versus Idle (LED off) states for power measurement.

```c
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "FreeRTOS.h"
#include "task.h"

#define BLINKING_RATE_MS        5000
static void blinky(void *args)
{
    TickType_t ticks = pdMS_TO_TICKS(BLINKING_RATE_MS);

    /* Initialize the User LED */
    cyhal_gpio_init((cyhal_gpio_t)CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);

    while (true)
    {
        /* Turn ON LED (Active state) */
        cyhal_gpio_write((cyhal_gpio_t)CYBSP_USER_LED, false);
        /* Delay so the MCU stays active and does not enter Deep Sleep */
        cyhal_system_delay_ms(BLINKING_RATE_MS);
        /* Turn OFF LED (Idle state) */
        cyhal_gpio_write((cyhal_gpio_t)CYBSP_USER_LED, true);
        vTaskDelay(ticks);
    }
}

int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }
    __enable_irq();

    xTaskCreate(blinky, "Blinky Task", 1024 * 10, 0, 1, 0);
    /* Start the FreeRTOS scheduler */
    vTaskStartScheduler();
}
```

For the complete Wi-Fi host-wake and per-offload examples (ARP, packet filter, TCP keepalive, and others), see the per-offload quick start guides in the [Low Power Assistant (LPA) Datasheet](https://infineon.github.io/lpa/api_reference_manual/html/group_lpa_datasheet.html) and the [Related Code Examples](#related-code-examples).

### ThreadX Example (CYW955913EVK-01)

On CYW955913EVK-01, MCU low power is enabled by default. Start from [mtb-example-threadx-cyw5591x-low-power](https://github.com/Infineon/mtb-example-threadx-cyw5591x-low-power), then build and program:

```
make getlibs
make build TARGET=CYW955913EVK-01 TOOLCHAIN=GCC_ARM
make program TARGET=CYW955913EVK-01 TOOLCHAIN=GCC_ARM
```

## Expected Outcome

When you successfully run the Quick Start example, you should observe:

Visual/Hardware Behavior:
- The host MCU connects to the configured Wi-Fi AP, then enters System Deep Sleep during idle. On PSOC&trade; 6 kits, the user LED shows the Active (on) versus Idle (off) states.
- Sending traffic (for example, a ping) to the board asserts the WLAN host-wake (out-of-band) line and wakes the host to service the packet, after which it returns to System Deep Sleep.

Terminal Output:
```
Network Stack Suspended, MCU can enter DeepSleep power mode
Resuming Network Stack, Network stack was suspended for 15253ms
...
oob_intrs:72, sdio_intrs:147, error_intrs:0, read_aborts:0
Network Stack Suspended, MCU can enter DeepSleep power mode
```
The `oob_intrs` counter reflects the number of WLAN out-of-band interrupts (host wakes) received.

**Note:** On CY8CEVAL-062S2-CYW43022CUB, ICMP ping is offloaded to the WLAN firmware, so ping traffic does not wake the host. Send TCP/UDP packets to verify host wake.

Performance Metrics:
- With MCU and Wi-Fi low power enabled, the host remains in System Deep Sleep between offloaded events. Refer to the power-measurement instructions in the Reference Manual (documented for CY8CKIT-062S2-43012) to quantify current consumption in the Active versus Deep Sleep states.

# Troubleshooting

- **Host never enters Deep Sleep:** Ensure the System Idle Power Mode is set to System Deep Sleep in the Device Configurator Power personality, and that tickless idle is enabled in `FreeRTOSConfig.h` (PSOC&trade; 6). A task that never blocks (for example, a busy delay) keeps the MCU active by design.
- **Host does not wake on incoming traffic:** Verify the Wi-Fi host-wake configuration and that the host device interrupt pin is set to `CYBSP_WIFI_HOST_WAKE`. Host-wake is required for the WLAN offloads to wake the host.
- **Ping does not wake the host on CY8CEVAL-062S2-CYW43022CUB:** ICMP is offloaded to the WLAN firmware; use TCP/UDP traffic to observe host wake.
- **Wrong Wi-Fi core version:** LPA currently supports wifi-core-freertos-lwip-mbedtls latest-v1.X only.
- **Enable debug logging:** To print offload debug, log, and error messages, set the relevant `ol_log_level[]` entries to `LOG_OLA_LVL_DEBUG`:
  ```c
  ol_log_level[LOG_OLA_OLM] = LOG_OLA_LVL_DEBUG;
  ol_log_level[LOG_OLA_ARP] = LOG_OLA_LVL_DEBUG;
  ol_log_level[LOG_OLA_PF]  = LOG_OLA_LVL_DEBUG;
  ol_log_level[LOG_OLA_TKO] = LOG_OLA_LVL_DEBUG;
  ```

# Related Code Examples

- [mtb-example-wifi-wlan-lowpower](https://github.com/Infineon/mtb-example-wifi-wlan-lowpower) - WLAN low power for FreeRTOS
- [mtb-example-wlan-offloads](https://github.com/Infineon/mtb-example-wlan-offloads) - WLAN TCP keepalive and other offloads for FreeRTOS
- [mtb-example-threadx-cyw5591x-low-power](https://github.com/Infineon/mtb-example-threadx-cyw5591x-low-power) - Low power for CYW955913EVK-01 (ThreadX)

# Configuration Reference

LPA is configured through the ModusToolbox&trade; Device Configurator personalities (MCU/Power, Wi-Fi, and Bluetooth&reg;), which generate the initialization sources that LPA consumes at startup. The complete set of configuration parameters — MCU System Idle Power Mode and Deep Sleep Latency, host-wake pins, and the per-offload parameters for ARP, packet filter, TCP keepalive, DLTRO, ICMP, NDOE, NULL keepalive, NAT keepalive, WOWLPF, and MQTT keepalive — is documented in the [Low Power Assistant (LPA) Datasheet](https://infineon.github.io/lpa/api_reference_manual/html/group_lpa_datasheet.html).

Key generated sources:
- PSOC&trade; 6: `cycfg_connectivity_wifi.c` / `cycfg_connectivity_wifi.h`
- CYW955913EVK-01: `cycfg_peripherals.c` / `cycfg_peripherals.h`

Debug logging is controlled at runtime through the `ol_log_level[]` array (see [Troubleshooting](#troubleshooting)).

For library limitations, see the [Low Power Assistant (LPA) Datasheet](https://infineon.github.io/lpa/api_reference_manual/html/group_lpa_datasheet.html). For tool usage, see the [ModusToolbox&trade; Device Configurator tool guide](https://www.infineon.com/dgdl/Infineon-ModusToolbox_Device_Configurator_4.20_User_Guide-UserManual-v01_00-EN.pdf?fileId=8ac78c8c8d2fe47b018e0ea9a6727916&redirId=180683).

# Release Notes and Changelog

- [RELEASE.md](https://github.com/Infineon/lpa/blob/master/RELEASE.md) - Detailed release notes and version history for all LPA releases

# License

This library is provided under Infineon (Cypress Semiconductor) software license terms (see the LICENSE file). The LPA middleware is used together with the Wi-Fi core libraries that you add to your application (for example, wifi-core-freertos-lwip-mbedtls or wifi-core-threadx-cat5), which pull in third-party components such as FreeRTOS or ThreadX, lwIP, and mbed TLS; those components are governed by their respective license terms. You are responsible for reviewing and accepting all applicable third-party license terms.

- [LICENSE](./LICENSE.txt)

---

# Copyright

(c) 2026, Infineon Technologies AG, or an affiliate of Infineon
Technologies AG.  All rights reserved.

All other trademarks or registered trademarks referenced herein are the property of their respective owners.

The Bluetooth&reg; word mark and logos are registered trademarks owned by Bluetooth SIG, Inc., and any use of such marks by Infineon is under license.
