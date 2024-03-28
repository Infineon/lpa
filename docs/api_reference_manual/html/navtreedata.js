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
  [ "Low Power Assistant Middleware Library 5.2.0", "index.html", [
    [ "Overview", "index.html#section_lpa_overview", [
      [ "Features", "index.html#section_lpa_features", null ]
    ] ],
    [ "Getting started", "index.html#section_lpa_getting_started", null ],
    [ "Prerequisites", "index.html#section_lpa_Prerequisites", null ],
    [ "Definitions", "index.html#group_lpa_definitions", null ],
    [ "ModusToolbox™ Device Configurator flow", "index.html#group_device_configurator_flow", null ],
    [ "Part 1. MCU Low Power", "index.html#group_lpa_p1", [
      [ "Quick start guide", "index.html#group_lpa_p1_mcu_qsg", null ],
      [ "MCU Low Power Configuration Considerations", "index.html#group_lpa_p1_cc", [
        [ "Configuration Parameters", "index.html#group_lpa_p1_cc_parameters", null ]
      ] ]
    ] ],
    [ "Part 2. Wi-Fi low power", "index.html#group_lpa_p2", [
      [ "Wi-Fi host-wake signal", "index.html#group_lpa_p2_host_wake", [
        [ "Quick start guide", "index.html#group_lpa_p2_host_wake_qsg", null ]
      ] ],
      [ "Wi-Fi ARP Offload", "index.html#group_lpa_p2_arp_offload", [
        [ "Awake vs. Sleeping", "index.html#group_lpa_p2_awake_sleeping", null ],
        [ "Host Auto Reply", "index.html#group_lpa_p2_hostautoreply", null ],
        [ "Peer Auto Reply", "index.html#group_lpa_p2_peerautoreply", null ],
        [ "Host and Peer Auto Reply", "index.html#group_lpa_p2_hostpeerautoreply", null ],
        [ "Host IP Snoop", "index.html#group_lpa_p2_hostsnoop", null ],
        [ "Quick start guide", "index.html#group_lpa_p2_arp_offload_qsg", null ]
      ] ],
      [ "Wi-Fi Packet filter offload", "index.html#group_lpa_p2_packet_filter", [
        [ "Filter types", "index.html#group_lpa_p2_filtertypes", null ],
        [ "Action (keep vs toss)", "index.html#group_lpa_p2_keeptoss", null ],
        [ "When Active (Sleep vs Wake)", "index.html#group_lpa_p2_sleepwake", null ],
        [ "Quick start guide", "index.html#group_lpa_p2_packet_filter_qsg", null ]
      ] ],
      [ "Wi-Fi TCP keepalive offload", "index.html#group_lpa_p2_tcp_keepalive", [
        [ "Quick start guide", "index.html#group_lpa_p2_tcp_keepalive_qsg", null ]
      ] ],
      [ "DHCP Lease Time Renew offload", "index.html#group_lpa_p2_dltro", [
        [ "Quick start guide", "index.html#group_lpa_p2_dltro_qsg", null ]
      ] ],
      [ "ICMP offload", "index.html#group_lpa_p2_icmp", [
        [ "Quick start guide", "index.html#group_lpa_p2_icmp_qsg", null ]
      ] ],
      [ "Neighbor Discovery offload", "index.html#group_lpa_p2_ndoe", [
        [ "Quick start guide", "index.html#group_lpa_p2_ndoe_qsg", null ]
      ] ],
      [ "NULL Keepalive offload", "index.html#group_lpa_p2_null_keepalive", [
        [ "Quick start guide", "index.html#group_lpa_p2_null_keepalive_qsg", null ]
      ] ],
      [ "NAT Keepalive offload", "index.html#group_lpa_p2_nat_keepalive", [
        [ "Quick start guide", "index.html#group_lpa_p2_nat_keepalive_qsg", null ]
      ] ],
      [ "Wake on WirelessLAN", "index.html#group_lpa_p2_wowlpf", [
        [ "Quick start guide", "index.html#group_lpa_p2_wowlpf_qsg", null ]
      ] ],
      [ "MQTT keepalive offload", "index.html#group_lpa_p2_mqtt_keepalive", [
        [ "Quick start guide", "index.html#group_lpa_p2_mqtt_keepalive_qsg", null ]
      ] ],
      [ "Wi-Fi low power configuration considerations", "index.html#group_lpa_p2_cc", [
        [ "Host Wake Configuration Notes", "index.html#group_lpa_p2_cc_host_wake", null ],
        [ "Limitations", "index.html#group_lpa_p2_cc_limitations", null ],
        [ "IPv6 Packet Filter for MCU Deep Sleep", "index.html#group_lpa_p2_ipv6_filter", null ],
        [ "WLAN low power configuration", "index.html#group_lpa_p2_wlan_lowpower", null ],
        [ "Configuration parameters", "index.html#group_lpa_p2_cc_parameters", null ]
      ] ]
    ] ],
    [ "Part 3. Bluetooth® Low Power", "index.html#group_lpa_p3", [
      [ "BT Low Power Configuration considerations", "index.html#group_lpa_p3_cc", null ],
      [ "Configuration parameters", "index.html#group_lpa_p3_cc_parameters", null ]
    ] ],
    [ "How to measure power consumption", "index.html#section_lpa_measurement", null ],
    [ "Wi-Fi host wake configuration", "index.html#section_lpa_hostwake", [
      [ "CY8CKIT_062S2_43012", "index.html#section_lpa_hostwake_CY8CKIT_062S2_43012", null ]
    ] ],
    [ "BT Wake Configuration", "index.html#section_lpa_bt_wakeconfig", [
      [ "CY8CKIT_062S2_43012", "index.html#section_lpa_bt_wakeconfig_CY8CKIT_062S2_43012", null ]
    ] ],
    [ "MISRA-C Compliance", "index.html#group_lpa_misra", null ],
    [ "Errata", "index.html#group_lpa_errata", null ],
    [ "More information", "index.html#section_lpa_more_information", null ],
    [ "Code Snippets", "index.html#section_lpa_code_snippets", [
      [ "Code Snippet 1: Using wait_net_suspend for suspend/resume network stack", "index.html#subsection_lpa_snippet_1", null ],
      [ "Code Snippet 2: Read OLM configuration from device configurator.", "index.html#subsection_lpa_snippet_2", null ],
      [ "Code Snippet 3: Search for TCP KA offload in the device configurator list.", "index.html#subsection_lpa_snippet_3", null ],
      [ "Code Snippet 5: Use cy_tcp_create_socket_connection to offload TCP Keep-alive to WiFi Firmware", "index.html#subsection_lpa_snippet_5", null ],
      [ "Code Snippet 6: Read OLM configuration from device configurator.", "index.html#subsection_lpa_snippet_6", null ],
      [ "Code Snippet 7: MQTT offload configuration.", "index.html#subsection_lpa_snippet_7", null ]
    ] ],
    [ "LPA API", "index.html#apis", null ]
  ] ]
];

var NAVTREEINDEX =
[
"index.html"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';