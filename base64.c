/** Functions to perform conversions from binary to base64 encoded and vice-versa.


    @file base64.c
    @ingroup misclib

    @author Christian D&ouml;nges <cd@platypus-projects.de>

    @note The master repository for this file is at
     <a href="https://github.com/cdoenges/misclib">https://github.com/cdoenges/misclib</a>

    LICENSE

    This software is open source software under the "Simplified BSD License"
    as approved by the Open Source Initiative (OSI)
    <http://opensource.org/licenses/bsd-license.php>:


    Copyright (c) 2013, Christian Doenges (Christian D&ouml;nges) All rights
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


#include <string.h>
#ifdef TEST
#include <stdio.h>
#endif // TEST

#include "base64.h"



/** Encoding table as per RFC4648. */
static const unsigned char s_base64EncodingTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/** Decoding table from base64 to 6 bit binary. The value -1 means to ignore
    the current value (because it is whitespace), any other negative value
    indicates an encoding error.
 */
static const signed char s_base64DecodingTable[] = {
	-2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -1, -2, -1, -1, -2, -2,
	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
	-1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 62, -2, -2, -2, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -2, -2, -2, -1, -2, -2,
	-2,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -2, -2, -2, -2, -2,
	-2, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -2, -2, -2, -2, -2,
	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2
};



unsigned char *base64_decode(unsigned char *pInputString, size_t *pOutputLength) {
	unsigned char *pOutputData, *pOut;
	size_t inputLength, outputLength;

	if (NULL == pInputString) {
		return NULL;
	}

	inputLength = strlen(pInputString);
	if (0 == inputLength) {
		return NULL;
	}

	// The output length is roughly 3/4 the input length. We could save one
	// or two bytes, but since malloc() is usually implemented with a granularity
	// of at least 4 bytes (more commonly much more), we won't bother.
	pOutputData = calloc(inputLength * 3 / 4, sizeof(unsigned char));
	if (NULL == pOutputData) {
		return NULL;
	}
	pOut = pOutputData;

	// Iterate over all input bytes.
	outputLength = 0;
	while (inputLength > 0) {
		unsigned char inByte[4] = { 0, 0, 0, 0 };
		signed i = 0;

		while ((i < 4) && (inputLength > 0)) {
			signed inValue = s_base64DecodingTable[*pInputString++];
			inputLength --;
			if (-1 == inValue) {
				// These are characters that must be skipped, for example
				// CR, LF, space, etc.
				// NOTE: we also skip over the pad character '=' which means
				// that we effectively allow it anywhere. RFC-4648 allows padding
				// only at the end of the input.
				continue;
			} else if (inValue < 0) {
				// These are illegal characters. Abort decoding.
				free(pOutputData);
				return NULL;
			}
			inByte[i++] = (unsigned char) inValue;
		} // while i

		// Decode the entire 4 bytes of input to 3 bytes of output.
		*pOutputData++ = (unsigned char) ((inByte[0] << 2) | (inByte[1] >> 4));
		*pOutputData++ = (unsigned char) ((inByte[1] << 4) | (inByte[2] >> 2));
		*pOutputData++ = (unsigned char) (((inByte[2] << 6) & 0xC0) | inByte[3]);

		switch (i) {
			case 4:
				outputLength += 3;
				break;
			case 3:
				outputLength += 2;
				break;
			case 2:
				outputLength += 1;
				break;
			case 1:
			case 0:
				break;
			default:
				// A strange error occurred.
				free(pOutputData);
				return NULL;
		} // switch i
	} // while inputLength

	*pOutputLength = outputLength;
	return pOut;
} // base64_decode()



