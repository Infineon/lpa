# Low Power Assistant Middleware(LPA) Library

### What's Included?

Please refer to the [README.md](./README.md) and the [Low Power Assistant Middleware Library](https://infineon.github.io/lpa/lpa_api_reference_manual/html/index.html) for a complete description of the LPA Middleware.
The revision history of the LPA Middleware is also available on the [Low Power Assistant Middleware Library Changelog](https://infineon.github.io/lpa/lpa_api_reference_manual/html/index.html#group_lpa_changelog).

New in this release:
Removed support for MBED OS and Amazon FreeRTOS


### Release Versions

|  Version         | Description of Change                                                  | Comments                                                 |
| ---------------- | ---------------------------------------------------------------------- | -------------------------------------------------------------- |
| 5.0.0            | Removed support for MBED OS and Amazon FreeRTOS                        | ModusToolbox                                               |
| 4.0.0            | Add LPA support for 43439 Kit                                          | ModusToolbox                                               |
| 3.2.0            | Add LPA support for 4373 Kit                                           | ModusToolbox                                               |
| 3.1.1            | Fix for wait_net_suspend with TCPIP core locking Configuration         | MBED OS 6.8.0 , ModusToolbox, Amazon FreeRTOS v202007.00  |
| 3.1.0            | Minor Fixes and documentation update                                   | MBED OS 6.8.0 , ModusToolbox, Amazon FreeRTOS v202007.00  |
| 3.0.0            | LPA Middleware for all SDK(s) and Modustoolbox 2.2                     | MBED OS 6.2.0 , ModusToolbox, Amazon FreeRTOS v202007.00  |
| 2.1.0(ER)        | Add AFR SDK Support (Wi-Fi Only LPA) and WLAN low power configurations | Amazon FreeRTOS v202002.00                                     |
| 2.0.0            | Add TCP Keepalive offload Feature                                      | ModusToolbox SDK                                          |
| 1.0.0            | New LPA Middleware. Only ARP and Packet Filter offloads supported      | MBEDOS upto 5.15.2                                             |

### Known Issues

### Defect Fixes

### Supported Software and Tools
This version of the LPA Middleware was validated for compatibility with the following Software and Tools:

| Software and Tools                                                        | Version   |
| :---                                                                      | :------:  |
| ModusToolbox Software Environment                                         |   3.1     |
| - ModusToolbox Device Configurator                                        |   4.10    |
| - ModusToolbox MCU Personality in Device Configurator                     |   4.10    |
| - ModusToolbox WiFi and BT Personalities in Device Configurator           |   4.10    |
| GCC compiler for ModusToolbox                                             |   11.3.1  |
| IAR Compiler                                                              |   9.30    |
| ARM Compiler 6                                                            |   6.16    |

### More information
The following resources contain more information:
* [LPA Middleware RELEASE.md](./RELEASE.md)
* [Low Power Assistant Middleware Library](https://infineon.github.io/lpa/lpa_api_reference_manual/html/index.html)
* [ModusToolbox Software Environment, Quick Start Guide, Documentation, and Videos](https://www.infineon.com/cms/en/design-support/tools/sdk/modustoolbox-software/)
* [LPA Middleware WLAN Low power Code Example for FREERTOS](https://github.com/Infineon/mtb-example-anycloud-wlan-lowpower)
* [LPA Middleware WLAN TCP Keepalive Offload Example for FREERTOS](https://github.com/Infineon/mtb-example-anycloud-offload-tcp-keepalive)
* [ModusToolbox Device Configurator Tool Guide](https://www.infineon.com/dgdl/Infineon-ModusToolbox_Device_Configurator_4.0_User_Guide-UserManual-v01_00-EN.pdf?fileId=8ac78c8c8386267f0183a960bd41598f&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-files&redirId=180683&redirId=VL144)
* [PSoC 6 Technical Reference Manual](https://www.infineon.com/dgdl/Infineon-PSoC_6_MCU_PSoC_63_with_BLE_Architecture_Technical_Reference_Manual-AdditionalTechnicalInformation-v11_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0f946fea01ca&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-technical_reference_manual&redirId=TRM148)
* [PSoC 63 with BLE Datasheet Programmable System-on-Chip datasheet](https://www.infineon.com/dgdl/Infineon-PSoC_6_MCU_PSoC_63_with_BLE_Datasheet_Programmable_System-on-Chip_(PSoC)-DataSheet-v16_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0ee4efe46c37&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-datasheet&redirId=VL4079)
  
---

© 2021, Cypress Semiconductor Corporation (an Infineon company) or an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
This software, associated documentation and materials ("Software") is owned by Cypress Semiconductor Corporation or one of its affiliates ("Cypress") and is protected by and subject to worldwide patent protection (United States and foreign), United States copyright laws and international treaty provisions. Therefore, you may use this Software only as provided in the license agreement accompanying the software package from which you obtained this Software ("EULA"). If no EULA applies, then any reproduction, modification, translation, compilation, or representation of this Software is prohibited without the express written permission of Cypress.
Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress reserves the right to make changes to the Software without notice. Cypress does not assume any liability arising out of the application or use of the Software or any product or circuit described in the Software. Cypress does not authorize its products for use in any products where a malfunction or failure of the Cypress product may reasonably be expected to result in significant property damage, injury or death ("High Risk Product"). By including Cypress's product in a High Risk Product, the manufacturer of such system or application assumes all risk of such use and in doing so agrees to indemnify Cypress against all liability.
