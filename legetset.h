/** Get and set 16 and 32 bit values in memory without alignment restrictions.

   @file legetset.h
   @defgroup misclib

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


#include <stddef.h>
#include <stdint.h>



/** Writes an unsigned 16-bit integer to a non-aligned location in RAM
   using 2 byte little-endian format.

   @param pDst  The destination location in RAM to write to.
   @param value The value to write.
 */
extern void leSetUint16(void *pDest, uint_fast16_t value);



/** Reads a little-endian 2 byte value from a non-aligned location in memory
   and returns it as an unsigned 16-bit integer.

   @param pSrc The source location in memory to read from.
   @return The value read.
 */
extern uint_fast16_t leGetUint16(void const *pSrc);



/** Writes an unsigned 32-bit integer to a non-aligned location in RAM using
   4 byte little-endian format.

   @param pDst The destination in RAM to write to.
   @param value The value to write.
 */
extern void leSetUint32(void *pDest, uint_fast32_t value);



/** Reads a little-endian 4 byte value from a non-aligned location in memory
   and returns it as an unsigned 32-bit integer.

   @param pSrc The source location in memory to read from.
   @return The value read.
 */
extern uint_fast32_t leGetUint32(void const *pSrc);
