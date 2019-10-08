/** Unit test runner for misclib.

   @file misclibTest.c
   @ingroup misclib

   @author Christian D&ouml;nges <cd@platypus-projects.de>

   @note The master repository for this file is at
    <a href="https://github.com/cdoenges/misclib">https://github.com/cdoenges/misclib</a>

    LICENSE

    Copyright 2016, 2017 Christian Doenges (Christian D&ouml;nges)

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
#include "logging.h"
#include "misclibTest.h"



// Add test functions to this array.
static utfunc_t unittest_functions[] = {
    unittest_factorial,
    unittest_keyvalue,
    unittest_lstrip,
    unittest_prng,
    unittest_ringbuffer,
    unittest_rstrip
};

static unsigned long nrTestsExecuted = 0;
static unsigned long nrErrors = 0;

void testFail(char *description) {
    printf("FAIL %s\n", description);
    nrErrors++;
} // testFail()


int main(int argc, char *argv[]) {
    (void) argc;
    (void) argv;

    log_setStdoutLevel(LOGLEVEL_INFO);

    for (nrTestsExecuted = 0;
         nrTestsExecuted < sizeof(unittest_functions) / sizeof(utfunc_t);
         nrTestsExecuted++) {
        bool result = unittest_functions[nrTestsExecuted]();

        // log_logMessage(LOGLEVEL_INFO,
        //         "Test #%lu result %s",
        //         nrTestsExecuted + 1,
        //         result ? "PASS" : "FAIL");
        if (false == result) {
            nrErrors++;
        }
    } // for nrTestsExecuted


    if (0 == nrErrors) {
        log_logMessage(LOGLEVEL_ALWAYS, "Test result: PASS");
    } else {
        log_logMessage(LOGLEVEL_ALWAYS, "Test result: FAIL");
    }

    return nrErrors;
} // main()
