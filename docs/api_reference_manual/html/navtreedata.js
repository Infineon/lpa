/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "Low Power Assistant Middleware Library 5.8.0", "index.html", [
    [ "Low Power Assistant (LPA) middleware library - Wi-Fi, Bluetooth®, and MCU low-power configuration for Infineon devices", "index.html", "index" ],
    [ "Low Power Assistant (LPA) Datasheet", "group_lpa_datasheet.html", [
      [ "Overview", "group_lpa_datasheet.html#section_lpa_overview", [
        [ "Features", "group_lpa_datasheet.html#section_lpa_features", null ]
      ] ],
      [ "Getting started", "group_lpa_datasheet.html#section_lpa_getting_started", null ],
      [ "Prerequisites", "group_lpa_datasheet.html#section_lpa_Prerequisites", null ],
      [ "Definitions", "group_lpa_datasheet.html#group_lpa_definitions", null ],
      [ "ModusToolbox™ Device Configurator flow", "group_lpa_datasheet.html#group_device_configurator_flow", null ],
      [ "Part 1. MCU Low Power", "group_lpa_datasheet.html#group_lpa_p1", [
        [ "Quick start guide", "group_lpa_datasheet.html#group_lpa_p1_mcu_qsg", null ],
        [ "MCU Low Power Configuration Considerations", "group_lpa_datasheet.html#group_lpa_p1_cc", [
          [ "Configuration Parameters", "group_lpa_datasheet.html#group_lpa_p1_cc_parameters", null ]
        ] ]
      ] ],
      [ "Part 2. Wi-Fi low power", "group_lpa_datasheet.html#group_lpa_p2", [
        [ "Wi-Fi host-wake signal", "group_lpa_datasheet.html#group_lpa_p2_host_wake", [
          [ "Quick start guide", "group_lpa_datasheet.html#group_lpa_p2_host_wake_qsg", null ]
        ] ],
        [ "Wi-Fi ARP Offload", "group_lpa_datasheet.html#group_lpa_p2_arp_offload", [
          [ "Awake vs. Sleeping", "group_lpa_datasheet.html#group_lpa_p2_awake_sleeping", null ],
          [ "Host Auto Reply", "group_lpa_datasheet.html#group_lpa_p2_hostautoreply", null ],
          [ "Peer Auto Reply", "group_lpa_datasheet.html#group_lpa_p2_peerautoreply", null ],
          [ "Host and Peer Auto Reply", "group_lpa_datasheet.html#group_lpa_p2_hostpeerautoreply", null ],
          [ "Host IP Snoop", "group_lpa_datasheet.html#group_lpa_p2_hostsnoop", null ],
          [ "Quick start guide", "group_lpa_datasheet.html#group_lpa_p2_arp_offload_qsg", null ]
        ] ],
        [ "Wi-Fi Packet filter offload", "group_lpa_datasheet.html#group_lpa_p2_packet_filter", [
          [ "Filter types", "group_lpa_datasheet.html#group_lpa_p2_filtertypes", null ],
          [ "Action (keep vs toss)", "group_lpa_datasheet.html#group_lpa_p2_keeptoss", null ],
          [ "When Active (Sleep vs Wake)", "group_lpa_datasheet.html#group_lpa_p2_sleepwake", null ],
          [ "Quick start guide", "group_lpa_datasheet.html#group_lpa_p2_packet_filter_qsg", null ]
        ] ],
        [ "Wi-Fi TCP keepalive offload", "group_lpa_datasheet.html#group_lpa_p2_tcp_keepalive", [
          [ "Quick start guide", "group_lpa_datasheet.html#group_lpa_p2_tcp_keepalive_qsg", null ]
        ] ],
        [ "DHCP Lease Time Renew offload", "group_lpa_datasheet.html#group_lpa_p2_dltro", [
          [ "Quick start guide", "group_lpa_datasheet.html#group_lpa_p2_dltro_qsg", null ]
        ] ],
        [ "ICMP offload", "group_lpa_datasheet.html#group_lpa_p2_icmp", [
          [ "Quick start guide", "group_lpa_datasheet.html#group_lpa_p2_icmp_qsg", null ]
        ] ],
        [ "Neighbor Discovery offload", "group_lpa_datasheet.html#group_lpa_p2_ndoe", [
          [ "Quick start guide", "group_lpa_datasheet.html#group_lpa_p2_ndoe_qsg", null ]
        ] ],
        [ "NULL Keepalive offload", "group_lpa_datasheet.html#group_lpa_p2_null_keepalive", [
          [ "Quick start guide", "group_lpa_datasheet.html#group_lpa_p2_null_keepalive_qsg", null ]
        ] ],
        [ "NAT Keepalive offload", "group_lpa_datasheet.html#group_lpa_p2_nat_keepalive", [
          [ "Quick start guide", "group_lpa_datasheet.html#group_lpa_p2_nat_keepalive_qsg", null ]
        ] ],
        [ "Wake on WirelessLAN", "group_lpa_datasheet.html#group_lpa_p2_wowlpf", [
          [ "Quick start guide", "group_lpa_datasheet.html#group_lpa_p2_wowlpf_qsg", null ]
        ] ],
        [ "MQTT keepalive offload", "group_lpa_datasheet.html#group_lpa_p2_mqtt_keepalive", [
          [ "Quick start guide", "group_lpa_datasheet.html#group_lpa_p2_mqtt_keepalive_qsg", null ]
        ] ],
        [ "Wi-Fi low power configuration considerations", "group_lpa_datasheet.html#group_lpa_p2_cc", [
          [ "Wi-Fi host wake configuration", "group_lpa_datasheet.html#group_lpa_p2_cc_hostwake", null ],
          [ "CY8CKIT_062S2_43012", "group_lpa_datasheet.html#group_lpa_p2_cc_CY8CKIT_062S2_43012", null ],
          [ "Limitations", "group_lpa_datasheet.html#group_lpa_p2_cc_limitations", null ],
          [ "IPv6 Packet Filter for MCU Deep Sleep", "group_lpa_datasheet.html#group_lpa_p2_ipv6_filter", null ],
          [ "WLAN low power configuration", "group_lpa_datasheet.html#group_lpa_p2_wlan_lowpower", null ],
          [ "Configuration parameters", "group_lpa_datasheet.html#group_lpa_p2_cc_parameters", null ]
        ] ]
      ] ],
      [ "Part 3. Bluetooth® Low Power", "group_lpa_datasheet.html#group_lpa_p3", [
        [ "BT Low Power Configuration considerations", "group_lpa_datasheet.html#group_lpa_p3_cc", [
          [ "BT Wake Configuration", "group_lpa_datasheet.html#group_lpa_p3_cc_btwakeconfig", null ],
          [ "CY8CKIT_062S2_43012", "group_lpa_datasheet.html#group_lpa_p3_cc_btwakeconfig_CY8CKIT_062S2_43012", null ],
          [ "Configuration parameters", "group_lpa_datasheet.html#group_lpa_p3_cc_parameters", null ]
        ] ]
      ] ],
      [ "How to measure power consumption", "group_lpa_datasheet.html#section_lpa_measurement", null ],
      [ "MISRA-C Compliance", "group_lpa_datasheet.html#group_lpa_misra", null ],
      [ "Errata", "group_lpa_datasheet.html#group_lpa_errata", null ],
      [ "More information", "group_lpa_datasheet.html#section_lpa_more_information", null ],
      [ "Code Snippets", "group_lpa_datasheet.html#section_lpa_code_snippets", [
        [ "Code Snippet 1: Using wait_net_suspend for suspend/resume network stack", "group_lpa_datasheet.html#subsection_lpa_snippet_1", null ],
        [ "Code Snippet 2: Read OLM configuration from device configurator.", "group_lpa_datasheet.html#subsection_lpa_snippet_2", null ],
        [ "Code Snippet 3: Search for TCP KA offload in the device configurator list.", "group_lpa_datasheet.html#subsection_lpa_snippet_3", null ],
        [ "Code Snippet 5: Use cy_tcp_create_socket_connection to offload TCP Keep-alive to WiFi Firmware", "group_lpa_datasheet.html#subsection_lpa_snippet_5", null ],
        [ "Code Snippet 6: Read OLM configuration from device configurator.", "group_lpa_datasheet.html#subsection_lpa_snippet_6", null ],
        [ "Code Snippet 7: MQTT offload configuration.", "group_lpa_datasheet.html#subsection_lpa_snippet_7", null ]
      ] ],
      [ "LPA API", "group_lpa_datasheet.html#apis", null ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"group_lpa_datasheet.html"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';