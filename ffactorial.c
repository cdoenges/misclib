/** Calculates the factorial of the given number using floating point numbers.

   @file ffactorial.c
   @ingroup misclib

   @author Christian D&ouml;nges <cd@platypus-projects.de>

   @note The master repository for this file is at
    <a href="https://github.com/cdoenges/misclib">https://github.com/cdoenges/misclib</a>


   Compile with #define UNITTEST
   (i.e. @verbatim gcc -DUNITTEST ffactorial ffactorial.c @endverbatim)
   for a self-test.


    LICENSE

    This software is open source software under the "Simplified BSD License"
    as approved by the Open Source Initiative (OSI)
    <http://opensource.org/licenses/bsd-license.php>:


    Copyright (c) 2012-2015, Christian Doenges (Christian D&ouml;nges) All rights
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


static const double m_factorials[] = {
1.0, // 0!
1.0, // 1!
2.0, // 2!
6.0, // 3!
24.0, //4!
120.0, // 5!
720.0, // 6!
5040.0, // 7!
40320.0, // 8!
362880.0, // 9!
3628800.0, // 10!
39916800.0, // 11!
479001600.0, // 12!
6227020800.0, // 13!
87178291200.0, // 14!
1307674368000.0, // 15!
20922789888000.0, // 16!
355687428096000.0, // 17!
6402373705728000.0, // 18!
121645100408832000.0, // 19!
2432902008176640000.0, // 20!
};

double ffactorial(double n) {
    double f;


    if (n < sizeof(m_factorials) / sizeof(double)) {
        // Value is stored in the table.
        f = m_factorials[(unsigned) n];
    } else {
        // Calculate factorial iteratively.
        unsigned i;

        assert(n < UINT_MAX);
        f = 1.0;
        for (i = (unsigned) n;i > 0;i --) {
            f = f * i;
        } // for i
    }

printf("ffactorial(%g) = %g\n", n, f);
    return f;

} // ffactorial()


#ifdef UNITTEST
#define MAX_FACTORIAL 100
int main(int argc, char *argv[]) {
    double f, n;


    f = 1.0;
    n = 0.0;
    while (ffactorial(n) - f < 0.001) {
        n = n + 1;
        f = f * n;
        if (n > MAX_FACTORIAL) {
            break;
        }
    }

    if (n <= MAX_FACTORIAL) {
        fprintf(stderr, "FAIL: %.0f! != %.0f (%.0f)\n", n, f, ffactorial(n));
        return 1;
    }

    printf("PASS: maximum factorial is %.0f!\n", n - 1);
    return 0;
} // main()
#endif // UNITTEST
