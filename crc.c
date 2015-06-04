/** Functions to perform CRC calculations.

    To run unit tests, compile with

    $ gcc -DTEST_CRC -DCRCTYPE=CRC8CCITT -o crctest crc.c logging.c hex.c


    @file crc.c
    @ingroup misclib

    @author Christian D&ouml;nges <cd@platypus-projects.de>

    @note The master repository for this file is at
     <a href="https://github.com/cdoenges/misclib">https://github.com/cdoenges/misclib</a>

    LICENSE

    This software is open source software under the "Simplified BSD License"
    as approved by the Open Source Initiative (OSI)
    <http://opensource.org/licenses/bsd-license.php>:


    Copyright (c) 2015, Christian Doenges (Christian D&ouml;nges) All rights
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
#include <stdlib.h>
#ifdef TEST_CRC
#include <string.h>
#endif // TEST_CRC
#ifndef CRCTYPE
#define CRCTYPE CRC8CCITT
#endif // CRCTYPE
#include "crc.h"
#define LOGGING_API_USES_VARIADIC_MACROS 1
#include "logging.h"


/** The LSB of the given value. */
#define LSB(x) ((x) & 0x01)
/** The MSB (with the CRC_SIZE_BITS) of the given value. */
#define MSB(x) ((x) & (1UL << (CRC_SIZE_BITS - 1)))



/** Reorders the bits of the given value by reflecting them about the middle
    position so that 31 <-> 0, 30 <-> 1, 29 <-> 2, ..., and 16 <-> 15.

    @param data The data to reflect.
    @param nrOfBits The number of bits in the data word.
    @return The reflected data word.
 */
static crc_t reflect(crc_t data, unsigned nrOfBits) {
    crc_t  reflection = 0;
    uint_fast8_t currentBitPos;


    assert(sizeof(data) >= nrOfBits);

    // Note: If you are handling confidential data, you must replace this
    // implementation with one that provides protection from side-channel
    // attacks.

    // Calculate the reflection by mirroring the 1 bits.
    for (currentBitPos = 0; currentBitPos < nrOfBits;currentBitPos++) {
        if (0 == data) {
            // Since we mirror only 1 bits, we're done if there are no more.
            break;
        }
        if (LSB(data) != 0) {
            reflection |= (crc_t) (1UL << ((nrOfBits - 1) - currentBitPos));
        }
        data = data >> 1;
    } // for currentBitPos

    return reflection;
} // reflect()



crc_t const *crc_init_fast(void) {
    crc_t *pTable;
    unsigned dividend;
    unsigned bit;
    crc_t remainder;


    if (NULL == (pTable = calloc(256UL, sizeof(crc_t)))) {
        log_logMessage(LOGLEVEL_ERROR, "malloc() failed.");
        return NULL;
    }


    for (dividend = 0;dividend < 256;dividend ++) {
        // The remainder starts as the dividend shifted left into position.
	//lint -e{506,778,845} Constant value for expession is a feature.
        remainder = (crc_t) (dividend << (CRC_SIZE_BITS < 8 ? 0 : CRC_SIZE_BITS - 8));

        // Perform bit-wise division modulo 2.
        for (bit = 8;bit > 0;bit --) {
            // Note: we are not worried about side-channel attacks here,
            // because this is public data.
            if (MSB(remainder) != 0) {
                remainder = (crc_t) (remainder << 1) ^ CRC_POLYNOMIAL;
            } else {
                remainder = (crc_t) (remainder << 1);
            }
        } // for bit

        // Store the result.
        pTable[dividend] = remainder;
    } // for dividend

    return pTable;
} // crc_init_fast()



