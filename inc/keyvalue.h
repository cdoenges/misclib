/** Key-value encoded data storage.

    Each key-value object contains a number of keys with an attached value.

    The key is always a string in the current implementation. The value is
    a boolean, an integer, a floating-point number, a pointer, or a string.

    Note that this implementation could be improved in performance by choosing
    a different way of organizing the data, such as a hash table. This is
    currently not implemented.


    @file keyvalue.h
    @ingroup misclib

    @author Christian D&ouml;nges <cd@platypus-projects.de>

    @note The master repository for this file is at
     <a href="https://github.com/cdoenges/misclib">https://github.com/cdoenges/misclib</a>


    LICENSE

    This software is open source software under the "Simplified BSD License"
    as approved by the Open Source Initiative (OSI)
    <http://opensource.org/licenses/bsd-license.php>:


    Copyright (c) 2010-2015, Christian Doenges (Christian D&ouml;nges) All rights
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
#ifndef KEYKV_VALUE_H
#define KEYKV_VALUE_H

#include <stdbool.h>


#undef MISCLIB_EXTERN
#if defined(_WIN32)
#if defined(MISCLIB_DLL)
#ifdef BUILD_MISCLIB
// Export symbols while building this part of the DLL.
#define MISCLIB_EXTERN __declspec(dllexport)
#else
// Import symbols while using the DLL.
#define MISCLIB_EXTERN __declspec(dllimport)
#endif // !BUILD_MISCLIB
#elif defined(MISCLIB_STATIC)
#define MISCLIB_EXTERN
#else
#error One of MISCLIB_DLL or MISCLIB_STATIC must be defined for Windows builds.
#endif
#else
#define MISCLIB_EXTERN extern
#endif // !_WIN32

// This header defines an API, do not complain if functions are not used.
//lint -esym(714, kv_initializeIterator, kv_iterateNext, kv_createCollection, kv_clearCollection, kv_freeCollection, kv_createObject, kv_freeObject, kv_findObjectForKey, kv_addObjectToCollection, kv_remove, kv_getTypeFromObject, kv_getBoolValueFromObject, kv_getIntValueFromObject, kv_getFloatValueFromObject, kv_getPointerValueFromObject, kv_getStringValueFromObject, kv_insertBool, kv_insertInt, kv_insertFloat, kv_insertPointer, kv_insertString, kv_getBool, kv_getInt, kv_getFloat, kv_getPointer, kv_getString)
//lint -esym(759, kv_initializeIterator, kv_iterateNext, kv_createCollection, kv_clearCollection, kv_freeCollection, kv_createObject, kv_freeObject, kv_findObjectForKey, kv_addObjectToCollection, kv_remove, kv_getTypeFromObject, kv_getBoolValueFromObject, kv_getIntValueFromObject, kv_getFloatValueFromObject, kv_getPointerValueFromObject, kv_getStringValueFromObject, kv_insertBool, kv_insertInt, kv_insertFloat, kv_insertPointer, kv_insertString, kv_getBool, kv_getInt, kv_getFloat, kv_getPointer, kv_getString)


/** The type to use for object keys. */
typedef char const *kv_key_t;


/** All the supported types for the value. */
typedef enum {
    /** The value type has not been specified. This is a programming error. */
    KV_VALUE_UNSPECIFIED = 0,
    /** The value is a boolean. */
    KV_VALUE_BOOL,
    /** The value is a (long) integer. */
    KV_VALUE_INTEGER,
    /** The value is a floating-point number. */
    KV_VALUE_FLOAT,
    /** The value is a string. */
    KV_VALUE_STRING,
    /** The value is a pointer. */
    KV_VALUE_POINTER,
    /** The value type is unknown and unsupported. */
    KV_VALUE_UNKNOWN
} kv_value_type_t;


/** Each Key-Value object contains the key, the type of the value, and the
  value itself.
 */
struct s_kv_object {
    /** The Key-Value map is a linked list. This is the link to the next
       data object.
     */
    struct s_kv_object *next;
    /** The key is an identifier that must be unique in the map. It is case
       sensitive.
     */
    kv_key_t key;
    /** The type of the value. Used to decode the union, below. */
    kv_value_type_t type;
    /** The different value types are encoded in a union. */
    union {
        /** Boolean value. */
        bool   b;
        /** Integer value. */
        int    i;
        /** Floating-point value. */
        double f;
        /** Pointer value. */
        void *p;
        /** String value. */
        char  *s;
    } value;
};
typedef struct s_kv_object kv_object_t;



