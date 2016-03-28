/** Provides simple try/catch facilities for C.

    The implementation relies completely on macros that resolve to C code
    using variables on the stack and the setjmp()/longjmp() functions.

    @note It is strongly advised to use braces after each of the macros
    (except for #XEND/#xtc_end), but it is not required if a single statement
    follows the macro. This is exactly how C handles if/else statements.

    There are two different implementations for different needs:

    <h3>XTRY/XCATCH/XTHROW</h3>
    Works without any global storage and is reentrant and thread-safe.
    Exceptions may be nested.
    Supports exceptions only within the current local
    scope, so you can not catch an exception thrown within a function
    called in an XTRY block.

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


    <h3>xtc_try/xtc_catch/xtc_throw</h3>
    Requires a single global variable, so it is neither reentrant nor
    thread-safe.
    Exceptions may be nested.
    Exceptions thrown by a subroutine will be caught by the closest handler
    in the call hierarchy.

    Example:
    <code>
        typedef enum {
            FOOBAR_EXCEPTION = 1,
            SNAFU_EXCEPTION,
            SNOWBALL_EXCEPTION,
            UNKNOWN_EXCEPTION
        } xexeption_t;

        xtc_buf_t *g_xtc_last = NULL;

        void foo(void) {
            xtc_try {
                xtc_throw(SNAFU_EXCEPTION);
            } xtc_catchall {
                xtc_throw(FOOBAR_EXCEPTION);
            }
            xtc_end;
        }

        xtc_try {
            XTHROW(exception);
        } xtc_catch(FOOBAR_EXCEPTION) {
            printf("Caught FOOBAR_EXCEPTION\n");
        } xtc_catch(SNAFU_EXCEPTION) {
            printf("Caught SNAFU_EXCEPTION\n");
        } xtc_catch(SNOWBALL_EXCEPTION) {
            printf("Caught SNOWBALL_EXCEPTION\n");
        } xtc_catchall {
            printf("Caught something else\n");
        }
        xtc_end;
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

//
// First variant (XTRY/XCATCH/XTHROW).
//

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


#define XTHROW(e) longjmp(jump_buffer, e)



//
// Second variant (xtc_try/xtc_catch/xtc_throw).
//

/** Contains the current exception handler context. */
typedef struct xtc_buf_s {
    /** Pointer to the previous context (i.e. the one wrapping this one). */
    struct xtc_buf_s *previous;
    /** The code of the last exception thrown or 0 if none. */
    unsigned exceptionCode;
    /** The setjmp() buffer used by the handler. */
    jmp_buf jumpBuffer;
} xtc_buf_t;



/** Global pointer to last #xtc_buf_t instance.

    @note This *must* be declared and set to NULL somewhere.
 */
extern xtc_buf_t *g_xtc_last;


/** Starts a block that will catch thrown exceptions. */
#define xtc_try                                 \
    {                                           \
        xtc_buf_t xtcBuf = { g_xtc_last, 0 };   \
        g_xtc_last = &xtcBuf;                   \
                                                \
                                                \
        switch(setjmp(xtcBuf.jumpBuffer)) {     \
            case 0:             \

/** Catches the specified exception type.

    @param e An integer specifying the exception to catch.
    @pre e != 0
 */
#define xtc_catch(e)                            \
                break;                          \
            case (e):

/** Catches all exceptions not caught using explicit #XCATCH. */
#define xtc_catchall                            \
                break;                          \
            default:

/** Ends a block started with #XTRY. This *MUST* be used to avoid compiler
    errors.
 */
#define xtc_end                                 \
        }                                       \
        g_xtc_last = xtcBuf.previous;           \
    } // xtc_try


/** Throws an exception of the given type.

    @param e The exception type to throw. Must be an integer != 0.
    @pre e != 0
 */
#define xtc_throw(e) do { g_xtc_last->exceptionCode = e; longjmp(g_xtc_last->jumpBuffer, e); } while (0 == 1)


#endif // !XTRYXCATCH_H
