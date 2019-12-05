# Cypress Low Power Assistant Middleware Library

### Overview

The LPA middleware provides an easy way to make the low-power features of Cypress devices available to developers in the form of a portable configuration layer.
LPA consists of the following components:
* Configurator tool (using a personality), which makes the low-power features of the system easy to use (ModusToolbox Device Configurator Tool Guide). This personality writes data structures that are processed by firmware and implement the choices made in the personality.
* The above mentioned firmware is another component of the LPA feature. The firmware is used at system initialization and does not require user interaction.
* The final component of this feature is a small firmware module provides the integration between the low-power firmware and the IOT framework (most notably the RTOS) used in the system. This final piece of firmware will be part of the IOT framework you are using.

### Features
* MCU Low Power
* Wi-Fi and Bluetooth Low Power
* Wi-Fi Address Resolution Protocol (ARP) Offload
* Wi-Fi Packet Filter Offload

### Quick Start

The LPA could be configured by the ModusToolbox MCU, WiFi and BT personalities. Refer to the [API Reference Guide Configuration Considerations](https://cypresssemiconductorco.github.io/lpa/lpa_api_reference_manual/html/index.html#group_lpa_p2_cc).


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
