/** A portable (between MS Windows and Unix) timer routine that yields microsecond resolution.
 
    Usage:
       - Declare an instance of portable_timer_t.
       - Start the timer using pt_start().
       - Stop the timer using pt_stop().
       - You can read the timer using pt_elapsed_us() or pt_elapsed_ms()
         regadless of whether the timer is running or not.


    @file portable_timer.h
    @ingroup misclib

    @author Christian D&ouml;nges <cd@platypus-projects.de>

    @note The master repository for this file is at
     <a href="https://github.com/cdoenges/misclib">https://github.com/cdoenges/misclib</a>


    LICENSE

    This software is open source software under the "Simplified BSD License"
    as approved by the Open Source Initiative (OSI)
    <http://opensource.org/licenses/bsd-license.php>:


    Copyright (c) 2010, Christian Doenges (Christian D&ouml;nges) All rights
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

#ifndef PORTABLE_TIME_H
#define PORTABLE_TIME_H

#include <stdbool.h>

#ifdef WIN32    // Code for Microsoft Windows
#include <windows.h>
#else           // Code for Unix systems.
#include <sys/time.h>
#endif


/** Portable timer data structure.
 *
 * @note This is a private definition, do not look inside!
 */
typedef struct {
    /** True if the timer is running, false if stopped. */
    bool running;    
#ifdef WIN32
    LARGE_INTEGER frequency;      // Counter frequency in ticks per second.
    LARGE_INTEGER startTime;      // Counter when timer was started.
    LARGE_INTEGER endTime;        // Counter when timer was stopped.
#else
    struct timeval startTime;            // Counter when timer was started.
    struct timeval endTime;              // Counter when timer was stopped. 
#endif
} portable_timer_t;



/** Starts the given portable timer and stores the start time.
 *
 * @param pt Portable timer instance.
 */
void pt_start(portable_timer_t *pt);


/** Stops the given portable timer and stores the stop time.
 *
 * @param pt Portable timer instance.
 */
void pt_stop(portable_timer_t *pt);


/** Checks if the timer is running.
 *
 * @param pt Portable timer instance.
 * @return Is the timer running?
 * @retval true The timer is currently running.
 * @retval false The timer is stopped.
 */
bool pt_is_running(const portable_timer_t *pt);


/** Returns the number of milliseconds the timer has been running.
 *
 * @param pt Portable timer instance.
 * @return The number of milliseconds elapsed since pt_start() was called.
 */
unsigned long pt_elapsed_ms(portable_timer_t *pt);


/** Returns the number of microseconds the timer has been running.
 *
 * @param pt Portable timer instance.
 * @return The number of microseconds elapsed since pt_start() was called.
 */
unsigned long pt_elapsed_us(portable_timer_t *pt);


/** Waits for the specified number of milliseconds before proceeding.
 *
 * @param milliseconds The number of milliseconds to wait.
 *
 * @note If possible, the function will sleep (i.e. cede the CPU to another
 * process) instead of idling.
 */
void pt_sleep_ms(unsigned long milliseconds);

#endif // PORTABLE_TIME_H

