// misclibTest.c : Defines the entry point for the console application.
//
#include <stdbool.h>
#include <stdio.h>
#ifdef _WIN32
#include "stdafx.h"
#endif
#include "misclibTest.h"


// cd VS2013/Debug
// opencppcoverage.exe --modules misclibLIB.Windows --modules . -- misclibTest.exe



static utfunc_t unittest_functions[] = {
    unittest_factorial,
    unittest_keyvalue
};


int main(int argc, char *argv[]) {
    unsigned nrTestsExecuted;
    unsigned nrErrors = 0;


    for (nrTestsExecuted = 0;
         nrTestsExecuted < sizeof(unittest_functions) / sizeof(utfunc_t);
         nrTestsExecuted++) {
        bool result = unittest_functions[nrTestsExecuted]();

        if (false == result) {
            nrErrors++;
        }
    } // for nrTestsExecuted


    if (0 == nrErrors) {
        printf("PASS\n");
    } else {
        printf("FAIL\n");
    }
    
	return nrErrors;
} // main()

