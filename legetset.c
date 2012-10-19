/** A set of utility functions to get and set 16 and 32 bit values at arbitrary
   locations in memory in little endian format, regardless of alignment
   constraints.

   Example:
   <pre>
   Value        Memory (low -> high address)
   0x1234       34 12
   0x56789abc   bc 9a 78 56
   </pre>

   @file legetset.c
   @defgroup misclib

   @author Christian D&ouml;nges <cd@platypus-projects.de>


    LICENSE

    This software is open source software under the "Simplified BSD License"
    as approved by the Open Source Initiative (OSI)
    <http://opensource.org/licenses/bsd-license.php>:


    Copyright (c) 2011, Christian Doenges (Christian D&ouml;nges) All rights
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

#include <assert.h>

#include "legetset.h"


void leSetUint16(void *pDest, uint_fast16_t value) {
    unsigned char *pD = (unsigned char *) pDest;

    assert(NULL != pD);

    *(pD + 1) = (unsigned char) (value >>  8);
    *pD       = (unsigned char) (value);
} // leSetUint16()



uint_fast16_t leGetUint16(void const *pSrc) {
    unsigned char const *pS = (unsigned char const *) pSrc;
    uint_fast16_t u;

    assert(NULL != pS);

    u = (((uint_fast16_t) *(pS + 1)) << 8) + *pS);

    return u;
} // leGetUint16()



void leSetUint32(void *pDest, uint_fast32_t value) {
    unsigned char *pD = (unsigned char *) pDest;

    assert(NULL != pD);

    *(pD + 3) = (unsigned char) (value >> 24);
    *(pD + 2) = (unsigned char) (value >> 16);
    *(pD + 1) = (unsigned char) (value >>  8);
    *pD       = (unsigned char) (value);
} // leSetUint32()



uint_fast32_t leGetUint32(void const *pSrc) {
    unsigned char const *pS = (unsigned char const *) pSrc;
    uint_fast32_t u;

    assert(NULL != pS);

    u =   (((uint_fast32_t) *(pS + 3)) << 24) 
        + (((uint_fast32_t) *(pS + 2)) << 16)
        + (((uint_fast32_t) *(pS + 1)) <<  8)
        +                   *pS;

    return u;
} // leGetUint32()
