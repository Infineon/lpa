# Low Power Assistant Middleware(LPA) Library 3.0.0

### What's Included?

Please refer to the [README.md](./README.md) and the [Low Power Assistant Middleware Library](https://cypresssemiconductorco.github.io/lpa/lpa_api_reference_manual/html/index.html) for a complete description of the LPA Middleware.
The revision history of the LPA Middleware is also available on the [Low Power Assistant Middleware Library Changelog](https://cypresssemiconductorco.github.io/lpa/lpa_api_reference_manual/html/index.html#group_lpa_changelog).

New in this release:
1. Supports all SDK(s)
2. Support for AFR 202007
3. Support for MBED-OS 6.2.0
4. Support for Modustoolbox 2.2

### Release Versions

|  Version         | Description of Change                                                  | Supported SDK                                                  |
| ---------------- | ---------------------------------------------------------------------- | -------------------------------------------------------------- |
| 3.0.0            | LPA Middleware for all SDK(s) and Modustoolbox 2.2                     | MBED OS 6.2.0 , Anycloud SDK , Amazon FreeRTOS v202007.00      |
| 2.1.0(ER)        | Add AFR SDK Support (Wi-Fi Only LPA) and WLAN low power configurations | Amazon FreeRTOS v202002.00                                     |
| 2.0.0            | Add TCP Keepalive offload Feature and AnyCloud SDK Support             | FreeRTOS Anycloud SDK                                          |
| 1.0.0            | New LPA Middleware. Only ARP and Packet Filter offloads supported      |   MBEDOS upto 5.15.2                                           |


### Known Issues

### Defect Fixes

### Supported Software and Tools
This version of the LPA Middleware was validated for compatibility with the following Software and Tools:

| Software and Tools                                                        | Version   |
| :---                                                                      | :------:  |
| ModusToolbox Software Environment                                         |   2.2     |
| - ModusToolbox Device Configurator                                        |   2.2     |
| - ModusToolbox MCU Personality in Device Configurator                     |   1.2     |
| - ModusToolbox WiFi and BT Personalities in Device Configurator           |   1.0     |
| GCC compiler for MBED-OS                                                  |   9.2.0   |
| GCC compiler for other SDK(s)                                             |   7.2.1   |
| IAR Compiler                                                              |   8.32    |
| ARM Compiler 6                                                            |   6.14    |
| MBED OS                                                                   |   6.2.0   |
| AnyCloud FreeRTOS                                                         | SDK v1.2  |
| AMAZON FREERTOS                                                           | V202007.00|

### More information
The following resources contain more information:
* [LPA Middleware RELEASE.md](./RELEASE.md)
* [Low Power Assistant Middleware Library](https://cypresssemiconductorco.github.io/lpa/lpa_api_reference_manual/html/index.html)
* [ModusToolbox Software Environment, Quick Start Guide, Documentation, and Videos](https://www.cypress.com/products/modustoolbox-software-environment)
* [LPA Middleware WLAN Low power Code Example for MBED OS](https://github.com/cypresssemiconductorco/mbed-os-example-wlan-lowpower)
* [LPA Middleware WLAN ARP Offload Example for MBED OS](https://github.com/cypresssemiconductorco/mbed-os-example-wlan-offload-arp)
* [LPA Middleware WLAN Packet Filter Offload Example for MBED OS](https://github.com/cypresssemiconductorco/mbed-os-example-wlan-offload-packet-filter)
* [LPA Middleware WLAN Low power Code Example for FREERTOS](https://github.com/cypresssemiconductorco/mtb-example-anycloud-wlan-lowpower)
* [LPA Middleware WLAN TCP Keepalive Offload Example for FREERTOS](https://github.com/cypresssemiconductorco/mtb-example-anycloud-offload-tcp-keepalive)
* [LPA Middleware WLAN Low power Code Example for AMAZON FREERTOS](https://github.com/cypresssemiconductorco/afr-example-wlan-lowpower)
* [LPA Middleware WLAN Offload Example for AMAZON FREERTOS](https://github.com/cypresssemiconductorco/afr-example-wlan-offloads)
* [ModusToolbox Device Configurator Tool Guide](https://www.cypress.com/ModusToolboxDeviceConfig)
* [PSoC 6 Technical Reference Manual](https://www.cypress.com/documentation/technical-reference-manuals/psoc-6-mcu-psoc-63-ble-architecture-technical-reference)
* [PSoC 63 with BLE Datasheet Programmable System-on-Chip datasheet](http://www.cypress.com/ds218787)
  
---
© Cypress Semiconductor Corporation, 2019-2020.
