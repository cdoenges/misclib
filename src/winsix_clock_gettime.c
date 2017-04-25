/** A MS Windows replacement for the POSIX clock_gettime() function.

   @file winsix_clock_gettime.c
   @ingroup misclib

   @author Christian D&ouml;nges <cd@platypus-projects.de>

   @note The master repository for this file is at
    <a href="https://github.com/cdoenges/misclib">https://github.com/cdoenges/misclib</a>


    LICENSE

    Copyright 2017 Christian Doenges (Christian D&ouml;nges)

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

#include <assert.h>
#include <errno.h>
#include <time.h>
#include <windows.h>



/** Get the current time and store it in the provided timespec.

    @param option ignored.
    @param[out] pTS Pointer to the user-provided timespec.
    @retval 0 The timespec structure contains the current POSIX time.
    @retval -1 An error occurred and the timespec structure was not modified.

    @pre NULL != pTS
 */
int clock_gettime(int option, struct timespec *pTS) {
    static double ticks2nano;               // number of ticks per nanosecond
    static LARGE_INTEGER pc_start_ticks;    // starting value of the performance counter
    static LARGE_INTEGER pc_ticks_per_second = 0;   // performance counter ticks per second
    static struct timespec ts_start;        // Unix start time.
    LARGE_INTEGER pc_current_ticks;         // Current performance counter ticks.
    LARGE_INTEGER pc_delta_ticks;           // Elapsed performance counter ticks.


    assert(NULL != pTS);

    if (0 == pc_ticks_per_second) {
        LARGE_INTEGER win_time;


        // Get the frequency of the performance counter.
        if (0 == QueryPerformanceFrequency((LARGE_INTEGER *) &pc_ticks_per_second)) {
            goto fail;
        }
        // Get the current value of the performance counter.
        if (0 == QueryPerformanceCounter((LARGE_INTEGER *) &startticks)) {
            goto fail;
        }

        // Get the starting time in timespec format.
        GetSystemTimeAsFileTime((FILETIME *) &win_time);
        win_time -= 116444736000000000ULL;                // Convert epoch 1601-01-01 to 1970-01-01.
        ts_start->tv_sec = (win_time / 10000000UL);       // Convert 100 ns to seconds.
        ts_start->tv_nsec = win_time % 10000000UL * 100;  // Convert remaining 100 ns to ns.
        ticks2nano = 1000000000.0 / pc_ticks_per_second;
    }

    // Get the current value of the performance counter.
    if (0 == QueryPerformanceCounter((LARGE_INTEGER *) &pc_current_ticks)) {
        goto fail;
    }
    pc_delta_ticks = pc_current_ticks - pc_start_ticks;
    pTS->tv_sec = ts_start.tv_sec + (pc_delta_ticks / pc_ticks_per_second);
    pTS->tv_nsec = (long) (ts_start.tv_nsec + (double)(pc_delta_ticks % pc_ticks_per_second) * ticks2nano);

    if (pTS->tv_nsec >= 1000000000UL) {
        // Adjust for nanosecond overflow.
        pTS->tv_sec ++;
        pTS->tv_nsec -= 1000000000UL;
    }

    return 0;

fail:
    // There is no errno value corresponding to this WinAPI failure, so
    // report that the operation is not supported.
    errno = ENOSYS;
    return -1;
} // clock_gettime()