unsigned char *base64_encode(unsigned char *pInputData, size_t inputLength) {
	unsigned char *pOutputData;
	unsigned char *pOutputString;

	// Perform some basic input checks.
	if ((0 == inputLength) || (NULL == pInputData)) {
		return NULL;
	}


	// Base64 takes 3 input bytes and turns them into 4 output bytes.
	// The output length is the input length rounded up to multiples of 3,
	// then multiplied by 4/3. Add one for the C string terminator.
	pOutputData = (unsigned char *) calloc(((inputLength + 2) / 3) * 4 + 1, sizeof(unsigned char));
	if (NULL == pOutputData) {
		return NULL;
	}
	pOutputString = pOutputData;

	// Iterate through all the input data while there are still at least
	// 24 input bits left.
	while (inputLength >= 3) {
		unsigned char inByte0 = *pInputData++;
		unsigned char inByte1 = *pInputData++;
		unsigned char inByte2 = *pInputData++;

		*pOutputData++ = s_base64EncodingTable[inByte0 >> 2];	// Byte 0:bits 7..2
		*pOutputData++ = s_base64EncodingTable[((inByte0 & 0x03) << 4) + (inByte1 >> 4)];	// Byte 0:bits 1..0 Byte 1:bits 7..4
		*pOutputData++ = s_base64EncodingTable[((inByte1 & 0x0f) << 2) + (inByte2 >> 6)];	// Byte 1:bits 3..0 Byte 2:bits 7..6
		*pOutputData++ = s_base64EncodingTable[inByte2 & 0x3f];	// Byte 2:bits 5..0
		inputLength -= 3;
	} // while inputLength >= 3

	// Handle the remaining 8 or 16 bits.
	if (0 != inputLength) {
		unsigned char inByte0 = *pInputData++;
		unsigned char inByte1;

		*pOutputData++ = s_base64EncodingTable[inByte0 >> 2];	// Byte 0:bits 7..2
		if (1 == inputLength) {
			*pOutputData++ = s_base64EncodingTable[((inByte0 & 0x03) << 4)];	// Byte 0:bits 1..0
			*pOutputData++ = '=';
		} else {
			inByte1 = *pInputData++;
			*pOutputData++ = s_base64EncodingTable[((inByte0 & 0x03) << 4) + (inByte1 >> 4)];	// Byte 0:bits 1..0 Byte 1:bits 7..4
			*pOutputData++ = s_base64EncodingTable[((inByte1 & 0x0f) << 2)];	// Byte 1:bits 3..0
		}
		*pOutputData++ = '=';
	} // if inputLength

	// Terminate the C string.
	*pOutputData = '\0';

	return pOutputString;
} // base64_encode()



#ifdef TEST
// To compile and run the test harness, use
//
// gcc -g -o base64test -D TEST -Wall base64.c && ./base64test
//

// Test vectors are from RFC-4648.
#define NR_TESTCASES 7
unsigned char *tv_plain[NR_TESTCASES] = {
	"",
	"f",
	"fo",
	"foo",
	"foob",
	"fooba",
	"foobar"
};
unsigned char *tv_encoded[NR_TESTCASES] = {
	"",
	"Zg==",
	"Zm8=",
	"Zm9v",
	"Zm9vYg==",
	"Zm9vYmE=",
	"Zm9vYmFy"
};

int main(int argc, char **argv) {
	unsigned tc;
	unsigned nr_of_errors = 0;

	for (tc = 0;tc < NR_TESTCASES;tc ++) {
		unsigned char *pEncoded, *pPlain;
		size_t plainLen;

		pEncoded = base64_encode(tv_plain[tc], strlen(tv_plain[tc]));
//		printf("BASE64(%s) = %s\n", tv_plain[tc], pEncoded);
		if ((NULL == pEncoded) && (strlen(tv_plain[tc]) == 0)) {
			// Special handling required because 0-length strings are not supported.
			pEncoded = "";
		}
		if (NULL == pEncoded) {
			fprintf(stderr, "TC %d ... FAIL (NULL encoding).\n", tc);
			nr_of_errors++;
			continue;
		}
		if (0 != strcmp(pEncoded, tv_encoded[tc])) {
			fprintf(stderr, "TC %d ... FAIL (incorrect encoding).\n", tc);
			fprintf(stderr, "TC %d: expected '%s' got '%s'.\n", tc, tv_encoded[tc], pEncoded);
			nr_of_errors++;
			continue;
		}
		pPlain = base64_decode(tv_encoded[tc], &plainLen);
		if ((NULL == pPlain) && (strlen(tv_encoded[tc]) == 0)) {
			// Special handling required because 0-length strings are not supported.
			pPlain = "";
		}
//		printf("PLAIN64(%s) = %s\n", tv_encoded[tc], pPlain);
		if (NULL == pPlain) {
			fprintf(stderr, "TC %d ... FAIL (NULL decoding).\n", tc);
			nr_of_errors++;
			continue;
		}
		if (plainLen != strlen(tv_plain[tc])) {
			fprintf(stderr, "TC %d ... FAIL (incorrect decoded length).\n", tc);
			fprintf(stderr, "TC %d: expected %zu got %zu.\n", tc, strlen(tv_plain[tc]), plainLen);
			nr_of_errors++;
			continue;
		}
		if (0 != strcmp(pPlain, tv_plain[tc])) {
			fprintf(stderr, "TC %d ... FAIL (incorrect decoding).\n", tc);
			nr_of_errors++;
			continue;
		}
		printf("TC %d ... PASS.\n", tc);
	} // for tc

	if (0 == nr_of_errors) {
		printf("Result: PASS\n");
	} else if (1 == nr_of_errors) {
		printf("Result: FAIL (1 error)\n");
	} else {
		printf("Result: FAIL (%d errors)\n", nr_of_errors);
	}

	return nr_of_errors;
} // main()
#endif // TEST