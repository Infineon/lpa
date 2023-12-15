/***************************************************************************//**
* \file cy_lpa_wifi_ol_common.h
* \version 3.0
*
* \brief
* Low Power Offload ARP Assist.
*
********************************************************************************
* \copyright
* Copyright 2020, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

/**
 *******************************************************************************
 * \mainpage Low Power Assistant Middleware Library 5.0.0 
 *******************************************************************************
 * \section section_lpa_overview Overview
 *******************************************************************************
 *
 * Power consumption is a key operational factor for embedded devices.
 * The Low Power Assistant (LPA) allows you to configure a PSoC&trade; 6 MCU
 * and WLAN (Wi-Fi / Bluetooth&reg; radio) device to provide low-power features. This
 * framework presents a unified, low-overhead, user-friendly way to
 * configure, connect, and operate within multiple tasks/cases.
 *
 * The key points for LPA include:
 *
 * * Applies to MCU, Wi-Fi, and Bluetooth&reg;
 * * For RTOS-oriented applications (FreeRTOS).
 * * Configuration of LPA middleware is required. Application code changes are needed 
 *   to call LPA middleware functions in runtime.
 * * Configurations are done using ModusToolbox&trade; Device Configurator flow.
 *
 *******************************************************************************
 * \subsection section_lpa_features Features
 *******************************************************************************
 *
 * There are various use cases for the LPA covered in
 * the following sections. The LPA allows you to configure different parts
 * of the design to be energy-efficient.
 *
 * * \ref group_lpa_p1
 *   Allowa you to configure MCU to enter low-power mode to 
 *   achieve maximum power savings.
 *
 * * \ref group_lpa_p2
 *
 *   * \ref group_lpa_p2_host_wake
 *     Allows the WLAN device to wake up the Host MCU
 *     from its low-power state.
 *   * \ref group_lpa_p2_arp_offload
 *     Improves the power consumption of the connected system
 *     by reducing the time that the host needs to stay awake because of ARP
 *     broadcast traffic.
 *   * \ref group_lpa_p2_packet_filter
 *     Allows the host processor to limit which types of
 *     packets get passed up to the host processor from the WLAN subsystem.
 *     This is useful to keep out unwanted/unneeded packets from the network
 *     that might otherwise wake up the host out of a power saving
 *     Deep Sleep mode, or prevent it from entering Deep Sleep mode.
 *   * \ref group_lpa_p2_tcp_keepalive
 *     Improves the power consumption of the host MCU by offloading
 *     TCP keepalive to WLAN firmware.
 *
 * * \ref group_lpa_p3
 *   Enable the Bluetooth&reg; wake-up pins by configuring the Bluetooth&reg; host wake pin and Bluetooth&reg; device wake pin.
 *
 * The listed capabilities make the LPA middleware useful for a
 * variety of applications, including automotive, IoT, and industrial.
 *
 * The LPA middleware provides an easy way
 * to make the low-power features of Infineon devices available to
 * developers in the form of a portable configuration layer.
 * LPA consists of the following components:
 * * One of these components is a configurator tool
 *   (using a personality), which makes the low-power features of
 *   the system easy to use
 *   (<a href="https://www.infineon.com/ModusToolboxDeviceConfig">
 *   <b>ModusToolbox&trade; Device Configurator Tool Guide</b></a>).
 *   This personality writes data structures
 *   that are processed by the firmware and implement the
 *   choices made in the personality.
 * * This firmware is another component of the LPA feature. The firmware
 *   is used at system initialization and does not require user interaction.
 * * A small firmware module provides integration between the low-power 
 *   firmware and the user application. This final piece of firmware will be part of 
 *   the end-user application.
 *
 *******************************************************************************
 * \section section_lpa_getting_started Getting started
 *******************************************************************************
 *
 * The LPA middleware can be used in various software environments. 
 * The quickest way to get started is by using the Code Examples.
 * Infineon continuously extends its portfolio
 * of code examples at the <a href="http://www.infineon.com">
 * <b>Infineon website</b></a> and at the
 * <a href="https://github.com/Infineon">
 * <b>Infineon  GitHub</b></a> website.
 * The following quick start guide sections describe several use cases
 * for using the LPA features:
 * * MCU low power \ref group_lpa_p1_mcu_qsg
 * * Wi-Fi host wake signal \ref group_lpa_p2_host_wake_qsg
 * * Wi-Fi Address Resolution Protocol (ARP)
 *   Offload \ref group_lpa_p2_arp_offload_qsg
 * * Wi-Fi packet filter offload \ref group_lpa_p2_packet_filter_qsg
 * * Wi-Fi TCP keepalive offload \ref group_lpa_p2_tcp_keepalive_qsg
 * * Bluetooth&reg; low power
 *
 * For more details about LPA and ModusToolbox&trade;, refer to
 * the \ref section_lpa_more_information section.
 *
 *******************************************************************************
 * \section section_lpa_Prerequisites Prerequisites
 *******************************************************************************
 *
 * * ModusToolbox&trade; development environment configured for FreeRTOS.
 * * Availability of the CY8CKIT-062S2-43012 Pioneer Kit (or other kits 
 *   that support the PSoC&trade; 6 power consumption measurement).\n
 *   The CY8CKIT-062S2-43012 Kit is recommended because this section 
 *   documents the measurement instructions for this kit. If another kit 
 *   is used, refer to its documentation and learn how to measure the 
 *   current consumed.
 *
 *******************************************************************************
 * \section group_lpa_definitions Definitions
 *******************************************************************************
 *
 * This section lists the definitions used in this document.
 *
 * <table class="doxtable">
 *   <tr><th>Acronym/Term</th><th>Definition</th><th>Remark</th></tr>
 *   <tr><td>AP</td>
 *       <td>Access Point</td>
 *       <td>Wireless AP connection for the device
 *           (e.g., Wireless router).</td></tr>
 *   <tr><td>ARP</td>
 *       <td>Address Resolution Protocol</td>
 *       <td>ARP is a procedure for mapping a dynamic IP
 *           address to a permanent physical machine address in a
 *           local area network (LAN).</td></tr>
 *   <tr><td>TKO</td>
 *       <td>TCP keepalive offload</td>
 *       <td></td></tr>
 *   <tr><td>BT</td>
 *       <td>Bluetooth&trade;</td>
 *       <td>Bluetooth is a wireless technology standard.</td></tr>
 *   <tr><td>Device</td>
 *       <td>WLAN device</td>
 *       <td>The Wi-Fi and/or Bluetooth&reg; radio module (WLAN processor)</td></tr>
 *   <tr><td>Host</td>
 *       <td>Host processor</td>
 *       <td>The host (or application) processor (e.g., PSoC&trade; 6).</td></tr>
 *   <tr><td>LPA</td>
 *       <td>Low Power Assistant</td>
 *       <td></td></tr>
 *   <tr><td>OLM</td>
 *       <td>Offload Manager</td>
 *       <td></td></tr>
 *   <tr><td>OOB</td>
 *       <td>Out-Of-Band</td>
 *       <td></td></tr>
 *   <tr><td>Configurator</td>
 *       <td>Infineon configuration tool</td>
 *       <td>Configurators are a set of powerful but intuitive tools
 *           that work together to set up various MCU features. Each
 *           Configurator generates very readable, user-modifiable firmware to
 *           initialize the whole device with a single function call. Refer to
 *           <a href="https://www.infineon.com/products/modustoolbox-software-environment"><b>ModusToolBox&trade;</b></a></td></tr>
 *   <tr><td>Personality</td>
 *       <td>Information file</td>
 *       <td>Personalities are files that define how resources are used by a
 *           configurator. The Low Power Assistant functionality is
 *           embedded in the Device Configurator as a personality.</td></tr>
 *   <tr><td>SDIO</td>
 *       <td>Secure Digital Input/Output</td>
 *       <td></td></tr>
 *   <tr><td>WLAN</td>
 *       <td>Wireless Local Area Network</td>
 *       <td>Any WLAN, including Wi-Fi, which is a type of WLAN and
 *           adheres to the IEEE 802.11 standards, can be wireless.</td></tr>
 *   <tr><td>MTB</td>
 *       <td>ModusToolbox&trade;</td>
 *       <td>Refer to
 *           <a href="https://www.infineon.com/products/modustoolbox-software-environment"><b>ModusToolBox&trade;</b></a></td></tr>
 * </table>
 *
 *******************************************************************************
 * \section group_quick_start_guide LPA project Updates
 *******************************************************************************
 *
 * This section provides step-by-step instructions for how to create a project, 
 * modify the application, and program it to the device. \n
 * 
 * Follow these steps while creating a project to evaluate the LPA features with different MCU Low Power and Wi-Fi Low Power configurations:
 *
 * 1. Copy the latest <a href="https://github.com/Infineon/freertos/tree/master/Source/portable"><b> FreeRTOSConfig.h</b></a> and update with the below changes
 *
 *	  configUSE_TICKLESS_IDLE : This change to support Tickless Deep Sleep mode
 *    \code
 *    #include <cycfg_system.h>
 *
 *    #if (CY_CFG_PWR_SYS_IDLE_MODE == CY_CFG_PWR_MODE_SLEEP) || (CY_CFG_PWR_SYS_IDLE_MODE == CY_CFG_PWR_MODE_DEEPSLEEP)
 *    extern void vApplicationSleep( uint32_t xExpectedIdleTime );
 *    #define portSUPPRESS_TICKS_AND_SLEEP( xIdleTime ) vApplicationSleep( xIdleTime )
 *    #define configUSE_TICKLESS_IDLE  2
 *    #endif
 *    
 *    #if CY_CFG_PWR_DEEPSLEEP_LATENCY > 0
 *    #define configEXPECTED_IDLE_TIME_BEFORE_SLEEP   CY_CFG_PWR_DEEPSLEEP_LATENCY
 *    #endif
 *    \endcode
 *
 * 2. For FreeRTOS application, Add the following file to the deps folder for CY8CKIT_062S2_43012:
 *    \code
 *    echo https://github.com/Infineon/freertos/#latest-v10.X#\$\$ASSET_REPO\$\$/freertos/latest-v10.X > deps/freertos.mtb
 *    \endcode
 *
 * 3. For <b>MCU Low Power</b>, Replace main.c file with the following code. FreeRTOS-based application is needed for the system to enter low-power during system idling. LED ON/OFF state is used to measure the power when system is in active and sleep state.
 *    \code
 *    #include "cy_pdl.h"
 *    #include "cyhal.h"
 *    #include "cybsp.h"
 *    #include "FreeRTOS.h"
 *    #include "task.h"
 *    
 *    #define BLINKING_RATE_MS        5000
 *    static void blinky(void *args)
 *    {
 *        TickType_t ticks = pdMS_TO_TICKS(BLINKING_RATE_MS) ;
 *    
 *        // Initialize the User LED
 *        cyhal_gpio_init((cyhal_gpio_t) CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
 *    
 *        while (true)
 *        {
 *            //Turn ON LED
 *            cyhal_gpio_write((cyhal_gpio_t)CYBSP_USER_LED, false);
 *            //Add Delay for 5 seconds so that MCU doesnot go to Deep Sleep
 *            cyhal_system_delay_ms(BLINKING_RATE_MS);
 *            //Turn OFF LED
 *            cyhal_gpio_write((cyhal_gpio_t)CYBSP_USER_LED, true);
 *            vTaskDelay(ticks) ;
 *        }
 *    }
 *    int main(void)
 *    {
 *        cy_rslt_t result;
 *    
 *        // Initialize the device and board peripherals
 *        result = cybsp_init() ;
 *        if (result != CY_RSLT_SUCCESS)
 *        {
 *            CY_ASSERT(0);
 *        }
 *        __enable_irq();
 *    
 *        xTaskCreate( blinky, "Blinky Task", 1024*10,  0,  1,  0);
 *        // Start the FreeRTOS scheduler
 *        vTaskStartScheduler();
 *    }
 *    \endcode
 *
 * 4. For <b>Wi-Fi Low Power</b> application,
 *    * Add the following file to the deps folder for CY8CKIT_062S2_43012:
 *    \code
 *    echo mtb://wifi-core-freertos-lwip-mbedtls#release-v1.1.0#$$ASSET_REPO$$/wifi-core-freertos-lwip-mbedtls/release-v1.1.0
 *    \endcode
 *
 *    * Modify the example for the Wi-Fi SSID parameters in lowpower_task.h
 *    \code
 *    #define WIFI_SSID                         "SSID"
 *    #define WIFI_PASSWORD                     "PASSWORD"
 *    #define WIFI_SECURITY                     CY_WCM_SECURITY_WPA2_MIXED_PSK
 *    \endcode
 *
 *    * The wait_net_suspend() used in lowpower_task.c allows the host MCU to go to deep-sleep and stay in deep-sleep until woken up by an external event.
 *    \code
 *    wait_net_suspend(wifi, portMAX_DELAY, INACTIVE_INTERVAL_MS, INACTIVE_WINDOW_MS);
 *    \endcode
 *
 *******************************************************************************
 * \section group_device_configurator_flow ModusToolbox&trade; Device Configurator flow
 *******************************************************************************
 * Generating the initialization code using the ModusToolbox&trade; Device Configurator
 *   greatly simplifies configuring the PSoC&trade; and
 *   enabling the LPA features. The ModusToolbox&trade; Device Configurator
 *   provides the user interface to set up and automatically generate
 *   the initialization code (including analog routing) and
 *   configuration structures.
 *
 * <b>Note</b> 
 * If you modify the output of the ModusToolbox&trade; Device Configurator, 
 * you cannot import it back into the tool.
 *
 * * The following are steps to avoid the design.modus file modification in the checkout repo:
 *
 *   1. Copy contents of the mtb-example-psoc6-empty-app/libs/TARGET_CY8CKIT-062S2-43012/COMPONENT_BSP_DESIGN_MODUS folder to mtb-example-psoc6-empty-app/COMPONENT_CUSTOM_DESIGN_MODUS/TARGET_CY8CKIT_062S2_43012 folder.
 *   2. Delete design.cyqspi and design.cycapsense files in mtb-example-psoc6-empty-app/TARGET_CY8CKIT_062S2_43012/CUSTOM_BSP_DESIGN_MODUS.
 *   3. Update the Makefile in the mtb-example-psoc6-empty-app folder with the following details (This informs the build to ignore the BSP configuration inside the libs folder)
 *      \code
 *      COMPONENTS+=CUSTOM_DESIGN_MODUS
 *      DISABLE_COMPONENTS=BSP_DESIGN_MODUS 
 *      \endcode 
 *
 * * Navigate to the ModusToolbox&trade; installation folder and launch 
 *   the ModusToolbox&trade; Device Configurator
 *   (\<install_dir\>/tools_3.1/device-configurator).
 * * Select File-\>Open, navigate to the board's design.modus file,
 *   and open it: \n
 *   <i>mtb-example-psoc6-empty-app/COMPONENT_CUSTOM_DESIGN_MODUS/TARGET_CY8CKIT_062S2_43012/design.modus</i> \n
 * * If the design.modus file does not open and pops with an error message <i>No device support library path provided </i>,
 *    For ModusToolbox&trade; v3.0 and later, point to the mtb-pdl-cat1 folder inside the mtb_shared/mtb-pdl-cat1/<>/props.json and  .modustoolbox/global/device-db/release-v4.6.0/props.json files in the window popup.
 *    For ModusToolbox&trade; below v3.0, point to the psoc6pdl folder inside the mtb-example-anycloud-wlan-lowpower/libs/psoc6pdl/devicesupport.xml (For ModusToolbox&trade; v2.2, point to mtb_shared/mtb-pdl-cat1/<>/devicesupport.xml) file in the window popup.
 *   This is because the design.modus file will update its path to the PDL directory when it is opened for the first time.
 * * Select the host device tab, and then select the System tab for
 *    that device.
 * * Enable the Power personality (if disabled) and go to the Power Parameters
 *    pane to configure the LPA Middleware.
 * * Configure RTOS related parameters:
 *    1. System Idle Power Mode (Active/Sleep/DeepSleep)
 *    2. Deep Sleep Latency
 * * Select File-\>Save to generate the initialization code.
 *
 * \image html Power_Personality.png
 *
 * After saving the configuration file, the generated code is available under
 * the GeneratedSource folder, located in the same directory as
 * the design.modus file in the BSP:
 * * C Data File:
 *   GeneratedSource/cycfg_platform.c
 * * C Header File:
 *   GeneratedSource/cycfg_platform.h
 *
 * <b>Note</b> 
 * Using the ModusToolbox&trade; Device Configurator overwrites changes that
 * you made in the cycfg_system.h file.
 *
 *******************************************************************************
 * \section group_lpa_p1 Part 1. MCU Low Power
 *******************************************************************************
 *
 * The MCU low-power feature allows you to take advantage of the power
 * saving features of a PSoC&trade; MCU simply by configuring a few parameters.
 * Using the MCU low-power feature, you can configure the system to achieve 
 * maximum power savings during system idling or to establish maximum 
 * performance by operating only in Active power mode. This feature works in 
 * conjunction with FreeRTOS.
 *
 * There are two parameters available: System Idle Power Mode and Deep Sleep
 * Latency.
 *
 * The System Idle Power Mode parameter defines the power state that the MCU
 * needs to enter automatically any time the system is idle. Setting it
 * to Active power mode disables power saving and allows the system to perform
 * tasks with less intervention because there are no transitions to/from
 * CPU Sleep/System Deep Sleep states.
 *
 * The Deep Sleep Latency parameter controls if the desired deep sleep
 * time is larger than needed to perform a transition from
 * System Deep Sleep to Active power mode to perform tasks.
 *
 * For more information, refer the SysPm (System Power Management) documentation
 * https://infineon.github.io/mtb-pdl-cat1/pdl_api_reference_manual/html/group__group__syspm.html
 *
 * The LPA library provides features for MCU Low Power, Wi-Fi Low Power and Bluetooth&reg; Low Power; however, the LPA library only needs to be included in applications that use Wi-Fi low power.
 *
 *******************************************************************************
 * \subsection group_lpa_p1_mcu_qsg Quick start guide
 *******************************************************************************
 *
 * This quick start guide demonstrates how to configure and use 
 * the WLAN_HOST_WAKE pin for the MCU Low Power feature in 
 * the FreeRTOS environment. This guide also shows the feature's impact 
 * on the system's low power.
 *
 * 1. Refer readme of <a href="https://github.com/Infineon/mtb-example-psoc6-empty-app.git"><b>mtb-example-psoc6-empty-app</b></a> for project creation on <a href="https://www.infineon.com/dgdl/Infineon-CY8CKIT-062S2-43012_PSoC_62S2_Wi-Fi_BT_Pioneer_Kit_Guide-UserManual-v01_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0f01c8f11927"><b>CY8CKIT-062S2-43012</b></a> pioneer kit.
 * 
 * 2. Delete the following libs from the deps folder:
 *    \code
 *    deps/TARGET_CY8CPROTO-062-4343W.lib
 *    \endcode
 *
 * 3. Refer the \ref group_quick_start_guide section for LPA changes in the application.
 * 4. Refer the \ref section_lpa_hostwake section to verify WLAN_HOST_WAKE pin configurations using the Device Configurator.
 * 5. Set the desired System Idle Power mode (DeepSleep, Sleep or Active).
 *    In FreeRTOS, the System Idle Power mode is set to Deep Sleep by default 
 *    to achieve the best power saving.
 *    This step can be done by using the ModusToolbox&trade; Device Configurator 
 *    <b>MCU Low power using the ModusToolbox&trade; Device Configurator</b>
 *       Refer to \ref group_device_configurator_flow
 * 6. Refer mtb-example-psoc6-empty-app <a href="https://github.com/Infineon/mtb-example-empty-app/blob/master/README.md"><b>readme</b></a> to build the project and program the board.
 *    The following command is an example for the CY8CKIT_062S2_43012 Board, using GCC_ARM as the toolchain:
 *    \code
 *    make getlibs
 *    make program TARGET=CY8CKIT-062S2-43012 TOOLCHAIN=GCC_ARM
 *    \endcode
 *
 * 7. Check the board operation. Refer to the \ref section_lpa_measurement section for corresponding
 *    instructions. Observe the power consumption in different states of the main thread (Active and Idle).
 *    The illuminated user LED indicates the Active state. The non-illuminated LED indicates the Idle state.
 *    The duration of Active/Idle states can be adjusted by changing the BLINKING_RATE_MS value in the blinky function.
 *    Refer to the following picture for an example of the DC Power Analyzer output:
 *
 *    \image html FreeRTOS_blinky_current.png height=500px
 *
 *******************************************************************************
 * \subsection group_lpa_p1_cc MCU Low Power Configuration Considerations
 *******************************************************************************
 * Refer to the section \ref group_device_configurator_flow to configure
 * MCU low power using desgin.modus
 *
 *******************************************************************************
 * \subsubsection group_lpa_p1_cc_parameters Configuration Parameters
 *******************************************************************************
 *
 * The following parameters and their mapping to macros are available:
 *
 * <table class="doxtable">
 *   <tr><th>Category</th>
 *       <th>Parameter</th>
 *       <th>Description</th>
 *       <th>Parameter values</th></tr>
 *   <tr><td>RTOS</td>
 *       <td>System Idle Power Mode</td>
 *       <td>Selects the lowest power mode the system attempts to enter when
 *           there are no active tasks to execute; that is, the system
 *           is in idle state. This option only applies to
 *           an RTOS based application.</td>
 *       <td>
 *           * System Deep Sleep (default)
 *           * CPU Sleep
 *           * Active</td></tr>
 *   <tr><td>RTOS</td>
 *       <td>Deep Sleep Latency (ms)</td>
 *       <td>Selects the greater value among time required to enter in and exit
 *           from the Deep Sleep power mode. This option only applies to
 *           an RTOS based application.</td>
 *       <td>
 *           * 0 (default)
 *           * 0-10</td></tr>
 * </table>
 *
 *******************************************************************************
 * \section group_lpa_p2 Part 2. Wi-Fi low power
 *******************************************************************************
 *
 * LPA library provides features for MCU low power, Wi-Fi low power, and Bluetooth&reg; low power.
 * LPA library only needs to be included in applications that use Wi-Fi low power.
 *
 * The WLAN FW supports various offloads that continue operations on behalf
 * of the host while the host is asleep. Making the best use of these FW
 * offloads requires proper configuration, as well as coordination with 
 * the host OS and host power management systems. Until now, each application
 * developer was responsible for discovering the existence of FW offloads,
 * learning how to use and configure them, and coordinating
 * with the host's power management system.
 * The offloads manager (OLM) is responsible for:
 * * Encapsulating the configuration, coordination, and knowledge of WLAN
 *   offloads into a single, portable, easy-to-use middleware.
 * * Providing a consistent means of developing offloads.
 * * Providing a platform agnostic configuration and initialization interface.
 *
 * Integrating WLAN offloads on the host has typically been performed by
 * customers or hard-coded into the WLAN driver. With the introduction of
 * an offload configurator, customers can configure a range
 * of offloads. This configuration is consistent and portable since
 * multiple platforms perform similar steps to integrate any particular
 * offload.
 *
 * Power consumption is a key operational factor for embedded devices.
 * WLAN offloads play a key role in determining the host power consumption
 * because offloads let the host go into System Deep Sleep mode for extended
 * periods of time while handling things like 802.11 roaming, ARP,
 * IPV6 neighbor resolution, key rotation, and TCP keep alive, on behalf
 * of the host.
 *
 * However, each one of these different offloads needs to be recognized,
 * configured, connected to the power management subsystem, and debugged.
 * Currently, this needs to be done by each individual application developer.
 * Because of the high overhead, offloads are often overlooked, and therefore
 * power consumption is not as low as it could be.
 *
 * The LPA middleware provides a framework that manages WLAN offloads,
 * reduces the overhead incurred by application developers, and makes
 * the offloads more usable while reducing host power consumption.
 *
 * The framework:
 * * Encapsulates the programming for all low-power offloads it supports.
 *   Application writers do not need to know these details.
 * * Uses the ModusToolbox&trade; Device Configurator and personalities to configure:
 *   * which offloads will get compiled in
 *   * Parameters for each offload
 * * Each offload has its own set of configured parameters and its
 *   own implementation. Offloads do not call functionality contained
 *   in another offload.
 * * Provides a consistent means of developing offloads.
 * * Is adaptable to new offloads being offered by the firmware.
 * * Is easily portable new hosts and new architectures. Therefore, the OLM
 *   is independent on the platform and network stack.
 * * Code efficient:
 *   * Minimal space: The object code for an offload driver that is never
 *     used at run-time is not linked into the program image.
 *   * Static memory usage: no runtime calls to malloc/free.
 * * The framework supports multiple WLAN host driver instances.
 *   That is, a collection of offload driver instances and configurations
 *   are applied per WLAN host driver instance.
 *
 * Each offload can be enabled or disabled at build-time.
 *
 *******************************************************************************
 * \subsection group_lpa_p2_host_wake Wi-Fi host-wake signal
 *******************************************************************************
 *
 * Host-wake provides a way for a WLAN device to wake up the host MCU from
 * its low-power state. Host-wake is implemented using a GPIO on the MCU that
 * is connected to the WLAN device. The WLAN device asserts the host-wake GPIO
 * to bring the host MCU out of its low-power state. This interrupt is
 * called as an out-of-band (OOB) interrupt. This configuration is critical for all the WLAN Low power offload
 * such as ARP, Packet Filter, TCP keepalive to wake up host MCU out of its low-power state.
 *
 * Refer to the \ref group_lpa_p2_cc section to configure the Host-wake
 * pin on the Host. The Host-wake pin polarity is configurable.
 * The WLAN device is configured to re-route the SDIO in-band card
 * interrupt to WL_HOST_WAKE (OOB GPIO interrupt). The following diagram shows
 * connections between the host and WLAN device:
 *
 * \image html HostWakeSignal.png height=150px
 *
 * Where:
 * * SDIO: clock, data
 * * WL_HOST_WAKE: OOB interrupt line to wake Host for service
 *
 *******************************************************************************
 * \subsubsection group_lpa_p2_host_wake_qsg Quick start guide
 *******************************************************************************
 *
 * This quick start guide demonstrates how to configure and use 
 * the WLAN_HOST_WAKE pin for the MCU low power feature in 
 * the FreeRTOS environment. This guide also shows the feature's impact 
 * on the system's low power.
 *
 * 1. Refer readme of <a href="https://github.com/Infineon/mtb-example-anycloud-wlan-lowpower.git"><b>
 * mtb-example-anycloud-wlan-lowpower</b></a> for project creation. 
 * 2. Refer section \ref group_quick_start_guide for LPA changes in the application.
 * 3. Refer section \ref section_lpa_hostwake to verify WLAN_HOST_WAKE pin configurations using device configurator.
 * 4. Refer mtb-example-anycloud-wlan-lowpower <a href="https://github.com/Infineon/mtb-example-anycloud-wlan-lowpower/blob/master/README.md"><b>readme</b></a> to build the project and program the board.
 *    The following command is an example for the CY8CKIT_062S2_43012 Board, using GCC_ARM as the toolchain:
 *    \code
 *    make getlibs
 *    make program TARGET=CY8CKIT-062S2-43012 TOOLCHAIN=GCC_ARM
 *    \endcode
 *
 *    When the modified mtb-example-anycloud-wlan-lowpower starts, the console output 
 *    shows that it connects to the specified 
 *    above Wi-Fi Access Point, and then the PSoC&trade; 6 MCU goes to 
 *    System Deep Sleep mode.
 *    \code
 *    
 *    =======================================================
 *    CE230106 - Example: WLAN Lowpower
 *    =======================================================
 *
 *    WLAN MAC Address : D4:4D:A4:A0:02:A4
 *    WLAN Firmware    : wl0: Jan 27 2020 21:57:29 version 13.10.271.236 (5a526db) FWID 01-61e2b002
 *    WLAN CLM         : API: 18.2 Data: 9.10.0 Compiler: 1.36.1 ClmImport: 1.34.1 Creation: 2020-01-27 21:54:33
 *    WHD VERSION      : v1.80.0 : v1.80.0 : GCC 7.2 : 2020-03-10 04:09:17 -0500
 *    Info:Connecting to AP
 *    IP Address 10.0.0.201 assigned
 *    Info:Successfully joined Wi-Fi network 'SSID'.
 *    Info:Beacon period = 100, DTIM period = 3
 *    
 *    Network Stack Suspended, MCU will enter DeepSleep power mode
 *    Resuming Network Stack, Network stack was suspended for 15253ms
 *    
 *    =====================================================
 *    WHD Stats..
 *    tx_total:64, rx_total:71, tx_no_mem:0, rx_no_mem:0
 *    tx_fail:0, no_credit:0, flow_control:0
 *    Bus Stats..
 *    cmd52:2286, cmd53_read:366, cmd53_write:559
 *    cmd52_fail:0, cmd53_read_fail:0, cmd53_write_fail:0
 *    oob_intrs:72, sdio_intrs:147, error_intrs:0, read_aborts:0
 *    =====================================================
 *    Network is active. Resuming network stack
 *    Network Stack Suspended, MCU will enter DeepSleep power mode
 *    \endcode
 *
 * 5. PSoC&trade; 6 MCU is in System Deep Sleep mode. Only WLAN OOB can wake up the host
 *    in this situation. Check the board operation. Use a PC to connect 
 *    to the same Wi-Fi AP as the PSoC&trade; 6 board.
 *
 *    Send a "ping" command to the board and observe in the serial 
 *    terminal that the PSoC&trade; 6 MCU wakes up each command:
 *    \code
 *    C:\>ping -n 3 10.0.0.201
 *    Pinging 10.0.0.201 with 32 bytes of data:
 *    Reply from 10.0.0.201: bytes=32 time=274ms TTL=255
 *    Reply from 10.0.0.201: bytes=32 time=393ms TTL=255
 *    Reply from 10.0.0.201: bytes=32 time=396ms TTL=255
 *    
 *    Ping statistics for 10.0.0.201:
 *        Packets: Sent = 3, Received = 3, Lost = 0 (0% loss),
 *    Approximate round trip times in milli-seconds:
 *        Minimum = 274ms, Maximum = 396ms, Average = 354ms
 *    
 *    <Terminal logs >
 *    Resuming Network Stack, Network stack was suspended for 443ms
 *    =====================================================
 *    WHD Stats..
 *    tx_total:91, rx_total:97, tx_no_mem:0, rx_no_mem:0
 *    tx_fail:0, no_credit:0, flow_control:0
 *    Bus Stats..
 *    cmd52:2314, cmd53_read:488, cmd53_write:607
 *    cmd52_fail:0, cmd53_read_fail:0, cmd53_write_fail:0
 *    oob_intrs:93, sdio_intrs:187, error_intrs:0, read_aborts:0
 *    =====================================================
 *    
 *    Network Stack Suspended, MCU will enter Deep sleep power mode.
 *    \endcode
 *    Ping traffic causes WLAN OOB wakes up the host, and oob_intrs displayed in the serial terminal
 *    output shows the number of WLAN OOB interrupts received.
 *
 *******************************************************************************
 * \subsection group_lpa_p2_arp_offload Wi-Fi ARP Offload
 *******************************************************************************
 *
 * The ARP Offload part of the LPA is designed to improve the power consumption
 * of your connected system by reducing the time the Host needs to stay
 * awake due to ARP broadcast traffic. In general, the ARP Offload
 * reduces the broadcast traffic. To enable this support, refer to the
 * \ref group_lpa_p2_cc section. This document describes how to enable
 * the ARP Offload features that can be incorporated into your project from
 * <a href="https://github.com/Infineon/lpa">
 * <b>Infineon GitHub LPA Middleware</b></a>.
 *
 * ARP is used for mapping an IP address (e.g., 192.168.1.1)) to a physical machine
 * address (e.g., ac:32:df:14:16:07) lookup. ARP uses broadcast frames
 * to accomplish this.
 * * Reduce the System Deep Sleep wake-up to reduce the host processor
 *   power consumption.
 * * Reduce network traffic. If many IoT devices are in one space,
 *   the Wi-Fi bands can get congested with unnecessary broadcast traffic.
 *
 * ARP broadcast traffic is normally forwarded from the network to
 * the device (Wi-Fi radio) and then to the host (application CPU)
 * network stack. If the host is sleeping, the device wakes it up.
 * Having the device handle some of the ARP traffic will
 * reduce the frequency that the host sleeps/wakes up, reducing the host power
 * consumption by staying in CPU Sleep and System Deep Sleep states longer.
 * Having the device handle some of the ARP requests from
 * the host to the network will reduce network traffic.
 *
 * \image html ARP1.png height=500px
 *
 * The WLAN ARP Offload feature of the LPA helps you
 * configure the ARP requests handled by the device.
 *
 *******************************************************************************
 * \subsubsection group_lpa_p2_awake_sleeping Awake vs. Sleeping
 *******************************************************************************
 *
 * The ARP offload feature of the LPA has
 * the following basic modes:
 * * While the host (host processor) is "Awake"
 * * While the host (host processor) is in CPU Sleep or System Deep Sleep mode.
 *
 * It is possible to enable and configure these modes based on
 * the Sleep status of the application CPU.
 *
 *******************************************************************************
 * \subsubsection group_lpa_p2_hostautoreply Host Auto Reply
 *******************************************************************************
 *
 * Host Auto Reply is a power-saving and network traffic reduction feature.
 * Using the ARP Offload Host Auto Reply feature, the device answers
 * host ARP requests, without broadcasting to the network. If a host
 * generated ARP request results in a cache hit in the device ARP cache,
 * the device fabricates an ARP  response and not forward the request
 * to the network.
 *
 * This may be useful when the host ARP cache is cleared to disable
 * the host ARP table entry timers before entering the System Deep Sleep mode.
 * When the host is woken up, if the host ARP cache queries in its own
 * network stack and results in a cache miss, the host ARP request will
 * be sent to the device. If the ARP request results in a cache hit in
 * the device, the device responds without soliciting the network.
 * As long as the device ARP cache entry is not expired, the device 
 * fabricates an ARP response to the host, which therefore reduces the broadcast traffic.
 *
 * The ARP agent is enabled by setting the ARP Offload Agent flag and
 * ARP Offload Enable in the Device Configurator. The ARP agent 
 * stores the "ARP IP : MAC" combos in the device ARP cache. The device
 * ARP cache is filled with IP:MAC combos when ARP offload and ARP agent
 * are enabled and the network has responded to a host ARP request.
 * There is an "age out" value that you can set in the ARP offload
 * configuration to determine the length of time the ARP cache entry
 * is valid. This ensures that the WLAN ARP cache is updated appropriately.
 *
 * The size of the WLAN device ARP cache is 16. The host network stack
 * maintains an ARP cache regardless if the WLAN device ARP agent is turned
 * ON or not.
 *
 * \image html ARP2.png height=550px
 *
 *******************************************************************************
 * \subsubsection group_lpa_p2_peerautoreply Peer Auto Reply
 *******************************************************************************
 *
 * Peer Auto Reply is a power-saving feature that allows the Wi-Fi device
 * to reply to ARP request from the network peers without waking the host. 
 * This can be enabled by enabling the 'ARP Offload Enable' and 
 * 'Peer Auto Reply' flags in the Device Configurator. Once the system connects
 * to the network, the host instructs the Wi-Fi device to reply to ARP 
 * requests on its behalf while the host is in low-power state.
 *
 * The maximum number of entries for this feature is set to 8 (defined
 * in the device firmware and cannot be modified).
 *
 * \image html ARP3.png height=600px
 *
 *******************************************************************************
 * \subsubsection group_lpa_p2_hostpeerautoreply Host and Peer Auto Reply
 *******************************************************************************
 *
 * \ref group_lpa_p2_hostautoreply and \ref group_lpa_p2_peerautoreply
 * features can be enabled together for the application CPU Awake mode.
 *
 *******************************************************************************
 * \subsubsection group_lpa_p2_hostsnoop Host IP Snoop
 *******************************************************************************
 *
 * When enabled, the Snoop facility watches for ARP responses from the
 * host to the network, and caches them in the WLAN device host IP table.
 * The size of this table is 8 entries, which allows for the device to
 * support multiple IP addresses.
 *
 *******************************************************************************
 * \subsubsection group_lpa_p2_arp_offload_qsg Quick start guide
 *******************************************************************************
 * This quick start guide demonstrates how to configure and use 
 * the ARP offload feature in the FreeRTOS environment and its impact 
 * on system power consumption.
 *
 * Do the following to set up the wifi-low-power example
 * and enable the ARP offload feature:
 *
 * 1. Refer readme of <a href="https://github.com/Infineon/mtb-example-anycloud-wlan-lowpower.git"><b>
 * mtb-example-anycloud-wlan-lowpower</b></a> for project creation. 
 * 2. Refer section \ref group_quick_start_guide for LPA changes in the application.
 * 3. Configure the ARP offload. The easiest way to configure ARP offload
 *    is to use the ModusToolbox&trade; Device Configurator. \n
 *    * Refer section \ref section_lpa_hostwake to verify WLAN_HOST_WAKE pin configurations using the Device Configurator.
 *    * In design.modus, switch to the connectivity device "CYW943012WKWBG" tab 
 *      (if the CY8CKIT_062S2_43012 Board is used).
 *    * Enable Power-\>Wi-Fi.
 *    * In "Wi-Fi - Parameters" pane enable "Host Wake Configuration" and 
 *      set "Host Device Interrupt Pin" to "CYBSP_WIFI_HOST_WAKE".
 *    * Enable ARP offload.
 *    * Set "ARP offload Feature(s)" to "Peer Auto Reply".
 *    * Enable "Snoop Host IP From Traffic When ARP Offload Enabled".
 *    * Set "ARP Offload Cache Entries Expire after (s)" to 1200.
 *    * Save the configuration to generate the necessary code.
 *
 * 4. Refer mtb-example-anycloud-wlan-lowpower <a href="https://github.com/Infineon/mtb-example-anycloud-wlan-lowpower/blob/master/README.md"><b>readme</b></a> to build the project and program the board.
 *    The following command is an example for the CY8CKIT_062S2_43012 Board, using GCC_ARM as the toolchain:
 *    \code
 *    make getlibs
 *    make program TARGET=CY8CKIT-062S2-43012 TOOLCHAIN=GCC_ARM
 *    \endcode
 *
 *    When the modified mtb-example-anycloud-wlan-lowpower starts, the console output 
 *    shows that it connects to the specified 
 *    above Wi-Fi AP, and then the PSoC&trade; 6 MCU goes to 
 *    System Deep Sleep mode.
 *    \code
 *    
 *    =======================================================
 *    CE230106 - Example: WLAN Lowpower
 *    =======================================================
 *
 *    WLAN MAC Address : D4:4D:A4:A0:02:A4
 *    WLAN Firmware    : wl0: Jan 27 2020 21:57:29 version 13.10.271.236 (5a526db) FWID 01-61e2b002
 *    WLAN CLM         : API: 18.2 Data: 9.10.0 Compiler: 1.36.1 ClmImport: 1.34.1 Creation: 2020-01-27 21:54:33
 *    WHD VERSION      : v1.80.0 : v1.80.0 : GCC 7.2 : 2020-03-10 04:09:17 -0500
 *    Info:Connecting to AP
 *    IP Address 10.0.0.201 assigned
 *    Info:Successfully joined Wi-Fi network 'SSID'.
 *    Info:Beacon period = 100, DTIM period = 3
 *    
 *    Network Stack Suspended, MCU will enter DeepSleep power mode
 *    Resuming Network Stack, Network stack was suspended for 15253ms
 *    
 *    =====================================================
 *    WHD Stats..
 *    tx_total:64, rx_total:71, tx_no_mem:0, rx_no_mem:0
 *    tx_fail:0, no_credit:0, flow_control:0
 *    Bus Stats..
 *    cmd52:2286, cmd53_read:366, cmd53_write:559
 *    cmd52_fail:0, cmd53_read_fail:0, cmd53_write_fail:0
 *    oob_intrs:72, sdio_intrs:147, error_intrs:0, read_aborts:0
 *    =====================================================
 *    Network is active. Resuming network stack
 *    Network Stack Suspended, MCU will enter DeepSleep power mode
 *    \endcode
 *
 * 5. Check the board operation. Use a PC to connect to the same
 *    Wi-Fi AP as the PSoC&trade: 6 board.
 *
 *    * Send a "ping" command to the board and observe in the serial 
 *      terminal that the PSoC&trade; 6 MCU wakes up for each command:
 *      \code
 *      C:\>ping -n 3 10.0.0.201
 *    
 *      Pinging 10.0.0.201 with 32 bytes of data:
 *      Reply from 10.0.0.201: bytes=32 time=319ms TTL=255
 *      Reply from 10.0.0.201: bytes=32 time=233ms TTL=255
 *      Reply from 10.0.0.201: bytes=32 time=151ms TTL=255
 *    
 *      Ping statistics for 10.0.0.201:
 *        Packets: Sent = 3, Received = 3, Lost = 0 (0% loss),
 *      Approximate round trip times in milli-seconds:
 *        Minimum = 151ms, Maximum = 319ms, Average = 234ms
 *
 *      <Terminal logs >
 *      Resuming Network Stack, Network stack was suspended for 2456ms
 *      
 *      =====================================================
 *      WHD Stats..
 *      tx_total:174, rx_total:198, tx_no_mem:0, rx_no_mem:0
 *      tx_fail:0, no_credit:0, flow_control:0
 *      Bus Stats..
 *      cmd52:2638, cmd53_read:1001, cmd53_write:796
 *      cmd52_fail:0, cmd53_read_fail:0, cmd53_write_fail:0
 *      oob_intrs:199, sdio_intrs:401, error_intrs:0, read_aborts:0
 *      =====================================================
 *      Network is active. Resuming network stack
 *      
 *      Network Stack Suspended, MCU will enter DeepSleep power mode
 *      \endcode
 *
 *    * Send an "arping" command as follows and observe that the PSoC&trade; 6 MCU 
 *      is in Deep Sleep mode.
 *      \code
 *      C:\>arp-ping.exe -n 3 10.0.0.201
 *      Reply that D4:4D:A4:A0:02:A4 is 10.0.0.201 in 113.078ms
 *      Reply that D4:4D:A4:A0:02:A4 is 10.0.0.201 in 1115.498ms
 *      Reply that D4:4D:A4:A0:02:A4 is 10.0.0.201 in 1113.863ms
 *
 *      Ping statistics for 10.0.0.201/arp
 *         3 probes sent.
 *         3 successful, 0 failed.
 *      Approximate trip times in milli-seconds:
 *         Minimum = 113.078ms, Maximum = 1115.498ms, Average = 780.813ms
 *      \endcode
 *
 *      Use any available ARPping tool. As an example:
 *        * Windows: https://www.elifulkerson.com/projects/arp-ping.php
 *        * Mac : http://macappstore.org/arping/
 *        * linux : sudo apt install arping; arping [ip address]
 *
 *      Because the WLAN device’s ARP cache is empty on the initial ARP request 
 *      from the peer, it looks up the IP details from the host and updates 
 *      its ARP cache. This causes the host to wake up because of network 
 *      activity between the host MCU and the WLAN device. On subsequent 
 *      ARP requests from the peer, the host remains asleep. The WLAN device 
 *      continues to respond to the ARP request as it has the ARP data 
 *      available in its ARP cache.
 *
 *    * Verify the ARP offload is working as desired. Refer to 
 *      the \ref section_lpa_measurement section for corresponding 
 *      instructions. The following oscilloscope screen capture 
 *      shows current measurement with the ARP offload enabled:
 *
 *      \image html FreeRTOS_ARP_Enable.png height=500px
 * 
 *      While the WLAN device (purple graph) high spikes responds to each request, 
 *      the PSoC&trade; 6 host (blue graph) is in System Deep Sleep mode.
 *
 * 6. Disable the ARP Offload feature and observe that the PSoC&trade; 6 host
 *    wakes up on each request.
 *    Launch the ModusToolbox&trade; Device Configurator and open the appropriate 
 *    design.modus file. Select the "CYW943012WKWBG" tab, 
 *    select Power-\>Wi-Fi personality, and disable ARP offload by 
 *    deselecting the check box next to "ARP offload".
 *    Save the configuration. Then, build and program the application.
 *    With ARP offload disabled, the host MCU (PSoC&trade; 6) wakes up for 
 *    every ARP request.
 *
 *    \image html FreeRTOS_ARP_Disable.png height=500px
 *
 * <b>Note:</b> \n
 * For CY8CEVAL-062S2-CYW43022CUB device, ARP offload is enabled by default 
 * with configuration set to \ref group_lpa_p2_hostpeerautoreply. Hence for this device, 
 * ARP offload configurations are not present in the ModusToolbox&trade; Device Configurator.
 *
 *******************************************************************************
 * \subsection group_lpa_p2_packet_filter Wi-Fi Packet filter offload
 *******************************************************************************
 *
 * Packet filters allow the host processor to limit which types of
 * packets get passed up to the host processor from the WLAN subsystem.
 * This is useful to keep out unrequired packets from the network
 * that might otherwise wake the host out of a power-saving System
 * Deep Sleep mode or prevent it from entering the System Deep Sleep mode.
 *
 * Packet filters are useful when:
 * * Trying to keep the host processor in the System Deep Sleep mode for as long as
 *   possible.
 * * Trying to get the host processor into System Deep Sleep mode as soon as
 *   possible.
 *
 * Whenever a WLAN packet is destined for the host, the WLAN processor
 * must awaken the host (if it is asleep) so it can retrieve the packet
 * for processing. Often times the host network stack processes the
 * packet only to discover that the packet should be thrown away, because
 * it is not needed. For example, it is destined for a port or service that
 * is being used. Packet filters allow these types of packets to be filtered
 * and discarded by the WLAN processor so the host is not interrupted.
 *
 *******************************************************************************
 * \subsubsection group_lpa_p2_filtertypes Filter types
 *******************************************************************************
 *
 * The following types of packet filters are supported, based on
 * a standard IP Stack:
 *
 * \image html PacketFiletr.png height=200px
 *
 * A general purpose STA might want these types of packets:
 * * Ether type ARP
 * * IP protocol ICMP
 * * TCP port 22 (ssh)
 * * UDP port 68 (DHCP)
 *
 * Other packet types are highly dependent on what applications and
 * data types are in use. Multiple filters may be configured to
 * operate simultaneously.
 *
 * <b>Network layer/Ether types</b>
 *
 * The EtherType filter is based on a 16-bit ethertype field
 * present in ethernet packets, and it is the lowest level of the TC/IP stack.
 * A technical description and list of choices can be found in
 * numerous places, such as <a href="https://en.wikipedia.org/wiki/EtherType">
 * <b>Ether Type</b></a>.
 *
 * The most commonly used protocols (and most useful filters) are:
 * * IP (value 0x800)
 * * ARP (value 0x806).
 *
 * Filtering on IP would match any and all IP packets coming
 * from the network.  This is a very coarsely grained filter and it will
 * include all ICMP, TCP, and UDP packets as shown in the diagram above.
 * Filtering on ARP is finer grained, and it will only match on ARP packets.
 * Filtering all IP will have an enormous impact due to the large
 * number of packets it will match and is generally not recommended
 * for general usage.
 *
 * Valid EtherType filters consist of a 16-bit number greater
 * than or equal to 0x800.
 *
 * <b>Transport layer/IP protocols</b>
 *
 * The next layer up the stack is the Transport layer, which consists
 * of various IP protocols such as TCP, UDP, and ICMP. Discussions of
 * the protocols themselves are outside the realm of this document,
 * but are widely available. A list of protocol numbers is also
 * widely available, including:
 * <a href="https://en.wikipedia.org/wiki/List_of_IP_protocol_numbers">
 * <b>IP Protocol numbers</b></a>.
 *
 * IP protocol filters consist of a single 8-bit number. No checking
 * is done to see if this number matches a well-known protocol because
 * it is possible for vendors to use proprietary numbers and protocols.
 *
 * Filtering on one IP protocol will only match that protocol.
 * For example, filtering on UDP would match a UDP packet, but not
 * ICMP or TCP packets. However, matching on TCP is still very coarsely
 * grained and will likely include the majority of packets destined
 * for the host processor (depending on your environment). Port numbers
 * are the next level of filter refinement.
 *
 * <b>Applications Layer / TCP & UDP Port Numbers</b>
 *
 * The applications layer distinguishes between various applications
 * based on port numbers. These are simply well-known numbers used
 * to identify various TCP and UDP-based applications. Technical discussions
 * about port numbers and list of numbers that belong to which applications
 * can be found widely on the internet, for example:
 * <a href="https://en.wikipedia.org/wiki/List_of_TCP_and_UDP_port_numbers">
 * <b>TCP and UDP port numbers</b></a>.
 *
 * Port filters are 16-bit port numbers as described above. With a port
 * number filter, you can filter on, for example, only ssh packets (port 22),
 * only on ftp packets (port 20), or any other of the many applications
 * listed above.
 *
 * Due to the large number and constantly changing port definitions,
 * the OLM makes no attempt to sanity check these values.
 *
 * IP packets have both source and destination ports. Destination ports
 * are the well-known port numbers described in the link above and
 * generally the most useful.  Source ports describe the temporary,
 * ephemeral port numbers used by the host sending the packets. These
 * are generated on the fly and not well known. Because they are not
 * known ahead of time, creating a filter for them is difficult.
 * Port ranges can be used to match a wide range of source ports
 * to cover this case.
 *
 * Port filters support optional port ranges. A port range describes
 * a start and end such that any port number within that start-end
 * range will match.  This is most useful for matching a short-lived
 * ephemeral source port number since it will not be known ahead of time.
 *
 * Because both TCP and UDP use port numbers, filters are configured
 * to match one or the other (TCP or UDP). If both TCP and UDP need
 * to be filtered for a particular port, two filters can be created;
 * one for each.
 *
 *******************************************************************************
 * \subsubsection group_lpa_p2_keeptoss Action (keep vs toss)
 *******************************************************************************
 *
 * Filters can be configured to either keep (send to host) or toss
 * (drop/discard). When configured to drop, only the specified packets
 * are dropped, while any others not specifically filtered are passed
 * to the host. Likewise, when configured to keep packets, only the
 * specified packets are passed to the host, and the rest are discarded.
 * The most helpful model is to use only 'keep' filters. In other
 * words, use 'keep' filters to specify the complete list of packet
 * types that the host is interested in, and discard the rest. This works
 * because it is much simpler to list the packets that the host
 * wants to receive than to create a complete list of packets
 * it does not want.
 *
 * When using keep filters, use care to allow enough packets
 * through for networking protocols to work properly. For instance,
 * the processor must be able to join a network, get a DHCP address,
 * respond to ARP requests, and possibly share network keys. Not creating
 * enough keep filters to allow all these type of packets through will
 * prevent the host from joining the network. A reasonable minimal
 * set of keep filters includes:
 * * Keep, EtherType 0x806             		 \#Allow ARP through
 * * Keep, EtherType 0x888e            		 \#Allow security packets through
 * * Keep, Port Filter: UDP Dest Port 68     \#Allow DHCP packets through.
 *
 * The following additional filters might also be needed depending on the application:
 * * Keep, Port Filter: UDP, Source Port 53   \#Allow DNS packet (use source port)
 * * Keep, Port Filter: TCP Dest Port 8883    \#Allow Secure MQTT packets
 * * Keep, Port Filter: TCP Dest Port 1883    \#Allow Open MQTT packets
 *
 * These 'keep' filters will keep only the packet types as described;
 * all other traffic will be discarded so it is critical to use
 * enough filters to allow the application to receive the traffic it
 * needs. This type of configuration is useful when the system receives
 * many different kinds of traffic and it is easier to describe the traffic
 * to be kept rather than the traffic to be discarded.
 *
 * Alternatively, it is often simpler to describe the specific type of
 * traffic to be discarded and keeping everything else. For example, someone on
 * the network keeps pinging your machine (using ICMP packets) and
 * waking it. You want to block ICMP and keep everything else. In this
 * case, the following one filter is needed:
 * * Discard, IPType 1 				\# toss ICMP packet
 *
 * This discards all incoming ping/ICMP packets and keep all other traffic.
 *
 * There are no minimal filters for toss filters because the system 
 * filters the specific packets and everything else gets passed up to the host.
 *
 * <b>Note</b> 
 * All active filters need to be of the same type
 * (keep or toss). Mixing active keep and toss filters will cause
 * unexpected behaviors.
 *
 * The following diagram shows packet filter offload with ICMP packet configured to be discarded:
 *
 *    \image html pf_ping_packet_discard.png height=500px
 *
 *******************************************************************************
 * \subsubsection group_lpa_p2_sleepwake When Active (Sleep vs Wake)
 *******************************************************************************
 *
 * Filters can be configured to be active either when the host processor
 * is active or asleep, or both. When a filter is not active, it has
 * no effect. When the system goes into sleep mode, it disables
 * all wake filters and enables all sleep filters just before entering
 * sleep. When waking, all sleep filters are disabled and wake filters
 * enabled.
 * * Wake filters allow the host to go into sleep mode faster.
 * * Sleep filters help the host stay asleep longer.
 *
 * Normally, without packet filters, the WLAN subsystem passes all packets
 * destined for the host up to the host network stack for processing.
 * If the host is in System Deep Sleep, the WLAN subsystem
 * will first wake the host and then pass the packet up to the stack.
 * From there, the network stack will pass the packet on to the application
 * that is listening for that type of packet. If no applications are
 * listening for that type of packet, the network stack drops the packet.
 *
 * For example,
 * * An http packet arrives on port 80, but there is no http server
 *   running that would read the packet (port 80 is default http port).
 * * An ssh packet arrives on port 22, but there is no ssh server
 *   running that would read that packet (port 22 is default ssh port).
 *
 * In both cases, the host would awaken from its battery-saving
 * System Deep Sleep mode to drop a packet. It is not hard to imagine
 * scenarios where traffic your application doesn't want or doesn't care
 * about ends up penalizing your battery usage by constantly waking the
 * host from System Deep Sleep. Alternatively, these unwanted packets
 * can also prevent the host from entering System Deep Sleep mode in the
 * first place. The host has an idle sleep threshold. When the host
 * has been idle longer than that threshold, it puts itself to sleep.
 * Processing unwanted packets (even just dropping them) causes the
 * host to come out of idle and prevents it from reaching the idle sleep
 * threshold, preventing the host from entering sleep. In both
 * cases, traffic patterns keep the processor awake, burning power.
 *
 *******************************************************************************
 * \subsubsection group_lpa_p2_packet_filter_qsg Quick start guide
 *******************************************************************************
 *
 * This quick start guide demonstrates how to configure and use 
 * the Packet Filter feature in the FreeRTOS environment and its impact 
 * on the system power consumption. 
 *
 * Do the following to setup the wifi-low-power 
 * example with enabling the Packet Filter feature:
 *
 * 1. Refer readme of <a href="https://github.com/Infineon/mtb-example-anycloud-wlan-lowpower.git"><b>
 * mtb-example-anycloud-wlan-lowpower</b></a> for project creation. 
 * 2. Refer section \ref group_quick_start_guide for LPA changes in the application.
 * 3. Configure Packet Filters. The easiest way to configure Packet Filters
 *    is to use the ModusToolbox&trade; Device Configurator. \n
 *    * Refer section \ref section_lpa_hostwake to verify WLAN_HOST_WAKE pin configurations using device configurator.
 *    * In design.modus, switch to the connectivity device "CYW943012WKWBG" tab 
 *      (in case the CY8CKIT_062S2_43012 board is used).
 *    * Enable Power-\>Wi-Fi.
 *    * In "Wi-Fi - Parameters" pane, enable "Host Wake Configuration" and 
 *      set "Host Device Interrupt Pin" to "CYBSP_WIFI_HOST_WAKE".
 *    * Disable ARP offload.
 *    * Enable "Add minimal set of keep filters", these filters allow ARP, DNS, DHCP, and
 *      802.1x security packets to wake up the host, and are needed to connect to Wi-Fi 
 *      AP, any other Wi-Fi packets are dropped by WLAN chip and not forwarded 
 *      to the host MCU (PSoC&trade; 6)
 *    * Save the configuration to generate the necessary code.
 *
 * 4. Refer mtb-example-anycloud-wlan-lowpower <a href="https://github.com/Infineon/mtb-example-anycloud-wlan-lowpower/blob/master/README.md"><b>readme</b></a> to build the project and program the board.
 *    The following command is an example for the CY8CKIT_062S2_43012 Board, using GCC_ARM as the toolchain:
 *    \code
 *    make getlibs
 *    make program TARGET=CY8CKIT-062S2-43012 TOOLCHAIN=GCC_ARM
 *    \endcode
 *
 *    When the modified mtb-example-anycloud-wlan-lowpower starts, the console output 
 *    shows that it connects to the specified 
 *    above Wi-Fi AP, and then the PSoC&trade; 6 MCU goes to 
 *    System Deep Sleep mode.
 *    \code
 *    
 *    =======================================================
 *    CE230106 - Example: WLAN Lowpower
 *    =======================================================
 *
 *    WLAN MAC Address : D4:4D:A4:A0:02:A4
 *    WLAN Firmware    : wl0: Jan 27 2020 21:57:29 version 13.10.271.236 (5a526db) FWID 01-61e2b002
 *    WLAN CLM         : API: 18.2 Data: 9.10.0 Compiler: 1.36.1 ClmImport: 1.34.1 Creation: 2020-01-27 21:54:33
 *    WHD VERSION      : v1.80.0 : v1.80.0 : GCC 7.2 : 2020-03-10 04:09:17 -0500
 *    Info:Connecting to AP
 *    IP Address 10.0.0.201 assigned
 *    Info:Successfully joined Wi-Fi network 'SSID'.
 *    Info:Beacon period = 100, DTIM period = 3
 *    
 *    Network Stack Suspended, MCU will enter DeepSleep power mode
 *    Resuming Network Stack, Network stack was suspended for 15253ms
 *    
 *    =====================================================
 *    WHD Stats..
 *    tx_total:64, rx_total:71, tx_no_mem:0, rx_no_mem:0
 *    tx_fail:0, no_credit:0, flow_control:0
 *    Bus Stats..
 *    cmd52:2286, cmd53_read:366, cmd53_write:559
 *    cmd52_fail:0, cmd53_read_fail:0, cmd53_write_fail:0
 *    oob_intrs:72, sdio_intrs:147, error_intrs:0, read_aborts:0
 *    =====================================================
 *    Network is active. Resuming network stack
 *    Network Stack Suspended, MCU will enter DeepSleep power mode
 *    \endcode
 *
 * 5. Check the board operation. Use a PC to connect to the same
 *    Wi-Fi AP as the PSoC&trade; 6 board.
 *
 *    * Send "ping" command to the board and observe in serial terminal 
 *      that it does not wake up the PSoC&trade; 6 MCU because there is 
 *      no "keep" packet filter for ICMP pings, there is no response 
 *      for the pings:
 *      \code
 *      C:\>ping -n 3 10.0.0.201
 *   
 *      Pinging 10.0.0.201 with 32 bytes of data:
 *      Request timed out.
 *      Request timed out.
 *      Request timed out.
 *   
 *      Ping statistics for 10.0.0.201:
 *          Packets: Sent = 3, Received = 0, Lost = 3 (100% loss),
 *
 *      \endcode
 *
 *    * Send an "arping" command as follows and observe that the PSoC&trade; 6 MCU 
 *      is in Deep Sleep mode.
 *      Use any available ARPping tool. As an example:
 *        * Windows: https://www.elifulkerson.com/projects/arp-ping.php
 *        * Mac : http://macappstore.org/arping/
 *        * linux : sudo apt install arping; arping [ip address]
 *        \code
 *        C:\>arp-ping.exe -n 3 10.0.0.201
 *        Reply that D4:4D:A4:A0:02:A4 is 10.0.0.201 in 113.209ms
 *        Reply that D4:4D:A4:A0:02:A4 is 10.0.0.201 in 125.789ms
 *        Reply that D4:4D:A4:A0:02:A4 is 10.0.0.201 in 1114.333ms
 *   
 *        Ping statistics for 10.0.0.201/arp
 *            3 probes sent.
 *            3 successful, 0 failed.
 *        Approximate trip times in milli-seconds:
 *            Minimum = 113.209ms, Maximum = 1114.333ms, Average = 451.111ms
 *        \endcode
 *
 *        Observe that PSoC&trade; 6 MCU wakes up for each command because there
 *        is "keep" packet filter for ARP pings, the ARP pings
 *        are responded back:
 *        \code
 *        Resuming Network Stack, Network stack was suspended for 2456ms
 *        
 *        =====================================================
 *        WHD Stats..
 *        tx_total:174, rx_total:198, tx_no_mem:0, rx_no_mem:0
 *        tx_fail:0, no_credit:0, flow_control:0
 *        Bus Stats..
 *        cmd52:2638, cmd53_read:1001, cmd53_write:796
 *        cmd52_fail:0, cmd53_read_fail:0, cmd53_write_fail:0
 *        oob_intrs:199, sdio_intrs:401, error_intrs:0, read_aborts:0
 *        =====================================================
 *        Network is active. Resuming network stack
 *        
 *        Network Stack Suspended, MCU will enter DeepSleep power mode
 *        \endcode
 *
 * 6.  Verify that the packet Filter is working as desired. Refer to the \ref section_lpa_measurement section for corresponding instructions.
 *     The following oscilloscope screen capture shows current measurement with the Packet Filter enabled:
 *
 *     i. ARP Ping :
 *        This wakes up the host as packet filter for ARP is configured.
 *
 *      \image html FreeRTOS_PF_ARP_ping.png height=500px
 *
 *     ii. Ping :
 *        This doesnot wakeup the host as ICMP packet is not configured as packet filter type.
 *      \image html FreeRTOS_PF_ping_nowake.png height=500px
 *
 *******************************************************************************
 * \subsection group_lpa_p2_tcp_keepalive Wi-Fi TCP keepalive offload
 *******************************************************************************
 * TCP keepalive offload part of the LPA is designed to improve the power consumption of the connected system by reducing the time the host needs to stay awake to support TCP keepalive request.
 * This document describes how to enable TCP keepalive offload and configure four different sockets for TCP keepalive that can be incorporated into the project from
 * <a href="https://github.com/Infineon/lpa">
 * <b>Infineon GitHub LPA Middleware</b></a>.
 *
 * TCP keepalive maintains idle TCP connections by periodically passing packets between the client and server. If either the client or server does not respond to a packet,
 * the connection is considered inactive and is terminated. This helps in pruning dead connections. Typically TCP keepalives are sent every 45 or 60 seconds on an idle TCP connection,
 * and the connection is dropped after three sequential ACKs are missed.
 * This means host MCU has to wake up periodically to send TCP keepalive packet to maintain TCP connection during idle state.
 *
 * TCP keepalive offload helps in moving this functionality to WLAN firmware so that host MCU does not need to wake up periodically to send/receive TCP keepalive packets.
 * This functionality is offloaded only when host MCU goes to Sleep mode and network stack is suspended.
 *
 * \image html TCP_Keepalive_basic_graph.png height=500px
 *
 *******************************************************************************
 * \subsubsection group_lpa_p2_tcp_keepalive_qsg Quick start guide
 *******************************************************************************
 *
 * This quick start guide demonstrates how to configure and use
 * the TCP keepalive offload feature in the FreeRTOS environment and its impact
 * on the system power consumption.
 *
 * Do the following to set up the wifi-low-power tcp keepalive offload
 * example and enable the TCP keepalive offload feature.
 *
 * 1. Refer readme of <a href="https://github.com/Infineon/mtb-example-anycloud-offload-tcp-keepalive.git"><b> 
 * mtb-example-anycloud-offload-tcp-keepalive</b></a> for project creation. 
 * 2. Refer section \ref group_quick_start_guide for LPA changes in the application.
 * 3. Configure TCP keepalive offload. The easiest way to configure TCP keepalive offload
 *    is to use the ModusToolbox&trade; Device Configurator. \n
 *    * Refer section \ref section_lpa_hostwake to verify WLAN_HOST_WAKE pin configurations using the Device Configurator.
 *    * In design.modus, switch to the connectivity device "CYW943012WKWBG" tab 
 *      (in case the CY8CKIT_062S2_43012 board is used).
 *    * Enable Power-\>Wi-Fi.
 *    * In "Wi-Fi - Parameters" pane, enable "Host Wake Configuration" and 
 *      set "Host Device Interrupt Pin" to "CYBSP_WIFI_HOST_WAKE".
 *    * Enable TCP keepalive offload.
 *    * Configure Interval, Retry Interval, and Retry Count.
 *    * Configure Source port, Destination port, and Destination IP address (TCP server).
 *    * Save the configuration to generate the necessary code.
 *
 *   ii. Manual configuration steps
 *
 *    *  Read the OLM configuration from device configurator and make a copy of OLM device configurator \ref subsection_lpa_snippet_2
 *    *  search for "TKO" name in the device configurator list, if present then use it establish TCP connection \ref subsection_lpa_snippet_5
 *    *  If TCP Keep-alive offload is not present in device configurator then add TCP keep-alive offload list \ref subsection_lpa_snippet_3
 *    *  call the API to restart olm  \ref subsection_lpa_snippet_4
 *    *  Establish TCP connection  \ref subsection_lpa_snippet_5
 *    *  TCP keepalive offload will be enabled when host MCU goes to sleep \ref subsection_lpa_snippet_1
 *    *  <b>Note</b> The maximum supported TCP connections that can be offloaded to WLAN device is 4 (MAX_TKO) this is WLAN firmware limitation, if the API \ref subsection_lpa_snippet_5 is invoked
 *       more than 4 times then LIFO ( Last In First Out) Algorithm is followed where the MRU(Most Recently Used) TCP connections will only
 *       be offloaded to WLAN device.
 *
 * 7. Use a PC to connect to the same Wi-Fi Access Point as the PSoC&trade; 6 board. Run <a href="tcp_echo_server.py" rel="stylesheet" type="text/css" ><b>tcp_echo_server.py</b></a> in that PC to act as TCP Server
 *    \code
 *    python tcp_echo_server.py --port 3360
 *    \endcode
 *
 *
 * 8. Refer mtb-example-connectivity-offload-tcp-keepalive <a href="https://github.com/Infineon/mtb-example-connectivity-offload-tcp-keepalive/blob/master/README.md"><b>readme</b></a> to build the project and program the board.
 *    The following command is an example for the CY8CKIT_062S2_43012 board, using GCC_ARM as the toolchain:
 *    \code
 *    make getlibs
 *    make program TARGET=CY8CKIT-062S2-43012 TOOLCHAIN=GCC_ARM
 *    \endcode
 *
 *    When the modified mtb-example-connectivity-offload-tcp-keepalive starts, the console output 
 *    lists available Wi-Fi networks. It then connects to the specified 
 *    above Wi-Fi AP, and then the PSoC&trade; 6 MCU goes to 
 *    System Deep Sleep mode.
 *    \code
 *    
 *    Info: ============================================
 *    Info:  Example: WLAN TCP Keepalive Offload
 *    Info: ============================================
 *
 *    WLAN MAC Address : D4:4D:A4:A0:02:A4
 *    WLAN Firmware    : wl0: Jan 27 2020 21:57:29 version 13.10.271.236 (5a526db) FWID 01-61e2b002
 *    WLAN CLM         : API: 18.2 Data: 9.10.0 Compiler: 1.36.1 ClmImport: 1.34.1 Creation: 2020-01-27 21:54:33
 *    WHD VERSION      : v1.80.0 : v1.80.0 : GCC 7.2 : 2020-03-10 04:09:17 -0500
 *    Wi-Fi initialization is successful
 *    Join to AP: SSID
 *    IP Address 192.168.43.11 assigned
 *    Successfully joined wifi network SSID
 *    Taking TCP Keepalive configuration from the Generated sources.
 *    TCP remote IP Address f2ba8c0  remote port:3360
 *    TCP Connection Established Successfully ctx:8011050
 *    Socket[0]: Created connection to IP 192.168.43.15, local port 3353, remote port 3360
 *    Skipped TCP socket connection for socket id[1]. Check the TCP Keepalive configuration.
 *    Skipped TCP socket connection for socket id[2]. Check the TCP Keepalive configuration.
 *    Skipped TCP socket connection for socket id[3]. Check the TCP Keepalive configuration.
 *    Low power task created
 *    whd_tko_toggle: Successfully enabled
 *    
 *    Network Stack Suspended, MCU will enter DeepSleep power mode
 *    \endcode
 *
 * 9. Verify that the TCP keepalive offload is working as desired. Refer to the \ref section_lpa_measurement section for
 *    corresponding instructions. The following oscilloscope screen capture shows current measurement with
 *
 * <b>TCP keepalive offload enabled with 20 seconds interval configuration and WLAN DTIM configured as 3 :</b>
 *      \image html FreeRTOS_TKO_Enable.png height=500px
 *
 *******************************************************************************
 * \subsection group_lpa_p2_cc Wi-Fi low power configuration considerations
 *******************************************************************************
 *
 * Refer to the section \ref group_device_configurator_flow to configure
 * MCU low power using desgin.modus
 * To open the ModusToolbox&trade; Device Configurator, right-click on the project
 * in the Project Explorer pane in the ModusToolbox&trade; IDE, and select
 *   ModusToolbox&trade; Configurators > Device Configurator.
 *
 * 1. Select the Wi-Fi radio device tab (e.g., CYW4343WKUBG).
 * 2. Enable the Wi-Fi personality and go to the Wi-Fi Parameters pane
 *    to configure the LPA middleware.
 * 3. Set Host Wake Configuration. Enable and assign the Host Device
 *    Interrupt Pin. Refer to the kit datasheet to find out which pin
 *    is routed on board PCB.
 * 4. Set ARP Offload. Enable and configure the LPA with
 *    the desired parameters.
 * 5. Set AWS MQTT Filters. Enable MQTT TLS Filter to enable
 *    corresponding filter.
 * 6. Configure MQTT TLS Filter Configuration parameters with desired values.
 * 7. Set Packet Filters. Enable Filter Configuration \<N\> to get access to
 *    the filter parameters.
 * 8. Configure the enabled filter on the previous step with
 *    desired parameters.
 * 9. Perform File-\>Save to generate initialization code.
 *
 *
 * \image html WIFI_Personality.png
 *
 * For CY8CEVAL-062S2-CYW43022CUB device, do below additional changes.
 *
 * 1. Select the Wi-Fi radio device tab (e.g., CYW43022CUB).
 * 2. ULP support is enabled by default. Set the wait time.
 * 3. Perform File-\>Save to generate initialization code.
 *
 * \image html ULP_Personality.png
 *
 * After saving the configuration file, the generated code is available under
 * the GeneratedSource folder, located next to the design.modus file
 * in the BSP:
 * * C Data File:
 *   /GeneratedSource/cycfg_connectivity_wifi.c
 * * C Header File:
 *   /GeneratedSource/cycfg_connectivity_wifi.h
 *
 *********************************************************************************
 * \subsubsection group_lpa_p2_cc_host_wake Host Wake Configuration Notes
 *********************************************************************************
 * By Default, WLAN host wake is configured properly and below device configurator support allows user to modify it to different GPIOs.
 * But if user selects <b><i>Host Wake Configuration Enable</i></b> checkbox and doesnot select any pin for <b><i>Host Device Interrupt Pin</i></b>, System will Hang and this should be avoided by the user
 *
 * \image html hostwake_MTConfigure.png height=150px
 *
 *******************************************************************************
 * \subsubsection group_lpa_p2_cc_limitations Limitations
 *******************************************************************************
 *
 * 1. The maximum number of filters is ultimately dictated by free memory on
 * the Wi-Fi chipset. More memory will allow more filters. Different Wi-Fi chips
 * running FW from different branches will likely have slightly different
 * maximum numbers of filters. It is possible to add 20 filters on a
 * CY8CKIT_062S2_43012 kit without any issue.
 *
 * 2. Network stack suspend/resume functionality, which is called in the main.c function, enables the host MCU
 * to remain in Deep Sleep for longer durations (wake up only for external events).
 * (Maximum sleep-time possible is 36 hours (1.5 days). This means the host MCU will wake up without an interrupt from an external event after 36 hours
 * and then immediately go back to sleep. This has a very low power impact.
 * 
 ********************************************************************************
 * \subsubsection group_lpa_p2_ipv6_filter IPv6 Packet Filter for MCU Deep Sleep
 ********************************************************************************
 * * MCU will wake for IPv6 packets intermittently, So add packet filter for IPv6 packets to avoid unnecessary wakeup. \n
 *    How to configure packet Filter for IPv6 packets: \n
 *    * Navigate to ModusToolbox&trade; installation folder and launch
 *      the ModusToolbox&trade; Device Configurator
 *      (\<install_dir\>/tools_3.1/device-configurator).
 *    * Select File-\>Open, navigate to the board's design.modus file,
 *      and open it:
 *    * Switch to the connectivity device "CYW943012WKWBG" tab
 *      (in case the CY8CKIT_062S2_43012 board is used).
 *    * Enable Power-\>Wi-Fi.
 *    * In "Wi-Fi - Parameters" pane enable "Host Wake Configuration" and
 *      set "Host Device Interrupt Pin" to "CYBSP_WIFI_HOST_WAKE".
 *    * Select Enable Packet Filter Configuration 0
 *    * Select Filter Type as "Ether Type"
 *    * Select Action as "Discard"
 *    * Configure Ethertype as "0x86dd"
 *    * Save the configuration to generate the necessary code
 *
 ********************************************************************************
 * \subsubsection group_lpa_p2_wlan_lowpower WLAN low power configuration
 ********************************************************************************
 *   The WLAN firmware supports two different low-power modes: (legacy) 802.11 PS-Poll mode and high throughput powersave mode.
 *   Refer to <a href="https://Infineon.github.io/wifi-host-driver/API/group__wifipowersave.html"> <b> WLAN Powersave API </b></a>
 *
 *   These can be configured using the following Wi-Fi host driver APIs: \n
 *   1. <b> 802.11 PS-Poll mode </b>
 *   \code
 *   // ifp : Pointer to WHD Interface
 *
 *   whd_wifi_enable_powersave(ifp);
 *   \endcode
 *   2. <b> High Throughout Powersave mode </b>
 *   \code
 *   // This macro specifies the duration in milliseconds for which the STA stays
 *   // awake after receiving frames from AP in PM2 mode. The delay value must be set
 *   // to a multiple of 10 and not equal to zero. Minimum value is 10u. Maximum
 *   // value is 2000u.
 *   // ifp : Pointer to WHD Interface
 *
 *   #define RETURN_TO_SLEEP_MS                (10u)
 *
 *   whd_wifi_enable_powersave_with_throughput(ifp, RETURN_TO_SLEEP_MS);
 *   \endcode
 *   
 *******************************************************************************
 * \subsubsection group_lpa_p2_cc_parameters Configuration parameters
 *******************************************************************************
 *
 * The following parameters and their mapping to macros are available:
 *
 * <table class="doxtable">
 *   <tr><th>Category</th>
 *       <th>Parameter</th>
 *       <th>Description</th>
 *       <th>Parameter values</th></tr>
 *   <tr><td>Host Wake Configuration</td>
 *       <td>Host Device Interrupt Pin</td>
 *       <td>Selects the host pin which is connected to Wi-Fi device's
 *           HOST WAKE signal.</td>
 *       <td>
 *             * Empty (default)
 *             * Any enabled pin from Pin tab</td></tr>
 *   <tr><td>Host Wake Configuration</td>
 *       <td>Wi-Fi Device Interrupt Pin</td>
 *       <td>Wi-Fi device GPIO_0 is reserved as device wake-up interrupt
 *           request pin (WL HOST WAKE).</td>
 *       <td>
 *             * Read Only</td></tr>
 *   <tr><td>ARP Offload</td>
 *       <td>ARP Offload When Host Awake</td>
 *       <td>Respond to ARP Requests when Awake.</td>
 *       <td>
 *             * Disabled (default)
 *             * Host Auto Reply
 *             * Peer Auto Reply
 *             * Host and Peer Auto Reply</td></tr>
 *   <tr><td>ARP Offload</td>
 *       <td>ARP Offload When Host Sleeping</td>
 *       <td>Respond to ARP Requests when Sleeping.</td>
 *       <td>
 *             * Disabled (default)
 *             * Peer Auto Reply</td></tr>
 *   <tr><td>ARP Offload</td>
 *       <td>Snoop Host IP from traffic when ARP Offload enabled</td>
 *       <td>The host IP address is snooped from an ARP Request.
 *           If disabled, the WLAN Device will need to be informed of
 *           the Host IP address when the network interface is configured
 *           (statically or dynamically via DHCP).</td>
 *       <td>
 *             * Off (default)
 *             * On</td></tr>
 *   <tr><td>ARP Offload</td>
 *       <td>ARP Offload Cache entries expire after (s)</td>
 *       <td>When the ARP cache table is offloaded from the host to the device,
 *           table entries are subject to an aging value called "peer age".</td>
 *       <td>
 *             * 1200 (default)
 *             * 1-4294967295</td></tr>
 *   <tr><td>MQTT Filter Configuration</td>
 *       <td>Filter ID</td>
 *       <td>Filter ID</td>
 *       <td>
 *             * 0 (for MQTT Filter)
 *             * 1 (for MQTT TLS Filter)</td></tr>
 *   <tr><td>MQTT Filter Configuration</td>
 *       <td>Action</td>
 *       <td>Filter can either pass up packets that match the filter to
 *           the host (Keep) or drop them so the host never
 *           gets them (Discard).</td>
 *       <td>
 *             * Keep (default)
 *             * Discard</td></tr>
 *   <tr><td>MQTT Filter Configuration</td>
 *       <td>When Active</td>
 *       <td>Defines when the filter is active, when the host is awake,
 *           when it goes to sleep, or both.</td>
 *       <td>
 *             * Sleep (default)
 *             * Wake
 *             * Always</td></tr>
 *   <tr><td>MQTT Filter Configuration</td>
 *       <td>Protocol</td>
 *       <td>Choose communication protocol.</td>
 *       <td>
 *             * TCP (default)
 *             * UDP</td></tr>
 *   <tr><td>MQTT Filter Configuration</td>
 *       <td>Direction</td>
 *       <td>Inbound - destination port, outbound - source port.</td>
 *       <td>
 *             * Destination Port (default)
 *             * Source Port</td></tr>
 *   <tr><td>MQTT Filter Configuration</td>
 *       <td>Port Number</td>
 *       <td>Either the single port to be filtered or the beginning of
 *           the block of contiguous numbers. When using a block,
 *           the starting port must be power of 2.</td>
 *       <td>
 *             * 8883 for TLS
 *             * 1883 for Non TLS</td></tr>
 *   <tr><td>Packet Filter Configuration \<N\></td>
 *       <td>Filter ID</td>
 *       <td>Filter ID</td>
 *       <td>
 *             * 0-21</td></tr>
 *   <tr><td>Packet Filter Configuration \<N\></td>
 *       <td>Filter Type</td>
 *       <td>Each port filter can specify either a single port or a block
 *           of port numbers.
 *             * Port Filter on a 16 bit UDP or TCP port number. Refer to
 *               \ref group_lpa_p2_filtertypes for details of
 *               port numbers.
 *             * Port Block Filter of port numbers (vs a single port).
 *               Envisioned to be used by short lived ephemeral ports on
 *               source port.
 *             * Ether Type accepts a 16 bit ethertype value to filter on.
 *               Refer to the \ref group_lpa_p2_filtertypes section
 *               (EtherTypes). Common values are 0x800 (IP) and 0x806 (ARP).
 *             * IP Type accepts an 8 bit IP Protocol number to filter on.
 *               Refer to the \ref group_lpa_p2_filtertypes section 
 *               (IPProtocols).</td>
 *       <td>
 *             * Port Filter
 *             * Port Block Filter
 *             * Ether Type
 *             * IP Type</td></tr>
 *   <tr><td>Packet Filter Configuration \<N\></td>
 *       <td>Action</td>
 *       <td>Filter can either pass up packets that match the filter to
 *           the host (Keep) or drop them so the host never
 *           gets them (Discard).</td>
 *       <td>
 *             * Keep (default)
 *             * Discard</td></tr>
 *   <tr><td>Packet Filter Configuration \<N\></td>
 *       <td>When Active</td>
 *       <td>Defines when the filter is active, when the host is awake,
 *           when it goes to sleep, or both.</td>
 *       <td>
 *             * Sleep (default)
 *             * Wake
 *             * Always</td></tr>
 *   <tr><td>Packet Filter Configuration \<N\></td>
 *       <td>Protocol</td>
 *       <td>Chooses communication protocol. Filter can be constrained to
 *           either TCP or UDP. To match either TCP or UDP use two filters,
 *           one for each.</td>
 *       <td>
 *             * TCP (default)
 *             * UDP</td></tr>
 *   <tr><td>Packet Filter Configuration \<N\></td>
 *       <td>Direction</td>
 *       <td>Inbound - destination port, outbound - source port. Packets
 *           have both a source and destination port. This specifies which
 *           one to use. A single port usually uses the destination port.</td>
 *       <td>
 *             * Destination Port (default)
 *             * Source Port</td></tr>
 *   <tr><td>Packet Filter Configuration \<N\></td>
 *       <td>Port Number</td>
 *       <td>Either the single port to be filtered or the beginning of
 *           the block of contiguous numbers. When using a block, the
 *           starting port must be a power of 2.</td>
 *       <td>
 *             * 1024 for Packet filter (default)
 *             * 0-65535</td></tr>
 *   <tr><td>Packet Filter Configuration \<N\></td>
 *       <td>Range</td>
 *       <td>Indicates the size of the block of port numbers, must be of
 *           the form (2^y - 1) and less than Port Number
 *           (Filter Type = Port Filter Block).
 *           0 indicates just a single port.</td>
 *       <td>
 *             * 1023 (default)
 *             * 0-65535</td></tr>
 *   <tr><td>Packet Filter Configuration \<N\></td>
 *       <td>Ether Type</td>
 *       <td>Enter EtherType value for desired protocol
 *           (Filter Type = Ether Type).</td>
 *       <td>
 *             * 0 (default)
 *             * 0-65535</td></tr>
 *   <tr><td>Packet Filter Configuration \<N\></td>
 *       <td>IP protocol</td>
 *       <td>Enter desired IP protocol number (Filter Type = IP Type).</td>
 *       <td>
 *             * 0 (default)
 *             * 0-255</td></tr>
 *   <tr><td>TCP Keepalive Offload Configuration \<N\></td>
 *       <td>Interval</td>
 *       <td>Enter desired Keepalive Request interval in seconds</td>
 *       <td>
 *             * 20 (default)
 *             * 0-65535</td></tr>
 *   <tr><td>TCP Keepalive Offload Configuration \<N\></td>
 *       <td>Retry Interval</td>
 *       <td>Keepalive retry interval in seconds if Keepalive ack is not received</td>
 *       <td>
 *             * 3 (default)
 *             * 0-65535</td></tr>
 *   <tr><td>TCP Keepalive Offload Configuration \<N\></td>
 *       <td>Retry Count</td>
 *       <td>Retry count in seconds if Keepalive ack is not received</td>
 *       <td>
 *             * 3 (default)
 *             * 0-65535</td></tr>
 *   <tr><td>TCP Keepalive Offload Configuration \<N\></td>
 *       <td>Source port</td>
 *       <td>TCP Keepalive Source port number</td>
 *       <td>
 *             * 0 (default)
 *             * 0-65535</td></tr>
 *   <tr><td>TCP Keepalive Offload Configuration \<N\></td>
 *       <td>Destination port</td>
 *       <td>TCP Keepalive Destination port number</td>
 *       <td>
 *             * 0 (default)
 *             * 0-65535</td></tr>
 *   <tr><td>TCP Keepalive Offload Configuration \<N\></td>
 *       <td>Destination IP address</td>
 *       <td>TCP Keepalive Destination IP address </td>
 *       <td>
 *             * 0.0.0.0 (default)</td></tr>
 * </table>
 *
 *******************************************************************************
 * \section group_lpa_p3 Part 3. Bluetooth&reg; Low Power
 *******************************************************************************
 *
 * The Bluetooth low-power feature enables the host to achieve its lowest power
 * consumption with enabled Bluetooth. The BT Low Power personality helps
 * configure the pin connections between the MCU host and BT device.
 *
 *******************************************************************************
 * \subsection group_lpa_p3_cc BT Low Power Configuration considerations
 *******************************************************************************
 *
 * Follow the below steps for BT Low Power Configuration considerations.
 * 1. Refer to the section \ref group_device_configurator_flow to configure
 * MCU low power using desgin.modus
 * 2. Select Wi-Fi radio device tab (for example, CYW4343WKUBG).
 * 3. Enable the Bluetooth&reg; resource and go to the Bluetooth&reg; Parameters pane to configure
 *    the LPA Middleware.
 * 4. Enable Bluetooth&reg; Wake Configuration.
 * 5. Choose a pin for Host-wake-up signal. Refer to the kit datasheet to
 *    find out which pin is routed on board PCB.
 * 6. Choose a pin for Device-wake-up signal and Device-wake-up trigger
 *    interrupt type. Refer to the kit datasheet to find out which pin is
 *    routed on board PCB.
 * 7. Select File-\>Save to generate initialization code.
 *
 *
 * \image html BT_Personality.png height=250px
 *
 * After saving the configuration file, the generated code is available under
 * the GeneratedSource folder, located next to the design.modus file
 * in the BSP:
 * * C Data File:
 *   /GeneratedSource/cycfg_connectivity_bt.c
 * * C Header File:
 *   /GeneratedSource/cycfg_connectivity_bt.h
 *
 *******************************************************************************
 * \subsection group_lpa_p3_cc_parameters Configuration parameters
 *******************************************************************************
 *
 * The following parameters and their mapping to macros are available:
 *
 * <table class="doxtable">
 *   <tr><th>Category</th>
 *       <th>Parameter</th>
 *       <th>Description</th>
 *       <th>Parameter values</th></tr>
 *   <tr><td>Host Wake Configuration</td>
 *       <td>Host-Wake-Up Signal</td>
 *       <td>Select the host pin which is connected to BT device's
 *           BT_HOST_WAKE signal.</td>
 *       <td>
 *           * Empty (default)
 *           * Any enabled pin from Pin tab</td></tr>
 *   <tr><td>Host Wake Configuration</td>
 *       <td>Host-Wake-IRQ-Event</td>
 *       <td>Select the Trigger for Host wake IRQ event.</td>
 *       <td>
 *           * Falling Edge (Active Low) (default)
 *           * Rising Edge  (Active High)</td></tr>
 *   <tr><td>BT Device Wake Configuration</td>
 *       <td>Device-Wake-Up Signal</td>
 *       <td>Select the host pin which is connected to BT device's
 *           BT_DEV_WAKE signal.</td>
 *       <td>
 *           * Empty (default)
 *           * Any enabled pin from Pin tab</td></tr>
 *   <tr><td>BT Device Wake Configuration</td>
 *       <td>Device-Wake-Up Polarity</td>
 *       <td>Select the Polarity condition for BT_DEV_WAKE signal.</td>
 *       <td>
 *           * Active Low (default)
 *           * Active High</td></tr>
 * </table>
 *
 *******************************************************************************
 * \section section_lpa_measurement How to measure power consumption
 *******************************************************************************
 *
 * For jumper connections and how to measure power consumption, refer to the guide of the respective kit. \n
 * <b>For Example:</b> 
 * To measure the power consumption for the CY8CKIT-062S2-43012 Kit, refer to section <i>3.2.5 Power Supply System</i> in the kit guide
 * <a href="https://www.infineon.com/dgdl/Infineon-CY8CKIT-062S2-43012_PSoC_62S2_Wi-Fi_BT_Pioneer_Kit_Guide-UserManual-v01_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0f01c8f11927">
 * <b>User manual</b></a>.
 *
 *
 *******************************************************************************
 * \section section_lpa_hostwake Wi-Fi host wake configuration
 *******************************************************************************
 *
 * This section explains how to enable the Wi-Fi host wake configuration using the Device Configurator tool. \n
 * \n
 * The following table lists the Device name and its corresponding Wi-Fi wost wake pin.
 * <table class="doxtable">
 *   <tr>
 *     <th>Infineon platform</th>
 *     <th>Wi-Fi Host wake pin</th>
 *   </tr>
 *   <tr>
 *     <td align="center">CY8CKIT_062_WIFI_BT</td>
 *     <td align="center">P2[7]</td>
 *   </tr>
 *   <tr>
 *     <td align="center">CY8CKIT_062S2_43012</td>
 *     <td align="center">P4[1]</td>
 *   </tr>
 *   <tr>
 *     <td align="center">CY8CPROTO_062_4343W</td>
 *     <td align="center">P0[4]</td>
 *   </tr>
 *   <tr>
 *     <td align="center">CY8CEVAL_062S2_LAI_4373M2</td>
 *     <td align="center">P4[1]</td>
 *   </tr>
 *   <tr>
 *     <td align="center">CY8CEVAL_062S2_MUR_43439M2</td>
 *     <td align="center">P4[1]</td>
 *   </tr>
 * </table>
 * \n
 * The following is an example of the CY8CKIT_062S2_43012 device to how to enable the Wi-Fi host wake configuration:
 *******************************************************************************
 * \subsection section_lpa_hostwake_CY8CKIT_062S2_43012 CY8CKIT_062S2_43012
 *******************************************************************************
 *
 * * On the PSoC&trade; 6 MCU Pins tab of the Device Configurator tool , Enable the Host wake pin P4[1] and name it CYBSP_WIFI_HOST_WAKE.
 * * In the Parameters pane, change the following:
 *    * Drive Mode: Analog High-Z. Input buffer off.
 *    * Initial Drive State: High(1).
 *    * Interrupt Trigger Type: Rising Edge.
 * * Go to WiFi tab, set Host Device Interrupt Pin to CYBSP_WIFI_HOST_WAKE.
 *
 * \image html hostwake_config_43012_062S2_kit.png height=400px
 *
 *
 *******************************************************************************
 * \section section_lpa_bt_wakeconfig BT Wake Configuration
 *******************************************************************************
 *
 * This section explains how to enable the Bluetooth&reg; wake configuration using the device configurator tool. \n
 * \n
 * The following table lists the device name and its corresponding Bluetooth&reg; host wake pin and Bluetooth&reg; device wake pin:
 *  <table class="doxtable">
 *   <tr>
 *     <th>Infineon platform</th>
 *     <th>BT Host wake pin</th>
 *     <th>BT Device wake pin</th>
 *   </tr>
 *   <tr>
 *     <td align="center">CY8CKIT_062_WIFI_BT</td>
 *     <td align="center">P3[5]</td>
 *     <td align="center">P4[0]</td>
 *   </tr>
 *   <tr>
 *     <td align="center">CY8CKIT_062S2_43012</td>
 *     <td align="center">P3[5]</td>
 *     <td align="center">P4[0]</td>
 *   </tr>
 *   <tr>
 *     <td align="center">CY8CPROTO_062_4343W</td>
 *     <td align="center">P3[5]</td>
 *     <td align="center">P4[0]</td>
 *   </tr>
 *   <tr>
 *     <td align="center">CY8CEVAL_062S2_LAI_4373M2</td>
 *     <td align="center">P2[7]</td>
 *     <td align="center">P4[0]</td>
 *   </tr>
 *   <tr>
 *     <td align="center">CY8CEVAL_062S2_MUR_43439M2</td>
 *     <td align="center">P3[5]</td>
 *     <td align="center">P4[0]</td>
 *   </tr>
 * </table>
 * \n
 * The following table lists the Device name and its corresponding Bluetooth&reg; host wake pin and Bluetooth&reg; device wake pin:
 *******************************************************************************
 * \subsection section_lpa_bt_wakeconfig_CY8CKIT_062S2_43012 CY8CKIT_062S2_43012
 *******************************************************************************
 *
 * * On the PSoC&trade; 6 MCU Pins tab of the Device Configurator tool 
 *    * Enable the Bluetooth&reg; device wake pin P3[5] and name it CYBSP_BT_DEVICE_WAKE.
 *        * In the Parameters pane, change the following.
 *           * Drive Mode: Strong Drive. Input buffer off.
 *           * Initial Drive State: Low(0).
 *    * Enable the Bluetooth&reg; Host wake pin P4[0] and name it CYBSP_BT_HOST_WAKE.
 *        * In the Parameters pane, change the following.
 *           * Drive Mode: Analog High-Z. Input buffer off.
 *           * Initial Drive State: Low(0).
 * * Go to Bluetooth&reg; tab in CYW43012C0WKWBG and change the following.
 *    * Enable Bluetooth&reg; wake up configurations checkbox
 *    * Host Wake-Up Signal : CYBSP_BT_HOST_WAKE 
 *    * Host Wake-Up IRQ event:  Failing Edge
 *    * Device Wake-Up Signal : CYBSP_BT_DEVICE_WAKE
 *    * Device Wake-up Polarity : Active low
 *
 * \image html btwake_config_43012_062S2_kit.png height=400px
 *
 *
 *******************************************************************************
 * \section group_lpa_misra MISRA-C Compliance
 *******************************************************************************
 *
 * If you need a MISRA compliance report for this library,
 * create a ticket at <a href="https://www.infineon.com/cms/en/about-infineon/company/contacts/support/">
 * <b>Infineon support</b></a>
 *
 *******************************************************************************
 * \section group_lpa_errata Errata
 *******************************************************************************
 * This section lists the known problems with the LPA middleware.
 *
 *******************************************************************************
 * \section section_lpa_more_information More information
 *******************************************************************************
 *
 * For more information, Infineon highly
 * recommends starting with these documents.
 *
 * * <a href="https://github.com/Infineon/lpa">
 *   <b>Infineon GitHub LPA Middleware</b></a>
 *
 * * <a href="https://www.infineon.com/cms/en/design-support/tools/sdk/modustoolbox-software/?redirId=178597">
 *   <b>ModusToolbox&trade; Software Environment, Quick Start Guide, Documentation,
 *   and Videos</b></a>
 *
 * * <a href="https://github.com/Infineon/mtb-example-anycloud-wlan-lowpower">
 *   <b>LPA Middleware Code Example for FREERTOS </b></a>
 *
 * * <a href="https://github.com/Infineon">
 *   <b>LPA Middleware Code Examples at GITHUB</b></a>
 *
 * * <a href="https://www.infineon.com/dgdl/Infineon-ModusToolbox_Device_Configurator_4.0_User_Guide-UserManual-v01_00-EN.pdf?fileId=8ac78c8c8386267f0183a960bd41598f&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-files&redirId=180683&redirId=VL144">
 *   <b>ModusToolbox&trade; Device Configurator Tool Guide</b></a>
 *
 * * <a href="https://www.infineon.com/dgdl/Infineon-PSoC_6_MCU_PSoC_63_with_BLE_Architecture_Technical_Reference_Manual-AdditionalTechnicalInformation-v11_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0f946fea01ca&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-technical_reference_manual&redirId=TRM148">
 *   <b>PSoC&trade; 6 Technical Reference Manual</b></a>
 *
 * * <a href="https://www.infineon.com/dgdl/Infineon-PSoC_6_MCU_PSoC_63_with_BLE_Datasheet_Programmable_System-on-Chip_(PSoC)-DataSheet-v16_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0ee4efe46c37&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-datasheet&redirId=VL4079">
 *   <b>PSoC&trade; 63 with BLE Datasheet Programmable System-on-Chip datasheet</b></a>
 *
 * * <a href="https://www.infineon.com">
 *   <b>Infineon</b></a>
 *
 * <b>Note</b> 
 * Links to other software component's documentation
 * (middleware and PDL) point to Infineon GitHub and the latest available
 * version of the software. To get documentation for a specific version,
 * download it from GitHub and unzip the component archive.
 * The documentation is available in the docs folder.
 *
 *******************************************************************************
 * \section section_lpa_code_snippets Code Snippets
 *******************************************************************************
 *
 *
 * \subsection subsection_lpa_snippet_1 Code Snippet 1: Using wait_net_suspend for suspend/resume network stack
 * The following code snippet demonstrates an example for Suspending Network stack to allow MCU to go to Deep Sleep.
 * LPA API \b cylpa_on_emac_activity should be called to resume network stack in order to queue packets to LwIP.
 * \snippet wifi_low_power.c snippet_cylpa_wait_net_suspend
 *
 * \subsection subsection_lpa_snippet_2 Code Snippet 2: Read OLM configuration from device configurator.
 * The following code snippet demonstrates an example for reading OLM configuration from device configurator and makes a
 * local copy of the list.
 * \snippet wifi_low_power.c snippet2_cylpa_read_olm
 *
 * \subsection subsection_lpa_snippet_3 Code Snippet 3: Search for TCP KA offload in the device configurator list.
 * The following code snippet demonstrates an example for searching the "TKO" string in the device configurator list
 * and if not present it add manual TCP KA offload configuration.
 * \snippet wifi_low_power.c snippet3_cylpa_add_tko_olm
 *
 * \subsection subsection_lpa_snippet_4 Code Snippet 4: Use cylpa_restart_olm when the OLM configuration is changed by the user application.
 * The following code snippet demonstrates an example for Restarting OLM interface with user defined configuration.
 * \snippet wifi_low_power.c snippet4_cylpa_restart_olm
 *
 * \subsection subsection_lpa_snippet_5 Code Snippet 5: Use cy_tcp_create_socket_connection to offload TCP Keep-alive to WiFi Firmware
 * The following code snippet demonstrates an example for creating TCP connection(s) and offloads TCP keep-alive when host MCU enters sleep/deepsleep via
 * wait_net_suspend API call.
 * \snippet wifi_low_power.c snippet5_cy_tcp_create_socket_connection
 *
 * \subsection subsection_lpa_snippet_6 Code Snippet 6: Read OLM configuration from device configurator.
 *  The following code snippet demonstrates an example for reading OLM configuration from device configurator and makes a
 * local copy of the list.
 * \snippet wifi_low_power.c snippet6_cylpa_read_olm
 *
 *******************************************************************************
 * @section apis LPA API
 *******************************************************************************
 *
 * LPA utility API's used by LPA middleware. LPA API's are documented below.
 * @htmlonly
 * <ul>
 * <li><a href="group__lpautilities.html">LPA Utility API</a></li>
 * </ul>
 * <br>
 * @endhtmlonly
 **/


#ifndef OL_COMMON_H__
#define OL_COMMON_H__   (1)

#ifdef __cplusplus
extern "C" {
#endif

/* TODO: change whd_t define to be the correct whd pointer type */
typedef void whd_t;

/*******************************************************************************
* LPA Data Structures
*******************************************************************************/

/******************************************************************************/
/** \addtogroup group_lpa_structures *//** \{ */
/******************************************************************************/

/** Offload information
 *
 * Context pointers that an offload may use to accomplish its function. */
typedef struct ol_info
{
    whd_t   *whd;       /**< Wireless Host Driver pointer. */
    void    *ip;        /**< IP network stack pointer. */
    void    *worker;    /**< Worker Thread info pointer */
} ol_info_t;

/** \} */

#ifdef __cplusplus
}
#endif

#endif /* !OL_COMMON_H__ */

