/** List all network interfaces.

    @file list_interfaces.h
    @ingroup misclib


    @author Christian D&ouml;nges <cd@platypus-projects.de>

    @note The master repository for this file is at
     <a href="https://github.com/cdoenges/misclib">https://github.com/cdoenges/misclib</a>

    LICENSE
    SPDX-License-Identifier: BSD-3-Clause


    This software is open source software under the "Simplified BSD License"
    as approved by the Open Source Initiative (OSI)
    <http://opensource.org/licenses/bsd-license.php>:


    Copyright (c) 2019, Christian Doenges (Christian D&ouml;nges) All rights
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

#ifndef LIST_INTERFACES_H
#define LIST_INTERFACES_H

#include <stdbool.h>

/** List all network interfaces to stdout.

    @param[out] error_buffer Pointer to a buffer that can hold error messages.
    @param error_buffer_size The size in bytes of the error_buffer.
    @param debug Enable debug output?
    @return Did all go well?
    @retval true All went well, the error_buffer is empty.
    @retval false An error occurred. It is described in the error_buffer.
 */
extern bool list_interfaces(char *error_buffer, size_t error_buffer_size, bool debug);
#endif // LIST_INTERFACES_H
