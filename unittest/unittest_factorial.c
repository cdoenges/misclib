#include <stdbool.h>
#include <stdio.h>
#include "factorial.h"
#include "misclibTest.h"



bool unittest_factorial(void) {
    unsigned f, n;


    f = 1;
    n = 0;
    while (factorial(n) == f) {
        n++;
        f = f * n;
    }

    expectTrue(MAX_FACTORIAL_INPUT < n);
    expectTrue(UINT_MAX == factorial(n));

    return true;
}