/** Unit tests for the keyvalue module.

   @file unittest_keyvalue.c
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
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "keyvalue.h"
#include "logging.h"
#include "misclibTest.h"


#ifdef _MSC_VER
// Disable warnings for functions VS C considers deprecated.
#pragma warning(disable: 4996)
#endif // _MSC_VER


static bool unittest_keyvalue_functional(void) {
    kv_collection_t *pCollection;
    kv_object_t *theObject;

#define KEY1 (kv_key_t) "key1"
#define KEY2 (kv_key_t) "key2"
#define KEY3 (kv_key_t) "key3"
#define KEY4 (kv_key_t) "key4"
#define KEY5 (kv_key_t) "key5"
#define KEYx (kv_key_t) "keyX"
#define INT 1234567
#define FLOAT 123.45678
#define POINTER ((void *) 0x12345678)
#define STRING "Hello, world!"

    // Create a collection.
    pCollection = kv_createCollection();
    expectTrue(NULL != pCollection);

    // Searching an empty collection should return NULL.
    expectTrue(kv_findObjectForKey(pCollection, KEY1) == NULL);

    // Insert a key into the collection, find it, and check the value.
    theObject = kv_insertBool(pCollection, KEY1, true);
    expectTrue(NULL != theObject);
    expectTrue(kv_findObjectForKey(pCollection, KEY1) == theObject);
    expectTrue(kv_getTypeFromObject(theObject) == KV_VALUE_BOOL);
    expectTrue(kv_getBoolValueFromObject(theObject) == true);
    expectTrue(kv_getBool(pCollection, KEY1) == true);

    // Insert more keys into the collection.
    theObject = kv_insertInt(pCollection, KEY2, INT);
    expectTrue(NULL != theObject);
    theObject = kv_insertFloat(pCollection, KEY3, FLOAT);
    expectTrue(NULL != theObject);
    theObject = kv_insertString(pCollection, KEY4, STRING);
    expectTrue(NULL != theObject);
    theObject = kv_insertPointer(pCollection, KEY5, POINTER);
    expectTrue(NULL != theObject);

    // Check all the values.
    theObject = kv_findObjectForKey(pCollection, KEY2);
    expectTrue(NULL != theObject);
    expectTrue(kv_getTypeFromObject(theObject) == KV_VALUE_INTEGER);
    expectTrue(kv_getIntValueFromObject(theObject) == INT);
    expectTrue(kv_getInt(pCollection, KEY2) == INT);
    theObject = kv_findObjectForKey(pCollection, KEY3);
    expectTrue(NULL != theObject);
    expectTrue(kv_getTypeFromObject(theObject) == KV_VALUE_FLOAT);
    expectTrue(kv_getFloatValueFromObject(theObject) == FLOAT);
    expectTrue(kv_getFloat(pCollection, KEY3) == FLOAT);
    theObject = kv_findObjectForKey(pCollection, KEY4);
    expectTrue(NULL != theObject);
    expectTrue(kv_getTypeFromObject(theObject) == KV_VALUE_STRING);
    expectTrue(strcmp(kv_getStringValueFromObject(theObject), STRING) == 0);
    expectTrue(strcmp(kv_getString(pCollection, KEY4), STRING) == 0);
    theObject = kv_findObjectForKey(pCollection, KEY5);
    expectTrue(NULL != theObject);
    expectTrue(kv_getTypeFromObject(theObject) == KV_VALUE_POINTER);
    expectTrue(kv_getPointerValueFromObject(theObject) == POINTER);
    expectTrue(kv_getPointer(pCollection, KEY5) == POINTER);

    // Overwrite a value
    theObject = kv_insertInt(pCollection, KEY2, 1-INT);
    expectNotNull(theObject);
    expectTrue(kv_getIntValueFromObject(theObject) == 1-INT);
    theObject = kv_insertString(pCollection, KEY4, "abcdef");
    expectNotNull(theObject);
    expectTrue(strcmp(kv_getString(pCollection, KEY4), "abcdef") == 0);

    // Overwrite with the wrong type.
//    expectTrue(kv_insertBool(pCollection, KEY2, false) == NULL);

    // Check getters for non-existing keys.
    theObject = kv_findObjectForKey(pCollection, KEYx);
    expectTrue(NULL == theObject);
    expectTrue(kv_getBool(pCollection, KEYx) == false);
    expectTrue(kv_getInt(pCollection, KEYx) == 0);
    expectTrue(kv_getFloat(pCollection, KEYx) == 0.0);
    expectTrue(kv_getString(pCollection, KEYx) == NULL);
    expectTrue(kv_getPointer(pCollection, KEYx) == NULL);

    // Remove a key in the middle.
    expectTrue(kv_remove(pCollection, KEY2));
    expectNull(kv_findObjectForKey(pCollection, KEY2));
    expectFalse(kv_remove(pCollection, KEY2));
    // Remove the first key.
    expectTrue(kv_remove(pCollection, KEY1));
    expectNull(kv_findObjectForKey(pCollection, KEY1));
    // Remove the last key.
    expectTrue(kv_remove(pCollection, KEY5));
    expectNull(kv_findObjectForKey(pCollection, KEY5));

    // Free all resources.
    kv_freeCollection(pCollection);

    return true;
} // unittest_keyvalue()



static bool unittest_keyvalue_performance_write(void) {
    clock_t startclock, endclock;
    kv_collection_t *pCollection;
    int i;
    double t;


    srand(12345);
    pCollection = kv_createCollection();
    if (NULL == pCollection) {
        log_logMessage(LOGLEVEL_ERROR, "kv_createCollection() failed: %s", strerror(errno));
        return false;
    }

    log_logMessage(LOGLEVEL_INFO, "Starting kv_insertInt() timing ...");
    startclock = clock();
    for (i = 0; i < 100000; i++) {
        char keyString[32];

        sprintf(keyString, "something@%08x", i);
        kv_insertInt(pCollection, keyString, i);
    }

    endclock = clock();

    t = ((double) (endclock - startclock)) / (double) CLOCKS_PER_SEC;


    log_logMessage(LOGLEVEL_INFO,
                   "kv_insertInt()\t%.2f s (%f us/operation)",
                   t, (t / i) * 1000000.0);


    return true;
} // unittest_keyvalue_performance_write()



bool unittest_keyvalue(void) {
    bool testsAllPassed = true;

    log_logMessage(LOGLEVEL_INFO, "Testing keyvalue");

    testsAllPassed &= unittest_keyvalue_functional();
    testsAllPassed &= unittest_keyvalue_performance_write();

    return testsAllPassed;
} // unittest_keyvalue_functional()
