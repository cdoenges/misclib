/** Key-value encoded data storage.


    @file keyvalue.c
    @ingroup misclib

    @author Christian D&ouml;nges <cd@platypus-projects.de>

    @note The master repository for this file is at
     <a href="https://github.com/cdoenges/misclib">https://github.com/cdoenges/misclib</a>

    LICENSE

    This software is open source software under the "Simplified BSD License"
    as approved by the Open Source Initiative (OSI)
    <http://opensource.org/licenses/bsd-license.php>:


    Copyright (c) 2010-2016, Christian Doenges (Christian D&ouml;nges) All rights
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

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "keyvalue.h"


#ifdef _MSC_VER
// Disable warnings for functions VS C considers deprecated.
#pragma warning(disable: 4996)
#endif // _MSC_VER

kv_object_t *kv_initializeIterator(kv_iterator_t *pIterator,
                                   kv_collection_t const *pCollection) {
    assert(NULL != pIterator);
    assert(NULL != pCollection);

    *pIterator = pCollection->first;
    return *pIterator;
} // end kv_initializeIterator()



kv_object_t *kv_iterateNext(kv_iterator_t *pIterator) {
    assert(NULL != pIterator);


    if (NULL != *pIterator) {
        kv_object_t *pObject = *pIterator;
        *pIterator = pObject->next;
    }

    return *pIterator;
} // end kv_iterateNext()



kv_collection_t *kv_createCollection(void) {
    kv_collection_t *pCollection = calloc(1ul, sizeof(kv_collection_t));

    // Note: the following is only needed for the odd platform where
    // NULL is not identical to binary 0.
    if (NULL != pCollection) {
        pCollection->first = NULL;
        pCollection->last  = NULL;
    }

    return pCollection;
} // end kv_createCollection()



void kv_clearCollection(kv_collection_t *pCollection) {
    kv_iterator_t iterator;
    kv_object_t *pObject;

    assert(NULL != pCollection);


    pObject = kv_initializeIterator(&iterator, pCollection);
    while (NULL != pObject) {
        kv_object_t *pOld = pObject;

        pObject = kv_iterateNext(&iterator);
        kv_freeObject(pOld);
    }

    pCollection->first = NULL;
    pCollection->last  = NULL;
} // end kv_clearCollection()



void kv_freeCollection(kv_collection_t *pCollection) {
    assert(NULL != pCollection);

    kv_clearCollection(pCollection);
    free(pCollection);
} // end kv_freeCollection()



kv_object_t *kv_createObject(kv_key_t pKey) {
    kv_object_t *pObject;
    assert(NULL != pKey);

    if (NULL == (pObject = calloc(1ul, sizeof(kv_object_t)))) {
        // Out of memory.
        return NULL;
    }

    if (NULL == (pObject->key = (kv_key_t) strdup((char *) pKey))) {
        // Out of memory.
        free(pObject);
        return NULL;
    }

    pObject->next = NULL;
    return pObject;
} // end kv_createObject()



void kv_freeObject(kv_object_t *pObject) {
    assert(NULL != pObject);

    if (KV_VALUE_STRING == pObject->type) {
        if (NULL != pObject->value.s) {
            free(pObject->value.s);
        }
    }

    free(pObject);
} // end freeObject()



kv_object_t *kv_findObjectForKey(kv_collection_t const *pCollection, kv_key_t pKey) {
    kv_iterator_t iterator;
    kv_object_t   *pObject;


    assert(NULL != pCollection);
    assert(NULL != pKey);

    // Find the object matching the given key.
    pObject = kv_initializeIterator(&iterator, pCollection);
    while (NULL != pObject) {
        assert(NULL != pObject->key);
        if (strcmp((char *) pKey, (char *) pObject->key) == 0) {
            return pObject;
        }
        pObject = kv_iterateNext(&iterator);
    } // while pObject

    return NULL;
} // end kv_findObjectForKey()



void kv_addObjectToCollection(kv_collection_t *pCollection, kv_object_t *pObject) {
    assert(NULL != pCollection);
    assert(NULL != pObject);
    assert(NULL == pObject->next);

    if (NULL == pCollection->first) {
        // This is the first (and only) object in the collection.
        pCollection->first = pCollection->last = pObject;
        return;
    }

    // Insert object at the end of the list.
    pCollection->last->next = pObject;
    pCollection->last = pObject;
} // end kv_addObjectToCollection()



kv_object_t *kv_insertBool(kv_collection_t *pCollection, kv_key_t pKey, bool value) {
    kv_object_t   *pObject;

    assert(NULL != pCollection);
    assert(NULL != pKey);


    // Find the object matching the given key.
    pObject = kv_findObjectForKey(pCollection, pKey);
    if (NULL == pObject) {
        // No object was found, so create it and add it to the collection.
        if ((pObject = kv_createObject(pKey)) == NULL) {
            // Out of memory error.
            return NULL;
        }
        pObject->type = KV_VALUE_BOOL;
        kv_addObjectToCollection(pCollection, pObject);
    }

    // Enter the value.
    assert(KV_VALUE_BOOL == pObject->type);
    pObject->value.b = value;
    return pObject;
} // end kv_insertBool()



kv_object_t *kv_insertInt(kv_collection_t *pCollection, kv_key_t pKey, int value) {
    kv_object_t   *pObject;

    assert(NULL != pCollection);
    assert(NULL != pKey);


    // Find the object matching the given key.
    pObject = kv_findObjectForKey(pCollection, pKey);
    if (NULL == pObject) {
        // No object was found, so create it and add it to the collection.
        if ((pObject = kv_createObject(pKey)) == NULL) {
            // Out of memory error.
            return NULL;
        }
        pObject->type = KV_VALUE_INTEGER;
        kv_addObjectToCollection(pCollection, pObject);
    }

    // Enter the value.
    assert(KV_VALUE_INTEGER == pObject->type);
    pObject->value.i = value;
    return pObject;
} // end insertInt()



kv_object_t *kv_insertFloat(kv_collection_t *pCollection, kv_key_t pKey, double value) {
    kv_object_t   *pObject;

    assert(NULL != pCollection);
    assert(NULL != pKey);


    // Find the object matching the given key.
    pObject = kv_findObjectForKey(pCollection, pKey);
    if (NULL == pObject) {
        // No object was found, so create it and add it to the collection.
        if ((pObject = kv_createObject(pKey)) == NULL) {
            // Out of memory error.
            return NULL;
        }
        pObject->type = KV_VALUE_FLOAT;
        kv_addObjectToCollection(pCollection, pObject);
    }

    // Enter the value.
    assert(KV_VALUE_FLOAT == pObject->type);
    pObject->value.f = value;
    return pObject;
} // end insertFloat()



kv_object_t *kv_insertPointer(kv_collection_t *pCollection, kv_key_t pKey, void const *value) {
    kv_object_t   *pObject;

    assert(NULL != pCollection);
    assert(NULL != pKey);


    // Find the object matching the given key.
    pObject = kv_findObjectForKey(pCollection, pKey);
    if (NULL == pObject) {
        // No object was found, so create it and add it to the collection.
        if ((pObject = kv_createObject(pKey)) == NULL) {
            // Out of memory error.
            return NULL;
        }
        pObject->type = KV_VALUE_POINTER;
        kv_addObjectToCollection(pCollection, pObject);
    }

    // Enter the value.
    assert(KV_VALUE_POINTER == pObject->type);
    pObject->value.p = (void *) value;
    return pObject;
} // end kv_insertPointer()



kv_object_t *kv_insertString(kv_collection_t *pCollection, kv_key_t const pKey, char const *value) {
    kv_object_t   *pObject;

    assert(NULL != pCollection);
    assert(NULL != pKey);


    // Find the object matching the given key.
    pObject = kv_findObjectForKey(pCollection, pKey);
    if (NULL == pObject) {
        // No object was found, so create it and add it to the collection.
        if ((pObject = kv_createObject(pKey)) == NULL) {
            // Out of memory error.
            return NULL;
        }
        pObject->type = KV_VALUE_STRING;
        kv_addObjectToCollection(pCollection, pObject);
    } else {
        // The key exists, so the value must be replaced.
        assert(KV_VALUE_STRING == pObject->type);
        if (NULL != pObject->value.s) {
            free(pObject->value.s);
        }
    }

    // Enter the value.
    pObject->value.s = strdup(value);
    return pObject;
} // end kv_insertString()



bool kv_remove(kv_collection_t *pCollection, kv_key_t pKey) {
    kv_iterator_t iterator;
    kv_object_t   *pObject, *pPreviousObject = NULL;


    assert(NULL != pCollection);
    assert(NULL != pKey);

    // Find the object matching the given key.
    pObject = kv_initializeIterator(&iterator, pCollection);
    while (NULL != pObject) {
        assert(NULL != pObject->key);
        if (strcmp((char *) pKey, (char *) pObject->key) == 0) {
            break;
        }
        pPreviousObject = pObject;
        pObject = kv_iterateNext(&iterator);
    } // while pObject

    if (NULL == pObject) {
        // The key was not found.
        return false;
    }

    // Remove the object from the linked list.
    if (NULL == pPreviousObject) {
        assert(pObject == pCollection->first);
        pCollection->first = pObject->next;
    } else {
        assert(pObject == pPreviousObject->next);
        pPreviousObject->next = pObject->next;
    }
    if (pCollection->last == pObject) {
        pCollection->last = pPreviousObject;
    }

    kv_freeObject(pObject);
    return true;
} // end kv_remove()



kv_value_type_t kv_getTypeFromObject(kv_object_t const *pObject) {
    assert(NULL != pObject);

    return pObject->type;
} // end kv_getTypeFromObject()



bool kv_getBoolValueFromObject(kv_object_t const *pObject) {
    assert(NULL != pObject);
    assert(KV_VALUE_BOOL == pObject->type);

    return pObject->value.b;
} // end kg_getBoolValueFromObject()



int kv_getIntValueFromObject(kv_object_t const *pObject) {
    assert(NULL != pObject);
    assert(KV_VALUE_INTEGER == pObject->type);

    return pObject->value.i;
} // end kg_getIntValueFromObject()



double kv_getFloatValueFromObject(kv_object_t const *pObject) {
    assert(NULL != pObject);
    assert(KV_VALUE_FLOAT == pObject->type);

    return pObject->value.f;
} // end kg_getFloatValueFromObject()



void *kv_getPointerValueFromObject(kv_object_t const *pObject) {
    assert(NULL != pObject);
    assert(KV_VALUE_POINTER == pObject->type);

    return pObject->value.p;
} // end kv_getPointerValueFromObject()



char const *kv_getStringValueFromObject(kv_object_t const *pObject) {
    assert(NULL != pObject);
    assert(KV_VALUE_STRING == pObject->type);

    return pObject->value.s;
} // end kg_getStringValueFromObject()



bool kv_getBool(kv_collection_t const *pCollection, kv_key_t const pKey) {
    kv_object_t *obj = kv_findObjectForKey(pCollection, pKey);


    if (NULL == obj) {
        return false;
    }

    assert(KV_VALUE_BOOL == obj->type);
    return kv_getBoolValueFromObject(obj);
} // end kv_getBool()



int kv_getInt(kv_collection_t const *pCollection, kv_key_t const pKey) {
    kv_object_t *obj = kv_findObjectForKey(pCollection, pKey);


    if (NULL == obj) {
        return 0;
    }

    assert(KV_VALUE_INTEGER == obj->type);
    return kv_getIntValueFromObject(obj);
} // end kv_getInt()



double kv_getFloat(kv_collection_t const *pCollection, kv_key_t const pKey) {
    kv_object_t *obj = kv_findObjectForKey(pCollection, pKey);


    if (NULL == obj) {
        return 0.0;
    }

    assert(KV_VALUE_FLOAT == obj->type);
    return kv_getFloatValueFromObject(obj);
} // end kv_getFloat()



void *kv_getPointer(kv_collection_t const *pCollection, kv_key_t const pKey) {
    kv_object_t *obj = kv_findObjectForKey(pCollection, pKey);


    if (NULL == obj) {
        return NULL;
    }

    assert(KV_VALUE_POINTER == obj->type);
    return kv_getPointerValueFromObject(obj);
} // end kv_getPointer()



char const *kv_getString(kv_collection_t const *pCollection, kv_key_t const pKey) {
    kv_object_t *obj = kv_findObjectForKey(pCollection, pKey);


    if (NULL == obj) {
        return NULL;
    }

    assert(KV_VALUE_STRING == obj->type);
    return kv_getStringValueFromObject(obj);
} // end kv_getString()
