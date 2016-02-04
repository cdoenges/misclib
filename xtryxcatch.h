/** Provides simple try/catch facilities for C.

    The implementation relies completely on macros that resolve to C code
    using variables on the stack and the setjmp()/longjmp() functions.

    @note It is strongly advised to use braces after each of the macros
    (except for #XEND), but it is not required if a single statement
    follows the macro. This is exactly how C handles if/else statements.

    Example:
    <code>
        typedef enum {
            FOOBAR_EXCEPTION = 1,
            SNAFU_EXCEPTION,
            SNOWBALL_EXCEPTION,
            UNKNOWN_EXCEPTION
        } xexeption_t;

        XTRY {
            XTHROW(exception);
        } XCATCH(FOOBAR_EXCEPTION) {
            printf("Caught FOOBAR_EXCEPTION\n");
        } XCATCH(SNAFU_EXCEPTION) {
            printf("Caught SNAFU_EXCEPTION\n");
        } XCATCH(SNOWBALL_EXCEPTION) {
            printf("Caught SNOWBALL_EXCEPTION\n");
        } XCATCHALL {
            printf("Caught something else\n");
        }
        XEND;
    </code>
   @file xtryxcatch.h
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
#ifndef XTRYXCATCH_H
#define XTRYXCATCH_H

#include <setjmp.h>


/** Starts a block that will catch thrown exceptions. */
#define XTRY                    \
    {                           \
        jmp_buf jump_buffer;    \
                                \
        switch(setjmp(jump_buffer)) {   \
            case 0:             \

/** Catches the specified exception type.

    @param e An integer specifying the exception to catch.
    @pre e != 0
 */
#define XCATCH(e)               \
                break;          \
            case (e):

/** Catches all exceptions not caught using explicit #XCATCH. */
#define XCATCHALL               \
                break;          \
            default:

/** Ends a block started with #XTRY. This *MUST* be used to avoid compiler
    errors.
 */
#define XEND                    \
        }                       \
    } // XTRY


/** Throws an exception of the given type.

    @param e The exception type to throw. Must be an integer != 0.
    @pre e != 0
 */
#define XTHROW(e) longjmp(jump_buffer, e)


#endif // !XTRYXCATCH_H
