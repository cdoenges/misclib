/** Unit tests for pseudo-random number generators (PRNGs).

   @file unittest_rstrip.c
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
#include "misclibTest.h"
#include "stringfunctions.h"



static const unsigned NUMBER_OF_TEST_VECTORS = 8;
static char *testvector[NUMBER_OF_TEST_VECTORS] = {
    "NOWHITESPACE",
    "SPACEATEND ",
    "SPACEATEND123., ",
    "WHITESPACESATEND \t\n\v\f\r ",
    "SPACES IN STRING",
    "   SPACESINFRONT",
    "\rWHITESPACE \tALL\nOVER\v",
    "NON-ASCII STRING\xfb\xac\xe4 "
};

static char *expected[NUMBER_OF_TEST_VECTORS] = {
    "NOWHITESPACE",
    "SPACEATEND",
    "SPACEATEND123.,",
    "WHITESPACESATEND",
    "SPACES IN STRING",
    "   SPACESINFRONT",
    "\rWHITESPACE \tALL\nOVER",
    "NON-ASCII STRING\xfb\xac\xe4"
};


bool unittest_rstrip(void) {
    char testbuffer[256];
    size_t l;
    unsigned i;
    int failed = 0;

    for (i = 0;i < NUMBER_OF_TEST_VECTORS;i ++) {
        strncpy(testbuffer, testvector[i], sizeof(testbuffer));
        l = rstrip(testbuffer);
//        printf("Test #%d: %d - '%s'\n", i, l, testbuffer);
        if ((strlen(expected[i]) != l) || (strcmp(testbuffer, expected[i]) != 0)) {
            printf("Failed test #%d\n", i);
            failed++;
        } // if failed
    } // for i

    return (0 == failed);
} // unittest_rstrip()
