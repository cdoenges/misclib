/** A pseudo-random number generator implementation.

   @file xorshift1024star.c
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



#define NR_STATE_WORDS 16
#define STATE_WORD_MASK (NR_STATE_WORDS - 1)

/** The state MUST be initialized to a non-zero value before calling next(). */
static uint64_t state[NR_STATE_WORDS];
/** The 64-bit word currently being modified. */
static int state_word_index = 1;


/** Returns the next random value in the sequence.

    @return A pseudo-random number.
 */
uint64_t prng_xorshift1024star_next(void) {
	const uint64_t s0 = state[state_word_index];	// The first work word remains unchanged.
	uint64_t s1;	// The second work word is modified.

	// Select the second work word.
	state_word_index = (state_word_index + 1) & STATE_WORD_MASK;
	s1 = state[state_word_index];

	s1 ^= s1 << 31; // (a) The first shift/xor transformation.
	state[state_word_index] = s1 ^ s0 ^ (s1 >> 11) ^ (s0 >> 30); // (b, c) The second and third shift/xor operations.
	// Multiply with an odd number to remove linear artefacts.
	return state[state_word_index] * UINT64_C(1181783497276652981);
} // prng_xorshift1024star_next


/** Equivalent to calling prng_xorshift1024star_next() 2^512 times. */
void prng_xorshift1024star_jump() {
	static const uint64_t JUMP[] = {
		0x84242f96eca9c41dULL, 0xa3c65b8776f96855ULL, 0x5b34a39f070b5837ULL,
		0x4489affce4f31a1eULL, 0x2ffeeb0a48316f40ULL, 0xdc2d9891fe68c022ULL,
		0x3659132bb12fea70ULL, 0xaac17d8efa43cab8ULL, 0xc4cb815590989b13ULL,
		0x5ee975283d71c93bULL, 0x691548c86c1bd540ULL, 0x7910c41d10a1e6a5ULL,
		0x0b5fc64563b3e2a8ULL, 0x047f7684e9fc949dULL, 0xb99181f2d8f685caULL,
		0x284600e3f30e38c3ULL
	};

	uint64_t t[NR_STATE_WORDS] = { 0 };


	for(int i = 0; i < sizeof JUMP / sizeof *JUMP; i++) {
		for(int bit_index = 0; bit_index < 64; bit_index++) {
			if (JUMP[i] & 1ULL << bit_index) {
				for(int j = 0; j < NR_STATE_WORDS; j++) {
					t[j] ^= state[(j + state_word_index) & STATE_WORD_MASK];
				} // for j
			} // if
            prng_xorshift1024star_next();
		} // for bit_index
	} // for i

	for(int j = 0;j < NR_STATE_WORDS;j++) {
		state[(j + state_word_index) & STATE_WORD_MASK] = t[j];
	} // for j
} // prng_xorshift1024star_jump()



const prng_impl_t g_xorshift1024star = {
    (uint8_t * const) &state,
    sizeof(state),
    prng_xorshift1024star_next,
    prng_xorshift1024star_jump
};
