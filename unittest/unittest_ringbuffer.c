/** Unit tests for the ringbuffer module.

   @file unittest_ringbuffer.c
   @ingroup misclib

   @author Christian D&ouml;nges <cd@platypus-projects.de>

   @note The master repository for this file is at
    <a href="https://github.com/cdoenges/misclib">https://github.com/cdoenges/misclib</a>

    LICENSE

    Copyright 2016, 2017 Christian Doenges (Christian D&ouml;nges)

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "logging.h"
#include "misclibTest.h"
#include "ringbuffer.h"



bool unittest_ringbuffer(void) {
    uint8_t byteBuffer[128];
    unsigned length;
    ringbuffer_t rb = {
        byteBuffer,
        sizeof(byteBuffer),
        0,
        0
    };


    log_logMessage(LOGLEVEL_INFO, "Testing ringbuffer");

    // Initialize the buffer.
    ringbuffer_init(&rb);

    // Write bytes to the buffer, then read them.
    for (length = 0;length <= sizeof(byteBuffer) + 1;length ++) {
        ringbuffer_status_t rs;
        unsigned rl, wl;


        // Write 0..length bytes to the ringbuffer.
        for (wl = 0;wl < length;wl ++) {
            rs = ringbuffer_put(&rb, (length + wl) & 0xff);
            if (wl < sizeof(byteBuffer)) {
                expectTrue(ring_ok == rs);
                expectTrue(ringbuffer_length(&rb) == (wl + 1));
            } else {
                expectTrue(ring_full == rs);
                expectTrue(ringbuffer_length(&rb) == sizeof(byteBuffer));
            }
        } // for wl

        if (wl > sizeof(byteBuffer)) {
            wl = sizeof(byteBuffer);
        }

        // Read 1..length+1 bytes from the ringbuffer.
        for (rl = 0;rl <= length;rl ++) {
            int c = ringbuffer_get(&rb);

            if (rl < wl) {
                expectTrue(c >= 0);
                expectTrue(((length + rl) & 0xff) == c);
                expectTrue(ringbuffer_length(&rb) == (wl - rl - 1));
            } else {
                expectTrue(c < 0);
                expectTrue(ringbuffer_length(&rb) == 0);
            }

            c = ringbuffer_peek(&rb);
            if (rl + 1 < wl) {
                expectTrue(c >= 0);
                expectTrue(((length + rl + 1) & 0xff) == c);
            } else {
                expectTrue(c < 0);
            }
        } // for rl
    } // for length

    return true;
} // unittest_ringbuffer()
