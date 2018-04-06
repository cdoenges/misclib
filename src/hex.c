/** Functions to perform conversions from integer to hexadecimal and vice-versa.


    @file hex.c
    @ingroup misclib

    @author Christian D&ouml;nges <cd@platypus-projects.de>

    @note The master repository for this file is at
     <a href="https://github.com/cdoenges/misclib">https://github.com/cdoenges/misclib</a>

    LICENSE

    This software is open source software under the "Simplified BSD License"
    as approved by the Open Source Initiative (OSI)
    <http://opensource.org/licenses/bsd-license.php>:


    Copyright (c) 2010-2015, 2018, Christian Doenges (Christian D&ouml;nges)
    All rights reserved.

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
#include <ctype.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>

#include "hex.h"



void int4ToHex(char *pDest, int_fast8_t value) {
    assert(NULL != pDest);
    assert((value >= 0) && (value < 0x10));

    *pDest = nibbleToHexDigit(value);

} // nibbleToHex()



void int8ToHex(char *pDest, uint_fast8_t value) {
    assert(NULL != pDest);

    *pDest       = nibbleToHexDigit((value >> 4) & 0x0f);
    *(pDest + 1) = nibbleToHexDigit(value & 0x0f);
} // int8ToHex()



void int16ToHex(char *pDest, uint_fast16_t value) {
    int i;

    assert(NULL != pDest);

    for (i = 3;i >= 0;i --, value >>= 4) {
        *(pDest + i) = nibbleToHexDigit(value & 0x0f);
    } // for i
} // int16ToHex()



void int32ToHex(char *pDest, uint_fast32_t value) {
    int i;

    assert(NULL != pDest);

    for (i = 7;i >= 0;i --, value >>= 4) {
        *(pDest + i) = nibbleToHexDigit(value & 0x0f);
    } // for i
} // int32ToHex()



uint_fast8_t hexToInt8(char const *pSrc) {
    uint_fast8_t n;

    assert(NULL != pSrc);

    //lint -e{734} Loss of precision is not real because of the shift operation.
    n = ((uint_fast8_t) hexdigitToNibble(*pSrc) << 4)
        + (uint_fast8_t) hexdigitToNibble(*(pSrc + 1));
    return n;
} // hexToInt8()



uint_fast16_t hexToInt16(char const *pSrc) {
    uint_fast16_t n = 0;
    int i;

    assert(NULL != pSrc);

    for (i = 0;i < 4;i ++, pSrc++) {
        assert(isxdigit((int) *pSrc));
        n <<= 4;
        n += (uint_fast16_t) hexdigitToNibble(*pSrc);
    } // for i

    return n;
} // hexToInt16()



uint_fast32_t hexToInt32(char const *pSrc) {
    uint_fast32_t n = 0;
    int i;

    assert(NULL != pSrc);

    for (i = 0;i < 8;i ++, pSrc++) {
        assert(isxdigit((int) *pSrc));
        n <<= 4;
        n += (uint_fast16_t) hexdigitToNibble(*pSrc);
    } // for i

    return n;
} // hexToInt32()



size_t hexbuf2StringLength(size_t nrBytes,
                           bool useCRLF, bool showASCII, unsigned lineWidth,
                           bool showOffset) {
    size_t nrOfLines, bytesPerLine, totalBytes;

    assert(lineWidth > 0);

    if (0 == nrBytes) {
        return 0;
    }

    // Each line displays up to lineWidth bytes.
    nrOfLines = (nrBytes + lineWidth - 1) / lineWidth;  // ceil(nrBytes / lineWidth)

    // Displaying the line requires room for the (optional) offset,
    // the bytes, the (option) decoded ASCII, and the EOL terminator.
    bytesPerLine = (showOffset ? 8 : 0) + 3 * (size_t) lineWidth
        + (showASCII ? (size_t) lineWidth + 1 : 0)
        + (useCRLF ? 2 : 1);

    // The entire string is terminated with a NUL byte.
    totalBytes = bytesPerLine * nrOfLines + 1;

    return totalBytes;
} // hexbuf2StringLength()



#ifdef ENOMEM
char *hexbuf2String(char const *pValueBuffer, size_t nrBytes,
                    char *pStringBuffer, size_t stringBufferSize,
                    bool useCRLF, bool showASCII, unsigned lineWidth,
                    bool showOffset, size_t initialOffset) {
    size_t requiredStringBufferSize;
    size_t currentOffset = initialOffset;
    char *pOutputBuffer;


    assert(NULL != pValueBuffer);

    // This check is easy, perform it before anything else.
    if ((0 == nrBytes) || (0 == lineWidth)) {
        // Nothing to do.
        errno = 0;
        return NULL;
    }


    // Make sure the output buffer can be used.
    requiredStringBufferSize = hexbuf2StringLength(nrBytes,
        useCRLF, showASCII, lineWidth, showOffset);

    if (NULL == pStringBuffer) {
        // No output buffer was specified, so create it.
        if ((pStringBuffer = (char *) malloc(requiredStringBufferSize)) == NULL) {
            errno = ENOMEM;
            return NULL;
        }
    } else {
        // Check if the specified output buffer is large enough.
        if (stringBufferSize < requiredStringBufferSize) {
            errno = ERANGE;
            return NULL;
        }
    }


    // At this point, pStringBuffer provides a buffer large enough to
    // hold the entire dump, so start dumping.
    pOutputBuffer = pStringBuffer;
    while (nrBytes > 0) {
        unsigned offsetInLine;
        unsigned char const *pCurrentValue;
        unsigned paddingChars = 0;

        if (nrBytes < lineWidth) {
            // The last line is only partially filled.
            paddingChars = (unsigned) (lineWidth - nrBytes) * 3;
            lineWidth = (unsigned) nrBytes;
        }

        if (showOffset) {
            // Place the offset in the buffer.
            int32ToHex(pStringBuffer, currentOffset);
            pStringBuffer += 8;
        }

        // Dump as hex tuples.
        for (offsetInLine = 0, pCurrentValue = (unsigned char const *) pValueBuffer;
             offsetInLine < lineWidth;
             offsetInLine++, pCurrentValue ++) {
            *pStringBuffer++ = ' ';
            int8ToHex(pStringBuffer, *pCurrentValue);
            pStringBuffer += 2;
        } // for offsetInLine

        if (showASCII) {
            char c;

            // Dump ASCII representation.
            *pStringBuffer++ = ' ';
            while (paddingChars-- > 0) {
                // If this is the last line, pad the missing values to
                // achieve nice alignment of the ASCII representation.
                *pStringBuffer++ = ' ';
            }
            for (offsetInLine = 0, pCurrentValue = (unsigned char const *) pValueBuffer;
                 offsetInLine < lineWidth;
                 offsetInLine++, pCurrentValue ++) {
                c = isprint(*pCurrentValue) ? (char) *pCurrentValue : '.';
                *pStringBuffer++ = c;
            } // for offsetInLine
        } // if showASCII

        // Terminate the line
        *pStringBuffer++ = '\n';
        if (useCRLF) {
            *pStringBuffer++ = '\r';
        }

        // Continue with next line (if any).
        currentOffset += lineWidth;
        pValueBuffer += lineWidth;
        nrBytes -= lineWidth;
    } // while nrBytes

    // Terminate the string before returning.
    *pStringBuffer++ = '\0';
    return pOutputBuffer;
} // buf2String()
#endif // ENOMEM


#ifdef TEST
// To compile and run the test harness, use
//
// gcc -g -o hextest -D TEST -Wall hex.c && ./hextest
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]) {
    const char hex04lcArray[16] = { '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
    const char hex04ucArray[16] = { '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    char hex08Array[256*2];
    char hex16Array[4];
    char hex32Array[8];
    int i, j;
    char *p;
    size_t requiredStringBufferSize = 0;
    char *pStringBuffer;


    // Test nibbleToHexDigit()
    for (i = 0;i < 16;i ++) {
        if (nibbleToHexDigit(i) != hex04ucArray[i]) {
            printf("nibbleToHexDigit(%d) FAILED.\n", i);
            exit(1);
        }
    } // for i
    printf("nibbleToHexDigit() PASSED.\n");

    // Test nibbleToHexdigit()
    for (i = 0;i < 16;i ++) {
        if (nibbleToHexdigit(i) != hex04lcArray[i]) {
            printf("nibbleToHexdigit(%d) FAILED.\n", i);
            exit(1);
        }
    } // for i
    printf("nibbleToHexdigit() PASSED.\n");

    // Test hexdigitToNibble()
    for (i = 0;i < 16;i ++) {
        if (hexdigitToNibble(hex04ucArray[i]) != i) {
            printf("hexdigitToNibble(%c) FAILED.\n", hex04ucArray[i]);
            exit(1);
        }
    } // for i
    printf("hexdigitToNibble() PASSED.\n");


    // Test int4ToHex()
    for (i = 0;i < 16;i ++) {
        char c;

        int4ToHex(&c, i);
        if (hexdigitToNibble(c) != i) {
            printf("int4ToHex(i) FAILED.\n");
            exit(1);
        }
    } // for i


    // Test hexToInt8() using upper case
    p = hex08Array;
    for (i = 0;i < 16;i ++) {
        for (j = 0;j < 16;j ++) {
            *p++ = nibbleToHexDigit(i);
            *p++ = nibbleToHexDigit(j);
        } // for j
    } // for i
    for (i = 0;i < 256;i ++) {
        if (hexToInt8(&hex08Array[i * 2]) != i) {
            printf("hexToInt8(%c%c) FAILED.\n", hex08Array[i * 2], hex08Array[i * 2 + 1]);
            printf("Got %d, expected %d.\n", hexToInt8(&hex08Array[i * 2]), i);
            exit(1);
        }
    } // for i
    // Test hexToInt8() using lower case
    p = hex08Array;
    for (i = 0;i < 16;i ++) {
        for (j = 0;j < 16;j ++) {
            *p++ = nibbleToHexdigit(i);
            *p++ = nibbleToHexdigit(j);
        } // for j
    } // for i
    for (i = 0;i < 256;i ++) {
        if (hexToInt8(&hex08Array[i * 2]) != i) {
            printf("hexToInt8(%c%c) FAILED.\n", hex08Array[i * 2], hex08Array[i * 2 + 1]);
            printf("Got %d, expected %d.\n", hexToInt8(&hex08Array[i * 2]), i);
            exit(1);
        }
    } // for i
    printf("hexToInt8() PASSED.\n");


    // Test 16 bit values using upper case.
    for (i = 0;i < 65536;i ++) {
        int16ToHex(hex16Array, i);
        if ((hexdigitToNibble(hex16Array[0]) != ((i >> 12) & 0xf))
         || (hexdigitToNibble(hex16Array[1]) != ((i >>  8) & 0xf))
         || (hexdigitToNibble(hex16Array[2]) != ((i >>  4) & 0xf))
         || (hexdigitToNibble(hex16Array[3]) != ( i        & 0xf))) {
            printf("int16ToHex(%i) FAILED.\n", i);
            exit(1);
        }
        if (hexToInt16(hex16Array) != i) {
            printf("hexToInt16(%i) FAILED.\n", i);
            exit(1);
        }
    } // for i
    printf("int16ToHex() PASSED.\n");
    // Test some 16 bit values using lower case.
    if ((hexToInt16("abcd") != 0xabcd)
     || (hexToInt16("efE3") != 0xefe3)) {
        printf("hexToInt16(%i) FAILED.\n", i);
        exit(1);
    }
    printf("hexToInt16() PASSED.\n");

    // Test 32 bit quantities.
    if ((hexToInt32("0a1b3c4d") != 0x0a1b3c4d)
     || (hexToInt32("e5f6A7B8") != 0xe5f6a7b8)
     || (hexToInt32("9C9D8E7F") != 0x9c9d8e7f)) {
        printf("hexToInt32() FAILED.\n");
        exit(1);
    }
    printf("hexToInt32() PASSED.\n");

    int32ToHex(hex32Array, 0x0a1b3c4d);
    if (hexToInt32(hex32Array) != 0x0a1b3c4d) {
        printf("int32ToHex() FAILED.\n");
        exit(1);
    }
    int32ToHex(hex32Array, 0xe5f6a7b8);
    if (hexToInt32(hex32Array) != 0xe5f6a7b8) {
        printf("int32ToHex() FAILED.\n");
        exit(1);
    }
    int32ToHex(hex32Array, 0x9c9d8e7f);
    if (hexToInt32(hex32Array) != 0x9c9d8e7f) {
        printf("int32ToHex() FAILED.\n");
        exit(1);
    }
    printf("int32ToHex() PASSED.\n");


    // Test dumping a buffer.
    for (i = 0;i < 257;i ++) {
        hex08Array[i] = (char) i;
    } // for i

    requiredStringBufferSize = hexbuf2StringLength(0, false, true, 8, false);
    if (0 != requiredStringBufferSize) {
        printf("hexbuf2StringLength() case 1 FAILED.\n");
        exit(1);
    }
    requiredStringBufferSize = hexbuf2StringLength(117, false, true, 8, true);
    if (requiredStringBufferSize != (117/8 + 1) * (8 + 1 + 4*8 + 1) + 1) {
        printf("hexbuf2StringLength() case 2 FAILED.\n");
        exit(1);
    }
    requiredStringBufferSize = hexbuf2StringLength(117, false, true, 16, true);
    if (requiredStringBufferSize != (117/16 + 1) * (8 + 1 + 4*16 + 1) + 1) {
        printf("hexbuf2StringLength() case 3 FAILED.\n");
        exit(1);
    }
    requiredStringBufferSize = hexbuf2StringLength(117, false, false, 11, false);
    if (requiredStringBufferSize != (117/11 + 1) * (3*11 + 1) + 1) {
        printf("hexbuf2StringLength() case 4 FAILED.\n");
        exit(1);
    }
    requiredStringBufferSize = hexbuf2StringLength(117, true, true, 9, true);
    const unsigned EXP_H2SL_CASE5 = ((117 + 9 - 1) / 9) * (8 + 3*9 + 9 + 1 + 2) + 1;
    if (requiredStringBufferSize != EXP_H2SL_CASE5) {
        printf("hexbuf2StringLength() case 5 FAILED: expected %u got %u\n",
               EXP_H2SL_CASE5, (unsigned) requiredStringBufferSize);
        exit(1);
    }
    printf("hexbuf2StringLength() PASSED.\n");

    pStringBuffer = hexbuf2String(hex08Array, 0,
                                  hex16Array, sizeof(hex16Array),
                                  false, true, 8,
                                  true, 0);
    if (NULL != pStringBuffer) {
        printf("hexbuf2String() with length 0 FAILED.\n");
        exit(1);
    }
    printf("%s\n", hex16Array);
    pStringBuffer = hexbuf2String(hex08Array, 117,
                                  hex16Array, sizeof(hex16Array),
                                  false, true, 8,
                                  true, 0);
    if (NULL != pStringBuffer) {
        printf("hexbuf2String() with small buffer FAILED.\n");
        exit(1);
    }
    printf("%s\n", hex16Array);
    pStringBuffer = hexbuf2String(hex08Array + 0x19, 25,
                                  NULL, sizeof(hex16Array),
                                  false, true, 8,
                                  true, 0);
    if (NULL == pStringBuffer) {
        printf("hexbuf2String() with no buffer FAILED: %s\n", strerror(errno));
        exit(1);
    }
    printf("%s", pStringBuffer);
    free(pStringBuffer);
    pStringBuffer = hexbuf2String(hex08Array + 0x19, 25,
                                  NULL, sizeof(hex16Array),
                                  false, true, 8,
                                  false, 0);
    if (NULL == pStringBuffer) {
        printf("hexbuf2String() with no buffer FAILED: %s\n", strerror(errno));
        exit(1);
    }
    printf("%s", pStringBuffer);
    free(pStringBuffer);
    pStringBuffer = hexbuf2String(hex08Array + 0x19, 37,
                                  NULL, sizeof(hex16Array),
                                  false, true, 15,
                                  true, 0x12340023);
    if (NULL == pStringBuffer) {
        printf("hexbuf2String() with no buffer FAILED: %s\n", strerror(errno));
        exit(1);
    }
    printf("%s", pStringBuffer);
    free(pStringBuffer);
    pStringBuffer = hexbuf2String(hex08Array + 0x19, 37,
                                  NULL, sizeof(hex16Array),
                                  false, false, 8,
                                  true, 0x00112233);
    if (NULL == pStringBuffer) {
        printf("hexbuf2String() with no buffer FAILED: %s\n", strerror(errno));
        exit(1);
    }
    printf("%s", pStringBuffer);
    free(pStringBuffer);

    printf("hexbuf2String() PASSED.\n");

    return 0;   // All tests passed.
} // main()
#endif // TEST
