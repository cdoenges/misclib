/** Unit tests for the ringbuffer module.

   @file unittest_ringbuffer.c
   @ingroup misclib

   @author Christian D&ouml;nges <cd@platypus-projects.de>

   @note The master repository for this file is at
    <a href="https://github.com/cdoenges/misclib">https://github.com/cdoenges/misclib</a>

    LICENSE

    This software is open source software under the "Simplified BSD License"
    as approved by the Open Source Initiative (OSI)
    <http://opensource.org/licenses/bsd-license.php>:


    Copyright (c) 2016, Christian Doenges (Christian D&ouml;nges) All rights
    reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

    * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

    * Neither the name of the Platypus Projects GmbH nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
    TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
    PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "ringbuffer.h"
#include "misclibTest.h"



bool unittest_ringbuffer(void) {
    uint8_t byteBuffer[128];
    unsigned length;
    ringbuffer_t rb = {
        byteBuffer,
        sizeof(byteBuffer),
        0,
        0
    };


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
