/** A pseudo-random number generator implementation.

   @file xorshift64star.c
   @ingroup misclib

   @author Christian D&ouml;nges <cd@platypus-projects.de>

   @note The master repository for this file is at
    <a href="https://github.com/cdoenges/misclib">https://github.com/cdoenges/misclib</a>


    Based on work found in
    "An experimental exploration of Marsaglia's xorshift generators, scrambled",
    v6 03-JAN-2016, by Sebastiano Vigna

    http://arxiv.org/abs/1402.6246


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

#include <stdint.h>
#include "prng.h"


/** The state of the PRNG must be seeded with a non-zero value. */
static uint64_t state;

uint64_t prng_xorshift64star_next(void) {
    state ^= state >> 12; // (a) First shift if the state.
    state ^= state << 25; // (b) Second shift of the state.
    state ^= state >> 27; // (c) Third shift of the state.
    // Finally, perform a multiplication with an odd number to eliminate linear
    // artifacts.
    return state * UINT64_C(2685821657736338717);
} // xorshift64star_next()



void prng_xorshift64star_jump(void) {
    // Do nothing because xorshift64star does not require any iterations to
    // behave correctly. See
    // "An experimental exploration of Marsaglia's xorshift generators, scrambled",
    // v6 03-JAN-2016, by Sebastiano Vigna http://arxiv.org/abs/1402.6246
    // for details.
} // xorshift64star_jump()


const prng_impl_t g_xorshift64star = {
    (char *) &state,
    sizeof(state),
    prng_xorshift64star_next,
    prng_xorshift64star_jump
};
