/** Efficient generic ringbuffer implementation.



    @file ringbuffer.h
    @ingroup misclib

    @author Christian D&ouml;nges <cd@platypus-projects.de>

    @note The master repository for this file is at
     <a href="https://github.com/cdoenges/misclib">https://github.com/cdoenges/misclib</a>


    LICENSE

    This software is open source software under the "Simplified BSD License"
    as approved by the Open Source Initiative (OSI)
    <http://opensource.org/licenses/bsd-license.php>:


    Copyright (c) 2011-2014, Christian Doenges (Christian D&ouml;nges) All rights
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

#ifndef RINGBUFFER_H
#define RINGBUFFER_H


/** The status of the ringbuffer. */
typedef enum {
    /** The ringbuffer is empty and can not be read from. */
    ring_empty = -1,
    /** The ringbuffer is available for reading and writing. */
    ring_ok = 0,
    /** The ringbuffer is full and can not be written to. */
    ring_full = 1
} ringbuffer_status_t;


typedef struct {
    /** The starting location of the ringbuffer in memory. */
    unsigned char *pBuffer;
    /** The size of the ringbuffer.

       Must be a power of 2 *and* it must fit into headOffset/tailOffset. 
       So if 'unsigned' is 16 bits on your system, the size must be 
       [2, 4, 8, .., 16384, 32768].
     */
    size_t size;
    /** The offset of the head, i.e. the location where the next byte placed
       in the buffer will be written to.

       Note that this value must be used modulo ('%') the buffer size to deal
       with wrapping.
    */
    unsigned headOffset;
    /** The offset of the tail, i.e. the location where the next byte will
       be read from.

       Note that this value must be used modulo ('%') the buffer size to deal
       with wrapping.
    */
    unsigned tailOffset;
} ringbuffer_t;


/** Number of bytes in the ringbuffer. */
#define ringbuffer_length(pRB) ((pRB)->headOffset - (pRB)->tailOffset)

/** Initializes the ringbuffer.

   @note The members pBuffer and size MUST be set correctly.

   @param pRB Pointer to the ringbuffer description.
 */
extern void ringbuffer_init(ringbuffer_t *pRB);


/** Places an entry in the ringbuffer.

   @param pRB Pointer to the ringbuffer description.
   @param newEntry the etry to place in the buffer.
   @return The status of the ringbuffer operation.
   @retval ring_ok The entry was added to the ringbuffer.
   @retval ring_full The ringbuffer is full and the entry was not added.
 */
extern ringbuffer_status_t ringbuffer_put(ringbuffer_t *pRB, unsigned char newEntry);



/** Removes an entry from the ringbuffer.

   @param pRB Pointer to the ringbuffer description.
   @return The oldest entry in the ringbuffer as a positive integer, or a
        negative integer if the buffer is empty.
   @retval ring_empty The ringbuffer is empty.
 */
extern int ringbuffer_get(ringbuffer_t *pRB);


/** Returns the next entry from the ringbuffer without removing it.

   @param pRB Pointer to the ringbuffer description.
   @return The oldest entry in the ringbuffer as a positive integer, or a
        negative integer if the buffer is empty.
   @retval ring_empty The ringbuffer is empty.
 */
extern int ringbuffer_peek(ringbuffer_t *pRB);


#endif // RINGBUFFER_H
