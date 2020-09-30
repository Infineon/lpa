/*******************************************************************************
* File Name: ip4string.c
*
* Version: 1.0
*
* Description: This file implements the functions needed to convert IPv4 address to string
* and vice versa
*
********************************************************************************
* Copyright 2020 Cypress Semiconductor Corporation
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
********************************************************************************/
#include <stdio.h>
#include <string.h>
#include "ip4string.h"

static void ipv4_itoa(char *string, uint8_t byte);

/**
 * Print binary IPv4 address to a string.
 * String must contain enough room for full address, 16 bytes exact.
 * \param addr IPv4 address.
 * \p buffer to write string to.
 */
uint_fast8_t ip4tos(const void *ip4addr, char *p)
{
    uint_fast8_t outputPos = 0;
    const uint8_t *byteArray = ip4addr;

    for (uint_fast8_t component = 0; component < 4; ++component) {
        //Convert the byte to string
        ipv4_itoa(&p[outputPos], byteArray[component]);

        //Move outputPos to the end of the string
        while (p[outputPos] != '\0') {
            outputPos += 1;
        }

        //Append a dot if this is not the last digit
        if (component < 3) {
            p[outputPos++] = '.';
        }
    }

    // Return length of generated string, excluding the terminating null character
    return outputPos;
}

static void ipv4_itoa(char *string, uint8_t byte)
{
    char *baseString = string;

    //Write the digits to the buffer from the least significant to the most
    //  This is the incorrect order but we will swap later
    do {
        *string++ = '0' + byte % 10;
        byte /= 10;
    } while (byte);

    //We put the final \0, then go back one step on the last digit for the swap
    *string-- = '\0';

    //We now swap the digits
    while (baseString < string) {
        uint8_t tmp = *string;
        *string-- = *baseString;
        *baseString++ = tmp;
    }
}

/**
 * Convert numeric IPv4 address string to a binary.
 * \param ip4addr IPv4 address in string format.
 * \param len Length of IPv4 string, maximum of 16..
 * \param dest buffer for address. MUST be 4 bytes.
 * \return boolean set to true if conversion succeded, false if it didn't
 */
bool stoip4(const char *ip4addr, size_t len, void *dest)
{
    uint8_t *addr = dest;

    if (len > 16) { // Too long, not possible
        return false;
    }

    uint_fast8_t stringLength = 0, byteCount = 0;

    //Iterate over each component of the IP. The exit condition is in the middle of the loop
    while (true) {

        //No valid character (IPv4 addresses don't have implicit 0, that is x.y..z being read as x.y.0.z)
        if (stringLength == len || ip4addr[stringLength] < '0' || ip4addr[stringLength] > '9') {
            return false;
        }

        //For each component, we convert it to the raw value
        uint_fast16_t byte = 0;
        while (stringLength < len && ip4addr[stringLength] >= '0' && ip4addr[stringLength] <= '9') {
            byte *= 10;
            byte += ip4addr[stringLength++] - '0';

            //We go over the maximum value for an IPv4 component
            if (byte > 0xff) {
                return false;
            }
        }

        //Append the component
        addr[byteCount++] = (uint8_t) byte;

        //If we're at the end, we leave the loop. It's the only way to reach the `true` output
        if (byteCount == 4) {
            break;
        }

        //If the next character is invalid, we return false
        if (stringLength == len || ip4addr[stringLength++] != '.') {
            return false;
        }
    }

    return stringLength == len || ip4addr[stringLength] == '\0';
}