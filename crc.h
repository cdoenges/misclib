/** Functions to perform CRC calculations.

    On polynomials:

    The polynomial x^4 + x + 1 may be noted in three different common notations:
        - 0x3 = 0b0011, representing x^4 +0x^3 + 0x^2 + 1x^1 + 1x^0 (MSB-first code)
        - 0xC = 0b1100, representing 1x^0 + 1x^1 + 0x^2 + 0x^3+ x^4 (LSB-first code)
        - 0x9 = 0b1001, representing 1x^4 + 0x^3 + 0x^2 + 1x^1+ x^0 (Koopman notation)

    The MSB-first code will be used in this code.


    @file crc.h
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

#ifndef CRC_H
#define CRC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


#ifndef CRCTYPE
#error CRCTYPE must be defined.
#endif // CRCTYPE

#define CRC8CCITT 0x0800

#if CRCTYPE == CRC8CCITT
/** A human-readable name for the CRC. */
#define CRC_NAME "CRC-8-CCITT"
/** The number of bits the CRC will have. */
#define CRC_SIZE_BITS 8
/** The polynomial (in MSB notation) of the CRC. */
#define CRC_POLYNOMIAL 0x07
#define CRC_INITIAL_VALUE 0x00
#define CRC_REFLECT_INPUT false
#define CRC_REFLECT_RESULT false
#define CRC_FINAL_XOR_VALUE 0x00
#define CRC_CHECK_VALUE 0xFA

#endif // CRCTYPE

#if CRC_SIZE_BITS <= 8
typedef uint8_t crc_t;
#elif CRC_SIZE_BITS <= 16
typedef uint16_t crc_t;
#elif CRC_SIZE_BITS <= 32
typedef uint32_t crc_t;
#elif CRC_SIZE_BITS <= 64
typedef uint64_t crc_t;
#else
#error Unsupported CRC_SIZE_BITS.
#endif // CRC_SIZE_BITS


/** Initializes the division table used by #crc_calc_fast.

    MUST be called before the first call to #crc_calc_fast.
    In the default implementation, the caller is responsible for free()ing
    the division table when it is no longer needed.

    @return A pointer to the initialized division table.
    @retval NULL If no memory was available.
 */
extern crc_t const *crc_init_fast(void);
extern crc_t crc_calc_slow(unsigned char const *pMessage, size_t length);
extern crc_t crc_calc_fast(unsigned char const *pMessage, size_t length, crc_t const *pTable);

#endif // CRC_H