/** A number of key-value objects are grouped in a collection */
typedef struct {
    /** The first object in the collection. */
    kv_object_t *first;
    /** The last object in the collection. This is used for performance optimization. */
    kv_object_t *last;
} kv_collection_t;



/** An iterator to move through the list of objects. */
typedef kv_object_t *kv_iterator_t;



/** Initializes the given iterator with the first object in the collection.

   @param pIterator A pointer to the iterator.
   @param pCollection A pointer to the collection to iterate.
   @return The first object in the collection or NULL if the collection is empty.
 */
MISCLIB_EXTERN kv_object_t *kv_initializeIterator(kv_iterator_t *pIterator,
                                                  kv_collection_t const *pCollection);



/** Advance the iterator to the next object in the collection.

   @param pIterator A pointer to the iterator.
   @return Address of the object or NULL at the end of the collection.
 */
MISCLIB_EXTERN kv_object_t *kv_iterateNext(kv_iterator_t *pIterator);



/** Creates and initializes an empty Key-Value collection.

   @note The collection must be free()ed by calling kv_freeCollection()
      if the memory used is to be reclaimed.
   @return A pointer to the new collection of NULL if an error occurred.
 */
MISCLIB_EXTERN kv_collection_t *kv_createCollection(void);



/** Removes all entries from the Key-Value collection and releases the memory.

   @note The collection must be free()ed by calling kv_freeCollection()
      if the memory used is to be reclaimed.
 */
MISCLIB_EXTERN void kv_clearCollection(kv_collection_t *pCollection);



/** Frees the memory occupied by the entire collection.

   @param pCollection The collection to free.
 */
MISCLIB_EXTERN void kv_freeCollection(kv_collection_t *pCollection);



/** Creates a new copy for the given key.

   @param pKey A string to use as the object key. A copy will be created.
   @return The new object with the copy of the key. NULL if the object
       could not be created.
 */
MISCLIB_EXTERN kv_object_t *kv_createObject(kv_key_t pKey);



/** Frees the memory occupied by the given object.

   If the value if of the type KV_VALUE_STRING, the memory occupied by the
   string will also be free()ed.

   @note The linked list will not be updated to remove the object that is
   free()ed. The caller must handle this prior to calling this function!

   @param pObject The object to free.
 */
MISCLIB_EXTERN void kv_freeObject(kv_object_t *pObject);



/** Find the object with the given key in the collection.

   @param pCollection The collection to search.
   @param pKey The key to find.
   @return A pointer to the object or NULL if it was not found.
 */
MISCLIB_EXTERN kv_object_t *kv_findObjectForKey(kv_collection_t const *pCollection, kv_key_t pKey);


/** Adds a single object to the collection.

   @param pCollection The collection to add the object to.
   @param pObject The object to add to the collection.

   @pre pCollection != NULL
   @pre pObject != NULL
   @pre pObject->next == NULL
*/
MISCLIB_EXTERN void kv_addObjectToCollection(kv_collection_t *pCollection, kv_object_t *pObject);



/** Removes the object with the given key from the collection.

   @param pCollection The collection to remove the object from.
   @param pKey The key of the object to remove.
   @return Was an object found and removed?
   @retval true The object was found and removed from the collection.
   @retval false The object was not removed.
 */
MISCLIB_EXTERN bool kv_remove(kv_collection_t *pCollection, kv_key_t pKey);



/** Returns the type of value stored in the object.

   @param pObject The object to query.
   @return The value type stored in the object.
 */
MISCLIB_EXTERN kv_value_type_t kv_getTypeFromObject(kv_object_t const *pObject);



/** Returns the boolean value stored in the object.

   @param pObject The object to get the value from.
   @return The value stored in the object.
   @pre The object must contain the right type of value.
 */
MISCLIB_EXTERN bool kv_getBoolValueFromObject(kv_object_t const *pObject);



/** Returns the integer value stored in the object.

   @param pObject The object to get the value from.
   @return The value stored in the object.
   @pre The object must contain the right type of value.
 */
MISCLIB_EXTERN int kv_getIntValueFromObject(kv_object_t const *pObject);



/** Returns the floating-point value stored in the object.

   @param pObject The object to get the value from.
   @return The value stored in the object.
   @pre The object must contain the right type of value.
 */
MISCLIB_EXTERN double kv_getFloatValueFromObject(kv_object_t const *pObject);



/** Returns the pointer value stored in the object.

@param pObject The object to get the value from.
@return The value stored in the object.
@pre The object must contain the right type of value.
*/
MISCLIB_EXTERN void *kv_getPointerValueFromObject(kv_object_t const *pObject);



/** Returns the string value stored in the object.

   @param pObject The object to get the value from.
   @return The value stored in the object.
   @pre The object must contain the right type of value.
 */
