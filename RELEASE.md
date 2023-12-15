# Low Power Assistant (LPA) middleware library

### What's included?

See the [README.md](./README.md) and the [Low Power Assistant middleware library](https://infineon.github.io/lpa/lpa_api_reference_manual/html/index.html) for a complete description of the LPA middleware.

The revision history of the LPA middleware is also available on the [Low Power Assistant middleware library changelog](https://infineon.github.io/lpa/lpa_api_reference_manual/html/index.html#group_lpa_changelog).

New in this release:

Added support for CY8CEVAL-062S2-CYW43022CUB kit


### Release versions

|  Version         | Description of change                                                  | Comments                                                 |
| ---------------- | ---------------------------------------------------------------------- | -------------------------------------------------------------- |
| 5.1.0            | Added support for CY8CEVAL-062S2-CYW43022CUB kit                       | |
| 5.0.0            | Removed support for MBED OS and Amazon FreeRTOS                        | ModusToolbox&trade;                                               |
| 4.0.0            | Add LPA support for 43439 Kit                                          | ModusToolbox&trade;                                               |
| 3.2.0            | Add LPA support for 4373 Kit                                           | ModusToolbox&trade;                                               |
| 3.1.1            | Fix for wait_net_suspend with TCPIP core locking Configuration         | MBED OS 6.8.0 , ModusToolbox&trade; , Amazon FreeRTOS v202007.00  |
| 3.1.0            | Minor Fixes and documentation update                                   | MBED OS 6.8.0 , ModusToolbox&trade; , Amazon FreeRTOS v202007.00  |
| 3.0.0            | LPA Middleware for all SDK(s) and Modustoolbox&trade; 2.2              | MBED OS 6.2.0 , ModusToolbox&trade; , Amazon FreeRTOS v202007.00  |
| 2.1.0(ER)        | Add AFR SDK Support (Wi-Fi Only LPA) and WLAN low power configurations | Amazon FreeRTOS v202002.00                                     |
| 2.0.0            | Add TCP Keepalive offload Feature                                      | ModusToolbox&trade; SDK                                          |
| 1.0.0            | New LPA Middleware. Only ARP and Packet Filter offloads supported      | MBEDOS upto 5.15.2                                             |

### Known issues

### Defect fixes

### Supported software and tools

This version of the LPA middleware was validated for compatibility with the following software and tools:

| Software and tools                                                        | Version   |
| :---                                                                      | :------:  |
| ModusToolbox&trade; software environment                                         |   3.1     |
| - ModusToolbox&trade; Device Configurator                                        |   4.10    |
| - ModusToolbox&trade; MCU personality in Device Configurator                     |   4.10    |
| - ModusToolbox&trade; Wi-Fi and Bluetooth&reg; personalities in Device Configurator           |   4.10    |
| GCC compiler for ModusToolbox&trade;                                             |   11.3.1  |
| IAR Compiler                                                              |   9.30    |
| Arm&reg; Compiler 6                                                            |   6.16    |

### More information
'
The following resources contain more information:
- [LPA Middleware RELEASE.md](./RELEASE.md)
- [Low Power Assistant middleware library](https://infineon.github.io/lpa/api_reference_manual/html/index.html)
- [ModusToolbox&trade; software environment, quick start guide, documentation, and videos](https://www.infineon.com/modustoolbox)
- [LPA Middleware WLAN low-power code example for FreeRTOS](https://github.com/Infineon/mtb-example-anycloud-wlan-lowpower)
- [LPA middleware WLAN TCP keepalive offload example for FreeRTOS](https://github.com/Infineon/mtb-example-anycloud-offload-tcp-keepalive)
- [ModusToolbox&trade; Device Configurator tool guide](https://www.infineon.com/dgdl/Infineon-ModusToolbox_Device_Configurator_4.10_User_Guide-UserManual-v01_00-EN.pdf?fileId=8ac78c8c88704c7a0188a18bc3c94e70&redirId=180683)
- [PSoC&trade; 6 technical reference manual](https://www.infineon.com/dgdl/Infineon-PSoC_6_MCU_PSoC_63_with_BLE_Architecture_Technical_Reference_Manual-AdditionalTechnicalInformation-v11_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0f946fea01ca&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-technical_reference_manual&redirId=TRM148)
- [PSoC&trade; 63 with Bluetooth&reg; LE datasheet](https://www.infineon.com/dgdl/Infineon-PSoC_6_MCU_PSoC_63_with_BLE_Datasheet_Programmable_System-on-Chip_(PSoC)-DataSheet-v16_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0ee4efe46c37&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-datasheet&redirId=VL4079)
  

All referenced product or service names and trademarks are the property of their respective owners.

The Bluetooth® word mark and logos are registered trademarks owned by Bluetooth SIG, Inc., and any use of such marks by Infineon is under license.

----

© 2023, Cypress Semiconductor Corporation (an Infineon company) or an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
This software, associated documentation and materials ("Software") is owned by Cypress Semiconductor Corporation or one of its affiliates ("Cypress") and is protected by and subject to worldwide patent protection (United States and foreign), United States copyright laws and international treaty provisions. Therefore, you may use this Software only as provided in the license agreement accompanying the software package from which you obtained this Software ("EULA"). If no EULA applies, then any reproduction, modification, translation, compilation, or representation of this Software is prohibited without the express written permission of Cypress.
Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress reserves the right to make changes to the Software without notice. Cypress does not assume any liability arising out of the application or use of the Software or any product or circuit described in the Software. Cypress does not authorize its products for use in any products where a malfunction or failure of the Cypress product may reasonably be expected to result in significant property damage, injury or death ("High Risk Product"). By including Cypress's product in a High Risk Product, the manufacturer of such system or application assumes all risk of such use and in doing so agrees to indemnify Cypress against all liability.
