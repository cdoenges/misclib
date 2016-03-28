/** Fast ringbuffer implementation.


    @file ringbuffer.c
    @ingroup misclib

    @author Christian D&ouml;nges <cd@platypus-projects.de>

    @note The master repository for this file is at
     <a href="https://github.com/cdoenges/misclib">https://github.com/cdoenges/misclib</a>


    LICENSE

    This software is open source software under the "Simplified BSD License"
    as approved by the Open Source Initiative (OSI)
    <http://opensource.org/licenses/bsd-license.php>:


    Copyright (c) 2011-2015, Christian Doenges (Christian D&ouml;nges) All rights
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


    This module was inspired by Jack Ganssle's Embedded Muse #110
    http://www.ganssle.com/tem/tem110.pdf. Thank you Jack, Phil, and Keil!

 */

#include <assert.h>
#include <stdlib.h>


#include "ringbuffer.h"


/** True if the value is a power of two. */
#define isPowerOfTwo(value) ((TEMP__ = (value)) && !((TEMP__ -1) & TEMP__))



void ringbuffer_init(ringbuffer_t *pRB) {
    int TEMP__ = 0;

    assert(NULL != pRB->pBuffer);
    assert(isPowerOfTwo(pRB->size));

    pRB->headOffset = 0;
    pRB->tailOffset = 0;
} // ringbuffer_init()



ringbuffer_status_t ringbuffer_put(ringbuffer_t *pRB, unsigned char newEntry) {

    if (ringbuffer_length(pRB) >= pRB->size) {
        return ring_full;
    }

    pRB->pBuffer[pRB->headOffset & (pRB->size -1)] = newEntry;
    pRB->headOffset ++;

    return ring_ok;
} // ringbuffer_put()



int ringbuffer_get(ringbuffer_t *pRB) {
    unsigned char value;

    if (ringbuffer_length(pRB) == 0) {
        return ring_empty;
    }

    value = pRB->pBuffer[pRB->tailOffset & (pRB->size - 1)];
    pRB->tailOffset ++;

    return value;
} // ringbuffer_get()



int ringbuffer_peek(ringbuffer_t *pRB) {
    unsigned char value;

    if (ringbuffer_length(pRB) == 0) {
        return ring_empty;
    }

    value = pRB->pBuffer[pRB->tailOffset & (pRB->size - 1)];

    return value;
} // ringbuffer_get()
