/** Calculates the factorial of the given number.

   @file factorial.c
   @ingroup misclib

   @author Christian D&ouml;nges <cd@platypus-projects.de>

   @note The master repository for this file is at
    <a href="https://github.com/cdoenges/misclib">https://github.com/cdoenges/misclib</a>


   Compile with UNITTEST #defined
   (i.e. gcc -DUNITTEST -DNDEBUG -o factorial factorial.c)
   for a self-test.


    LICENSE

    This software is open source software under the "Simplified BSD License"
    as approved by the Open Source Initiative (OSI)
    <http://opensource.org/licenses/bsd-license.php>:


    Copyright (c) 2012, Christian Doenges (Christian D&ouml;nges) All rights
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

#include <assert.h>
#include <limits.h>
#include <stdint.h>
#ifdef UNITTEST
#include <stdio.h>
#include <stdlib.h>
#endif // UNITTEST


#include "factorial.h"

// The maximum factorial that can be held without an overflow depends on the
// integer size.
#if UINT_MAX > UINT32_MAX
/** Maximum factorial size that fits into an (unsigned) integer. */
#define MAX_FACTORIAL 20
#elif UINT_MAX > UINT16_MAX
/** Maximum factorial size that fits into an (unsigned) integer. */
#define MAX_FACTORIAL 12
#else
/** Maximum factorial size that fits into an (unsigned) integer. */
#define MAX_FACTORIAL 8
#endif

const unsigned MAX_FACTORIAL_INPUT = MAX_FACTORIAL;


static const unsigned m_factorials[] = {
1u, // 0!
1u, // 1!
2u, // 2!
6u, // 3!
24u, //4!
120u, // 5!
720u, // 6!
5040u, // 7!
40320u, // 8!
#if UINT_MAX > UINT16_MAX
362880u, // 9!
3628800u, // 10!
39916800u, // 11!
479001600u, // 12!
#endif
#if UINT_MAX > UINT32_MAX
6227020800u, // 13!
87178291200u, // 14!
1307674368000u, // 15!
20922789888000u, // 16!
355687428096000u, // 17!
6402373705728000u, // 18!
121645100408832000u, // 19!
2432902008176640000u, // 20!
#endif
};

unsigned factorial(unsigned n) {

    if (n > MAX_FACTORIAL_INPUT) {
        // An overflow will occur, signal an error.
        return UINT_MAX;
    }

    // Note: a really smart compiler could use constant code elimination to
    // do what we're doing with the #if below (i.e. realize that calculation
    // of the factorial is required only on platforms with integers > 64 bit.
    // Experience teaches that most compilers are not that smart ...
#if UINT_MAX <= UINT64_MAX
    return m_factorials[n];
#else
    if (n < sizeof(m_factorials) / sizeof(unsigned)) {
        // Value is stored in the table.
        return m_factorials[n];
    } else {
        // Calculate factorial iteratively.
       unsigned f, i;

        f = 1;
        for (i = n;i > 0;i --) {
            f = f * i;
        } // for i

        return f;
    }
#endif // else >64 bit integers
} // factorial()


#ifdef UNITTEST
int main(int argc, char *argv[]) {
    unsigned f, n;


    f = 1;
    n = 0;
    while (factorial(n) == f) {
        n ++;
        f = f * n;
    }

    if (n <= MAX_FACTORIAL_INPUT) {
        fprintf(stderr, "FAIL: %u! != %u\n", n, f);
        return 1;
    } else if (factorial(n) != UINT_MAX) {
        fprintf(stderr, "FAIL: %u! != %u\n", n, f);
        return 1;
    }

    printf("PASS: maximum factorial is %u!\n", n - 1);
    return 0;
} // main()
#endif // UNITTEST
