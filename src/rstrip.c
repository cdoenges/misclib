/** Strip whitespace from the end of a string.

   @file rstrip.c
   @ingroup misclib

   @author Christian D&ouml;nges <cd@platypus-projects.de>

   @note The master repository for this file is at
    <a href="https://github.com/cdoenges/misclib">https://github.com/cdoenges/misclib</a>


    LICENSE

    Copyright 2016 Christian Doenges (Christian D&ouml;nges)

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at
 
      http://www.apache.org/licenses/LICENSE-2.0
 
    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
 */

#include <ctype.h>
#include <stdlib.h>
#include "stringfunctions.h"



/**
 * Whitespace at the end of the given string is stripped by truncating the
 * string in place. Truncation is performed by overwriting the last
 * whitespace character with '\0'.
 *
 * Anything that isspace() (see "ctype.h") considers whitespace is whitespace
 * to this function.
 *
 * @param pStartOfString Pointer to the string to strip trailing whitespace from.
 * @return The new length of the string.
 */
size_t rstrip(char *pStartOfString) {
    char *pEndOfString = pStartOfString + strlen(pStartOfString);

    while (--pEndOfString >= pStartOfString) {
        if (!isspace(*pEndOfString)) {
            *(pEndOfString + 1) = '\0';
            break;
        } // if not space
    } // while index > 0

    return (size_t) pEndOfString - (size_t) pStartOfString + 1;
} // size_t rstrip(char *pStartOfString)
