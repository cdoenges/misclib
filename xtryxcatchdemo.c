/** Tests simple try/catch facilities for C.

    Usage
    <pre>
    $ gcc -Wall xtryxcatchdemo.c -o xtryxcatchdemo
    $ ./xtryxcatchdemo
    </pre>
    
   @file xtryxcatchdemo.c
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

#include <stdio.h>
#include "xtryxcatch.h"


typedef enum {
    FOOBAR_EXCEPTION = 1,
    SNAFU_EXCEPTION,
    SNOWBALL_EXCEPTION,
    UNKNOWN_EXCEPTION
} xexeption_t;


void foo(void) {
    printf("Entering foo()\n");
    // Won't work:
    // XTHROW(FOOBAR_EXCEPTION);
    xtc_throw(FOOBAR_EXCEPTION);
    printf("Exiting foo()\n");
} // foo()


void bar(xexeption_t e) {
    printf("Entering bar()\n");
    xtc_throw(e);
    printf("Exiting bar()\n");
} // bar()



xtc_buf_t *g_xtc_last = NULL;

int main(int argc, char *argv[]) {
    xexeption_t exception;


    printf("Test 1\n");
    XTRY {
        printf("Nothing caught.\n");
    } XCATCH(FOOBAR_EXCEPTION) {
        printf("caught foobar\n");
    }
    XEND

    printf("Test 2\n");
    for (exception = FOOBAR_EXCEPTION;
         exception <= UNKNOWN_EXCEPTION;
         exception ++) {
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
    } // for (exception)

    printf("Test 3\n");
    for (exception = FOOBAR_EXCEPTION;
         exception <= UNKNOWN_EXCEPTION;
         exception ++) {
        XTRY
            XTHROW(exception);
        XCATCH(FOOBAR_EXCEPTION)
            printf("Caught FOOBAR_EXCEPTION\n");
        XCATCH(SNAFU_EXCEPTION)
            printf("Caught SNAFU_EXCEPTION\n");
        XCATCH(SNOWBALL_EXCEPTION)
            printf("Caught SNOWBALL_EXCEPTION\n");
        XCATCHALL
            printf("Caught something else\n");
        XEND;
    } // for (exception)

    printf("Test 4\n");
    xtc_try {
        foo();
        printf("Caught nothing.\n");
    } xtc_catch(FOOBAR_EXCEPTION) {
        printf("Caught foobar\n");
    }
    xtc_end;

    printf("Test 5\n");
    for (exception = FOOBAR_EXCEPTION;
         exception <= UNKNOWN_EXCEPTION;
         exception ++) {
        xtc_try {
            bar(exception);
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
    } // for (exception)

    printf("Test 6\n");
    for (exception = FOOBAR_EXCEPTION;
         exception <= UNKNOWN_EXCEPTION;
         exception ++) {
        xtc_try
            bar(exception);
        xtc_catch(FOOBAR_EXCEPTION)
            printf("Caught FOOBAR_EXCEPTION\n");
        xtc_catch(SNAFU_EXCEPTION)
            printf("Caught SNAFU_EXCEPTION\n");
        xtc_catch(SNOWBALL_EXCEPTION)
            printf("Caught SNOWBALL_EXCEPTION\n");
        xtc_catchall
            printf("Caught something else\n");
        xtc_end;
    } // for (exception)

    printf("Done!\n");
} // main()
