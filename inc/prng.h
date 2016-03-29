/** Functions and definitions for pseudo-random number generators.

   @file prng.h
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
#include <stdint.h>
#include <string.h>

typedef uint64_t random_number_t;

/** Function returning the next #random_number_t. */
typedef random_number_t (prng_next_t)(void);

/** Function jumping the PRNG to an area where it provides better numbers. */
typedef void (prng_jump_t)(void);


typedef struct {
    uint8_t * const pSeed;
    size_t seed_size;
    prng_next_t     *next;
    prng_jump_t     *jump;
} prng_impl_t;

extern const prng_impl_t g_xorshift64star;
extern const prng_impl_t g_xorshift1024star;
