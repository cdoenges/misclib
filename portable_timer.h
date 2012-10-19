/** A portable timer routine that yields microsecond resolution.
 
    Usage:
       - Declare an instance of portable_timer_t.
       - Start the timer using pt_start().
       - Stop the timer using pt_stop().
       - You can read the timer using pt_elapsed_us() or pt_elapsed_ms()
         regadless of whether the timer is running or not.



    Copyright (c) 2010 by Christian Doenges <cd@doenges.com>

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
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

