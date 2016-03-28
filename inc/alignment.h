/** Aligns values and checks for alignment.

   @file alignment.h
   @ingroup misclib

   @author Christian D&ouml;nges <cd@platypus-projects.de>

   @note The master repository for this file is at
    <a href="https://github.com/cdoenges/misclib">https://github.com/cdoenges/misclib</a>

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

#include <stdint.h>

#include "static_assert.h"



/** Checks if the given value is 16 bit aligned (i.e. even).

  @param value The value to check. Note that this mus be an integer.
  @return Is the value 2 byte aligned?
  @retval true The value is aligned to a 2 byte boundary.
  @retval false The value is not aligned to a 2 byte boundary.
*/
#define isAligned2(value) (((unsigned)(value)  & 0x1) == 0)



/** Checks if the given value is 32 bit aligned (i.e. divisible by 4).

  @param value The value to check. Note that this mus be an integer.
  @return Is the value 4 byte aligned?
  @retval true The value is aligned to a 4 byte boundary.
  @retval false The value is not aligned to a 4 byte boundary.
*/
#define isAligned4(value) (((unsigned)(value)  & 0x3) == 0)



/** Returns the largest even number that is equal to or less than the given value.

  @param value The value to align. Note that this must be a positive integer.
  @return The nearest 2 byte aligned number not larger than the value.
 */
#define alignDown2(value) ((value) & ~0x01ul)



/** Returns the largest number that is divisible by 4 and equal to or
  less than the given value.

  @param value The value to align. Note that this must be a positive integer.
  @return The nearest 4 byte aligned number not larger than the value.
 */
#define alignDown4(value) ((value) & ~0x03ul)



/** Returns the smallest even number that is equal to or greater than the
  given value.

  @param value The value to align. Note that this must be a positive integer.
  @return The nearest 2 byte aligned number not smaller than the value.
 */
#define alignUp2(value) (((value) + 1) & ~0x01ul)



/** Returns the largest number that is divisible by 4 and equal to or less
  than the given value.

  @param value The value to align. Note that this must be a positive integer.
  @return The nearest 4 byte aligned number not smaller than the value.
 */
#define alignUp4(value) (((value) + 3) & ~0x03ul)



static_assert(isAligned2(2));
static_assert(!isAligned2(1));
static_assert(isAligned4(66000L));
static_assert(!isAligned4(2));
static_assert(!isAligned4(127001L));

static_assert(alignDown2(17) == 16);
static_assert(alignDown2(300000L) == 300000L);
static_assert(alignUp2(23) == 24);
static_assert(alignUp2(100019L) == 100020L);

static_assert(alignDown4(17) == 16);
static_assert(alignDown4(300000L) == 300000L);
static_assert(alignUp4(23) == 24);
static_assert(alignUp4(100019L) == 100020L);