MISCLIB_EXTERN char const *kv_getStringValueFromObject(kv_object_t const *pObject);



/** Stores a boolean value in the object with the given key in the collection.

   If an object with the specified value exists, the value will be overwritten.

   @param pCollection The collection that will contain the object.
   @param pKey The key identifying the object containing the value.
   @param value The value to store with the key.
   @return A pointer to the object containing the value.
*/
MISCLIB_EXTERN kv_object_t *kv_insertBool(kv_collection_t *pCollection, kv_key_t pKey, bool value);



/** Stores an integer value in the object with the given key in the collection.

   If an object with the specified value exists, the value will be overwritten.

   @param pCollection The collection that will contain the object.
   @param pKey The key identifying the object containing the value.
   @param value The value to store with the key.
   @return A pointer to the object containing the value.
*/
MISCLIB_EXTERN kv_object_t *kv_insertInt(kv_collection_t *pCollection, kv_key_t pKey, int value);



/** Stores a floating-point value in the object with the given key in the collection.

   If an object with the specified value exists, the value will be overwritten.

   @param pCollection The collection that will contain the object.
   @param pKey The key identifying the object containing the value.
   @param value The value to store with the key.
   @return A pointer to the object containing the value.
*/
MISCLIB_EXTERN kv_object_t *kv_insertFloat(kv_collection_t *pCollection, kv_key_t pKey, double value);



/** Stores a pointrt value in the object with the given key in the collection.

    If an object with the specified value exists, the value will be overwritten.

    @param pCollection The collection that will contain the object.
    @param pKey The key identifying the object containing the value.
    @param value The value to store with the key.
    @return A pointer to the object containing the value.
 */
MISCLIB_EXTERN kv_object_t *kv_insertPointer(kv_collection_t *pCollection, kv_key_t pKey, void const *value);



/** Stores a string value in the object with the given key in the collection.

   If an object with the specified value exists, the value will be overwritten.

   @note A copy of the string is stored using #strdup() from the C standard
   library. The copy of the string is automatically #free()ed when it is
   either replaced or the object in the collection is deleted (using
   #kv_freeObject).

   @param pCollection The collection that will contain the object.
   @param pKey The key identifying the object containing the value.
   @param value The value to store with the key.
   @return A pointer to the object containing the value.
*/
MISCLIB_EXTERN kv_object_t *kv_insertString(kv_collection_t *pCollection, kv_key_t const pKey, char const *value);



/** Returns the boolean value stored with the given key in the collection.

   If the key does not exist, False is returned. If you want to know if the key exists, use
   kv_findObjectForKey.

   @param pCollection The collection to search the for the key.
   @param pKey The key identifying the object containing the value.
   @return The value stored with the key.
*/
MISCLIB_EXTERN bool kv_getBool(kv_collection_t const *pCollection, kv_key_t const pKey);



/** Returns the integer value stored with the given key in the collection.

   If the key does not exist, 0 is returned. If you want to know if the key exists, use
   kv_findObjectForKey.

   @param pCollection The collection to search the for the key.
   @param pKey The key identifying the object containing the value.
   @return The value stored with the key.
*/
MISCLIB_EXTERN int kv_getInt(kv_collection_t const *pCollection, kv_key_t const pKey);



/** Returns the floating-point value stored with the given key in the collection.

   If the key does not exist, 0.0 is returned. If you want to know if the key exists, use
   kv_findObjectForKey.

   @param pCollection The collection to search the for the key.
   @param pKey The key identifying the object containing the value.
   @return The value stored with the key.
*/
MISCLIB_EXTERN double kv_getFloat(kv_collection_t const *pCollection, kv_key_t const pKey);



/** Returns the pointer value stored with the given key in the collection.

    If the key does not exist, NULL is returned. If you want to know if the key exists, use
    kv_findObjectForKey.

    @param pCollection The collection to search the for the key.
    @param pKey The key identifying the object containing the value.
    @return The value stored with the key.
    @retval NULL A NULL pointer was stored or the key does not exist.
 */
MISCLIB_EXTERN void *kv_getPointer(kv_collection_t const *pCollection, kv_key_t const pKey);



/** Returns the string value stored with the given key in the collection.

   If the key does not exist, NULL is returned. If you want to know if the key exists, use
   kv_findObjectForKey.

   @param pCollection The collection to search the for the key.
   @param pKey The key identifying the object containing the value.
   @return The value stored with the key.
*/
MISCLIB_EXTERN char const *kv_getString(kv_collection_t const *pCollection, kv_key_t const pKey);


#endif // KEYKV_VALUE_H
