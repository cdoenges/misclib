/** Functions to perform conversions from integer to hexadecimal and vice-versa.


    @file hex.h
    @ingroup misclib

    @author Christian D&ouml;nges <cd@platypus-projects.de>

    @note The master repository for this file is at
     <a href="https://github.com/cdoenges/misclib">https://github.com/cdoenges/misclib</a>

    LICENSE

    This software is open source software under the "Simplified BSD License"
    as approved by the Open Source Initiative (OSI)
    <http://opensource.org/licenses/bsd-license.php>:


    Copyright (c) 2010, Christian Doenges (Christian D&ouml;nges) All rights
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

#ifndef HEX_H
#define HEX_H

#include <stdint.h>

// Suppress warnings about these symbols not being used.
//lint -esym(714, hexToInt4, hexToInt8, hexToInt16, hexToInt32, int4ToHex, int8ToHex, int16ToHex, int32ToHex)
//lint -esym(759, hexToInt4, hexToInt8, hexToInt16, hexToInt32, int4ToHex, int8ToHex, int16ToHex, int32ToHex)


/** Converts a 4 bit nibble to an (uppercase) hexadecimal digit. */
#define nibbleToHexDigit(v) ((char) ((v) >= 10 ? 'A' + (v) - 10 : '0' + (v)))

/** Converts a 4 bit nibble to a (lowercase) hexadecimal digit. */
#define nibbleToHexdigit(v) ((char) ((v) >= 10 ? 'a' + (v) - 10 : '0' + (v)))

/** Converts a hexadecimal digit to a 4 bit integer. */
#define hexdigitToNibble(n) ((unsigned char) ((n) - '0' < 10 ? (n) - '0' : (n) - 'A' < 6 ? (n) - 'A' + 10 : (n) - 'a' + 10))


/** Converts an integer nibble to a hexadecimal character.

   @param pDest The address to write the hexadecimal character to.
   @param value The value to convert to a hexadecimal digit.
   @return The hexadecimal digit as ASCII.

   @pre value >= 0
   @pre value < 16
*/
extern void int4ToHex(char *pDest, int_fast8_t value);



/** Converts an 8 bit integer to two hexadecimal digits.

   @param pDest Pointer to a buffer able to store two bytes (or more).
   @param value The value to convert to hexadecimal digits.

   @pre value >= 0
   @pre value < 256
*/
extern void int8ToHex(char *pDest, uint_fast8_t value);



/** Converts a 16 bit integer to four hexadecimal digits.

   @param pDest Pointer to a buffer able to store four bytes (or more).
   @param value The value to convert to hexadecimal digits.

   @pre value >= 0
   @pre value < 65536
*/
extern void int16ToHex(char *pDest, uint_fast16_t value);



/** Converts a 32 bit integer to eight hexadecimal digits.

   @param pDest Pointer to a buffer able to store eight bytes (or more).
   @param value The value to convert to hexadecimal digits.

   @pre value >= 0
   @pre value < 4294967296
*/
extern void int32ToHex(char *pDest, uint_fast32_t value);



/** Convers two hexadecimal digits to an 8 bit integer.

   @param pSrc Pointer to the hexadecimal digits.
   @return The integer value of the hexadecimal digits.
*/
extern uint_fast8_t hexToInt8(char const *pSrc);



/** Convers four hexadecimal digits to a 16 bit integer.

   @param pSrc Pointer to the hexadecimal digits.
   @return The integer value of the hexadecimal digits.
*/
extern uint_fast16_t hexToInt16(char const *pSrc);



/** Convers four hexadecimal digits to a 32 bit integer.

   @param pSrc Pointer to the hexadecimal digits.
   @return The integer value of the hexadecimal digits.
*/
extern uint_fast32_t hexToInt32(char const *pSrc);

#endif // HEX_H
