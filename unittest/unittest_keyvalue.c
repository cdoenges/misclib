#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "keyvalue.h"
#include "misclibTest.h"



bool unittest_keyvalue(void) {
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
