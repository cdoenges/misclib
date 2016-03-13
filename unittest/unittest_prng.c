/** Unit tests for pseudo-random number generators (PRNGs).

   @file unittest_prng.c
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
#include <stdbool.h>
#include <stdio.h>
#include "prng.h"
#include "misclibTest.h"



static bool unittest_prng_xorshift64star(void) {
    prng_impl_t const *pPrng = &g_xorshift64star;
    random_number_t r1, r2;


    // Generate a non-zero seed.
    for (size_t i = 0;i < pPrng->seed_size;i ++) {
        *(pPrng->pSeed + i) = 0x55;
    }

    // Jump start the PRNG to get to the better random numbers.
    pPrng->jump();

    // Get two random numbers.
    r1 = pPrng->next();
    r2 = pPrng->next();

    expectTrue(r1 != r2);

    return true;
} // unittest_prng_xorshift64star()



bool unittest_prng(void) {
    expectTrue(unittest_prng_xorshift64star());

    return true;
} // unittest_prng()