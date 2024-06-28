# Low Power Assistant (LPA) middleware library

### What's included?

See the [README.md](./README.md) and the [Low Power Assistant middleware library](https://infineon.github.io/lpa/api_reference_manual/html/index.html) for a complete description of the LPA middleware.

The revision history of the LPA middleware is also available on the [Low Power Assistant middleware library changelog](https://infineon.github.io/lpa/api_reference_manual/html/index.html#group_lpa_changelog).

New in this release:

- Added support for WOWLPF Net-pattern offload for CY8CEVAL-062S2-CYW43022CUB.


### Release versions

|  Version         | Description of change                                                  | Comments                                                 |
| ---------------- | ---------------------------------------------------------------------- | -------------------------------------------------------------- |
| 5.4.0            | Added support for WOWLPF Net-pattern offload for CY8CEVAL-062S2-CYW43022CUB         | |
| 5.3.0            | Added support for CYW955913EVK-01 Kit         | |
| 5.2.0            | Added support for new offload Infra. Added support for DLTRO offload, ICMP offload, NDOE offload, NULL KA offload, NAT KA offload, WOWLPF, MQTT Keepalive offload         | |
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

| Software and tools                                                                  | Version   |
| :---                                                                                | :------:  |
| ModusToolbox&trade; software environment                                            |   3.2     |
| - ModusToolbox&trade; Device Configurator                                           |   4.20    |
| - ModusToolbox&trade; MCU personality in Device Configurator                        |   4.20    |
| - ModusToolbox&trade; Wi-Fi and Bluetooth&reg; personalities in Device Configurator |   4.10    |
| GCC compiler for ModusToolbox&trade;                                                |   11.3.1  |
| IAR Compiler                                                                        |   9.40.2  |
| Arm&reg; Compiler 6                                                                 |   6.16    |

### More information
'
The following resources contain more information:
- [LPA Middleware RELEASE.md](./RELEASE.md)
- [Low Power Assistant middleware library](https://infineon.github.io/lpa/api_reference_manual/html/index.html)
- [ModusToolbox&trade; software environment, quick start guide, documentation, and videos](https://www.infineon.com/modustoolbox)
- [LPA Middleware WLAN low-power code example for FreeRTOS](https://github.com/Infineon/mtb-example-anycloud-wlan-lowpower)
- [LPA middleware WLAN TCP keepalive offload example for FreeRTOS](https://github.com/Infineon/mtb-example-anycloud-offload-tcp-keepalive)
- [ModusToolbox&trade; Device Configurator tool guide](https://www.infineon.com/dgdl/Infineon-ModusToolbox_Device_Configurator_4.20_User_Guide-UserManual-v01_00-EN.pdf?fileId=8ac78c8c8d2fe47b018e0ea9a6727916&redirId=180683)
- [PSoC&trade; 6 technical reference manual](https://www.infineon.com/dgdl/Infineon-PSoC_6_MCU_PSoC_63_with_BLE_Architecture_Technical_Reference_Manual-AdditionalTechnicalInformation-v11_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0f946fea01ca&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-technical_reference_manual&redirId=TRM148)
- [PSoC&trade; 63 with Bluetooth&reg; LE datasheet](https://www.infineon.com/dgdl/Infineon-PSoC_6_MCU_PSoC_63_with_BLE_Datasheet_Programmable_System-on-Chip_(PSoC)-DataSheet-v16_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0ee4efe46c37&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-datasheet&redirId=VL4079)
