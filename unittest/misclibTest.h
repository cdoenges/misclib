#include <stdbool.h>

#define expectTrue(x) if (!(x)) { printf("ERROR: expectTrue("#x") failed.\n"); return false; }
#define expectFalse(x) if ((x)) { printf("ERROR: expectFalse("#x") failed.\n"); return false; }
#define expectNull(x) if (NULL != (x)) { printf("ERROR: expectNull("#x") failed.\n"); return false; }
#define expectNotNull(x) if (NULL == (x)) { printf("ERROR: expectNotNull("#x") failed.\n"); return false; }


typedef bool(*utfunc_t)(void);


extern bool unittest_factorial(void);
extern bool unittest_keyvalue(void);
