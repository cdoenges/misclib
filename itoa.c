/** Converts an integer to an ASCII string with the given radix.


    @file itoa.c
    @ingroup misclib

    @author Christian D&ouml;nges <cd@platypus-projects.de>

    @note The master repository for this file is at <a href="https://github.com/cdoenges/misclib">https://github.com/cdoenges/misclib</a>

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
#include <errno.h>

#include "itoa.h"


char *intoa(int value, char *pDst, size_t buflen, int base) {
    char const *digitToAscii = "0123456789abcdef";
    char *ptr = pDst;
    char *pR;
    int tmp;

    assert(NULL != pDst);
    assert(buflen > 0);

    // Check if base is valid.
    if ((base < 2) || (base > 16)) {
        errno = EINVAL;
        return NULL;
    }


    do {
        int digit;

        if (0 == buflen--) {
            return NULL;
        }
        tmp = value;
        value /= base;
        digit = tmp - value * base;
        *ptr++ = digitToAscii[digit];
    } while (0 != value);

    // Handle negative numbers.
    if (tmp < 0) {
        if (0 == buflen--) {
            return NULL;
        }
        *ptr++ = '-';
    } // if negative
    if (0 == buflen--) {
        return NULL;
    }

    *ptr++ = '\0';  // Terminate string.

    // Reverse the string.
    pR = pDst;
    while (pR < ptr) {
        char currentchar = *ptr;

        *ptr-- = *pR;
        *pR++ = currentchar;
    }

    return pDst;
} // intoa()



char *itoa(int value, char *pDst, int base) {
    char const *digitToAscii = "0123456789abcdef";
    char *ptr = pDst;
    char *pR;
    int tmp;

    assert(NULL != pDst);

    // Check if base is valid.
    if ((base < 2) || (base > 16)) {
        errno = EINVAL;
        return NULL;
    }


    do {
        int digit;

        tmp = value;
        value /= base;
        digit = tmp - value * base;
        *ptr++ = digitToAscii[digit];
    } while (0 != value);

    // Handle negative numbers.
    if (tmp < 0) {
        *ptr++ = '-';
    }
    *ptr-- = '\0';  // Terminate string.

    // Reverse the string.
    pR = pDst;
    while (pR < ptr) {
        char currentchar = *ptr;

        *ptr-- = *pR;
        *pR++ = currentchar;
    }

    return pDst;
} // itoa()