crc_t crc_calc_fast(unsigned char const *pMessage, size_t length, crc_t const *pTable) {
    crc_t remainder = CRC_INITIAL_VALUE;
    uint_fast8_t dataByte;


    assert(NULL != pMessage);
    assert(NULL != pTable);

    // Divide the message by the polynomial one byte at a time.
    while (length-- > 0) {
        dataByte = *pMessage ++;

	//lint -e{506,774} Constant value for expession is a feature.
        if (CRC_REFLECT_INPUT) {
            dataByte = reflect(dataByte, 8);
        }
	//lint -e{506,778,845} Constant value for expession is a feature.
        dataByte ^= (remainder >> (CRC_SIZE_BITS < 8 ? 0 : CRC_SIZE_BITS - 8));
        remainder = pTable[(uint8_t) dataByte] ^ (crc_t) (remainder << 8);
    }

    // The CRC is the final remainder.
    //lint -e{506,774} Constant value for expession is a feature.
    if (CRC_REFLECT_RESULT) {
        remainder = reflect(remainder, CRC_SIZE_BITS);
    }
    //lint -e{835} Depending on the CRC, the final XOR value may be zero.
    return remainder ^ CRC_FINAL_XOR_VALUE;
} // crc_calc_fast()



crc_t crc_calc_slow(unsigned char const *pMessage, size_t length) {
    crc_t remainder = CRC_INITIAL_VALUE;
    uint_fast8_t dataByte, currentBitPos;


    assert(NULL != pMessage);

    // Divide the message by the polynomial one byte at a time.
    while (length-- > 0) {
        dataByte = *pMessage ++;
        //lint -e{506,774} Constant value for expession is a feature.
        if (CRC_REFLECT_INPUT) {
            dataByte = reflect(dataByte, 8);
        }

	//lint -e{506,778,845} Constant value for expession is a feature.
        remainder ^= dataByte << (CRC_SIZE_BITS < 8 ? 0 : CRC_SIZE_BITS - 8);

        // Perfom bitwise modulo-2 divison.
        for (currentBitPos = 8;currentBitPos > 0;currentBitPos--) {
            // Note: if your data is confidential, you must use a different
            // implementation below to protect against side-channel attacks.
            if (MSB(remainder) != 0) {
                remainder = (crc_t) (remainder << 1) ^ CRC_POLYNOMIAL;
            } else {
                remainder = (crc_t) (remainder << 1);
            }
        } // for currentBitPos
    } // while length

    // The CRC is the final remainder.
    //lint -e{506,774} Constant value for expession is a feature.
    if (CRC_REFLECT_RESULT) {
        remainder = reflect(remainder, CRC_SIZE_BITS);
    }
    //lint -e{835} Depending on the CRC, the final XOR value may be zero.
    return remainder ^ CRC_FINAL_XOR_VALUE;
} // crc_calc_slow()


#ifdef TEST_CRC
int main(int argc, char *argv[]) {
    uint8_t const *testVector = (uint8_t const *) "12345678";
    size_t vectorLength;
    crc_t crcSlow, crcFast;
    crc_t const *pTable;

    if (argc > 1) {
        testVector = (uint8_t const *) argv[1];
    }
    vectorLength = strlen((char *) testVector);

    log_setStdoutLevel(LOGLEVEL_DEBUG);
    log_setStderrLevel(LOGLEVEL_ERROR);

    log_logMessage(LOGLEVEL_INFO, "Using algorithm: %s", CRC_NAME);
    log_logMessage(LOGLEVEL_INFO, "Using test vector: '%s'", testVector);
    crcSlow = crc_calc_slow(testVector, vectorLength);
    log_logMessage(LOGLEVEL_INFO, "Slow CRC 0x%0*lx",
                   sizeof(crc_t) * 2, (unsigned long) crcSlow);
    pTable = crc_init_fast();
    if (NULL == pTable) {
        log_logMessage(LOGLEVEL_ERROR, "Unable to perform crc_calc_fast().");
        exit(1);
    }
    crcFast = crc_calc_fast(testVector, vectorLength, pTable);
    log_logMessage(LOGLEVEL_INFO, "Fast CRC 0x%0*lx",
                   sizeof(crc_t) * 2, (unsigned long) crcFast);
    assert(crcSlow == crcFast);

    free((void *) pTable);
    exit(0);
} // main()
#endif // TEST_CRC
