# Cypress Low Power Assistant Middleware Library 1.0

### What's Included?

Please refer to the [README.md](./README.md) and the [API Reference Guide](https://cypresssemiconductorco.github.io/lpa/lpa_api_reference_manual/html/index.html) for a complete description of the LPA Middleware.
The revision history of the LPA Middleware is also available on the [API Reference Guide Changelog](https://cypresssemiconductorco.github.io/-lpa/lpa_api_reference_manual/html/index.html#group_lpa_changelog).
New in this release:

* Added the Errata section

### Known Issues
| Problem | Workaround |
| ------- | ---------- |
| Host application will not be notified of the join status when Station is roaming or AP config change such as DHCP pool etc..”.| Needs reboot and associate to AP again |

### Defect Fixes


### Supported Software and Tools
This version of the LPA Middleware was validated for compatibility with the following Software and Tools:

| Software and Tools                                                        | Version |
| :---                                                                      | :----:  |
| ModusToolbox Software Environment                                         | 2.0     |
| - ModusToolbox Device Configurator                                        | 2.0     |
| - ModusToolbox MCU Personality in Device Configurator                     | 2.0     |
| - ModusToolbox WiFi and BT Personalities in Device Configurator           | 2.0     |
| GCC Compiler                                                              | 7.2.1   |
| IAR Compiler                                                              | 8.32    |
| ARM Compiler 6                                                            | 6.11    |
| MBED OS                                                                   | 5.14.2  |

### More information
The following resources contain more information:
* [LPA Middleware RELEASE.md](./RELEASE.md)
* [LPA Middleware API Reference Guide](https://cypresssemiconductorco.github.io/lpa/lpa_api_reference_manual/html/index.html)
* [ModusToolbox Software Environment, Quick Start Guide, Documentation, and Videos](https://www.cypress.com/products/modustoolbox-software-environment)
* [LPA Middleware Code Example for MBED OS](https://github.com/cypresssemiconductorco/mbed-os-example-wlan-lowpower)
* [ModusToolbox Device Configurator Tool Guide](https://www.cypress.com/ModusToolboxDeviceConfig)
* [PSoC 6 Technical Reference Manual](https://www.cypress.com/documentation/technical-reference-manuals/psoc-6-mcu-psoc-63-ble-architecture-technical-reference)
* [PSoC 63 with BLE Datasheet Programmable System-on-Chip datasheet](http://www.cypress.com/ds218787)
* [Cypress Semiconductor](http://www.cypress.com)
  
---
© Cypress Semiconductor Corporation, 2019.
