/** Converts an integer to an ASCII string with the given radix.

    @file itoa.h
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

#ifndef ITOA_H
#define ITOA_H

#include <stddef.h>


/** Converts an integer to an ASCII representation with the given base.

  @param value The integer value to convert to ASCII.
  @param pDst  The location in memory for the ASCII representation. There
     must be space for all digits plus the NUL terminator character.
  @param buflen The number of bytes in the buffer pointed at by pDst.
  @param base  The radix (base) of the conversion. Common values are
     2, 8, 10, and 16.
  @return The location of the converted number.
  @retval NULL An invalid base was specified or the conversion was incomplete
     to avoid a buffer overflow.
 */
extern char *intoa(int value, char *pDst, size_t buflen, int base);



/** Converts an integer to an ASCII representation with the given base.

  @param value The integer value to convert to ASCII.
  @param pDst  The location in memory for the ASCII representation. There
     must be space for all digits plus the NUL terminator character.
  @param base  The radix (base) of the conversion. Common values are
     2, 8, 10, and 16.
  @return The location of the converted number.
  @retval NULL An invalid base was specified.

  @note This function can result in a buffer overflow. Use intoa to
     protect from this possibility.

  The prototype for this function was suggested by Kernighan and Ritchie
  "The C Programming Language" (both editions).
 */
extern char *itoa(int value, char *pDst, int base);


#endif // ITOA_H

