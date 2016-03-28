/** Functions to perform conversions from binary to base64 encoded and vice-versa.



    @file base64.h
    @ingroup misclib

    @author Christian D&ouml;nges <cd@platypus-projects.de>

    @note The master repository for this file is at
     <a href="https://github.com/cdoenges/misclib">https://github.com/cdoenges/misclib</a>

    LICENSE

    This software is open source software under the "Simplified BSD License"
    as approved by the Open Source Initiative (OSI)
    <http://opensource.org/licenses/bsd-license.php>:


    Copyright (c) 2013, Christian Doenges (Christian D&ouml;nges) All rights
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

#ifndef BASE64_H
#define BASE64_H

#include <stdlib.h>



// Suppress warnings about these symbols not being used.
//lint -esym(714, base64_decode, base64_encode)
//lint -esym(757, base64_decode, base64_encode)
//lint -esym(759, base64_decode, base64_encode)


/** Converts a Base64 encoded C string to binary data occording to RFC-4648.

  @param pInputString Pointer to the Base64 encoded input string.
  @return A pointer to the binary decoded data. The caller is responsible
      for calling free() to release the memory occupied.
  @retval NULL Either no input was specified or the output buffer could not
      be allocated.
 */
unsigned char *base64_decode(unsigned char const *pInputString, size_t *pOutputLength);


/** Converts the given binary input to a C string in Base64 encoding according
    to RFC-4648.

  @param pInputData Pointer to the binary input data.
  @param inputLength The number of input bytes.
  @return A pointer to the output string. The caller is responsible for calling
      free() to release the memory occupied.
  @retval NULL Either no input was specified or the output buffer could not
      be allocated.
 */
unsigned char *base64_encode(unsigned char const *pInputData, size_t inputLength);
#endif // BASE64_H
