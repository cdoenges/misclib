/** Portable timer routine with microsecond resolution.


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

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "portable_timer.h"


void pt_start(portable_timer_t *pt) {
    assert(NULL != pt);
    
    // Clear the instance to get a clean reading.
    memset(pt, 0, sizeof(portable_timer_t));
    pt->running = true;

    // Start the timer by marking the start time.
#ifdef WIN32
    QueryPerformanceFrequency(&pt->frequency);
    QueryPerformanceCounter(&pt->startTime);
#else
    gettimeofday(&pt->startTime, NULL);
#endif // !WIN32
} // end pt_start()



void pt_stop(portable_timer_t *pt) {
    assert(NULL != pt);
    assert(pt->running);

    // Stop the timer and mark the stop time.
#ifdef WIN32
    QueryPerformanceCounter(&pt->endTime);
#else
    gettimeofday(&pt->endTime, NULL);
#endif // !WIN32
    pt->running = false;
} // end pt_stop()



bool pt_is_running(const portable_timer_t *pt) {
    return pt->running;
} // end pt_is_running()



unsigned long pt_elapsed_ms(portable_timer_t *pt) {
    return pt_elapsed_us(pt) / 1000;
} // end pt_elapsed_ms()



unsigned long pt_elapsed_us(portable_timer_t *pt) {
    double startMicros, endMicros;

    if (pt->running) {
        // If the timer is running, we have to sample it.
#ifdef WIN32
        QueryPerformanceCounter(&pt->endTimer);
#else
        gettimeofday(&pt->endTime, NULL);
#endif // !WIN32
    } // if running

#ifdef WIN32
    startMicros = pt->startTimer.QuadPart * 1000000.0 / pt->frequency.QuadPart;
    endMicros   = pt->endTimer.QuadPart * 1000000.0 / pt->frequency.QuadPart;
#else
    startMicros = pt->startTime.tv_sec * 1000000.0 + pt->startTime.tv_usec;
    endMicros   = pt->endTime.tv_sec * 1000000.0 + pt->endTime.tv_usec;
#endif // !WIN32

    return (unsigned long) (endMicros - startMicros);
} // end pt_elapsed_us()



void pt_sleep_ms(unsigned long milliseconds) {
#if WIN32
    Sleep(milliseconds);
#elif _POSIX_C_SOURCE >= 199309L
    time_t seconds = milliseconds / 1000;
    struct timespec sleeptime = {
        seconds,
        (milliseconds - (seconds*1000)) * 1000000L };
    if (nanosleep(&sleeptime, NULL) != 0) {
        // TODO Do something intelligent here.
    }
#else
#error "More work: portme"
#endif
} // end pt_sleep_ms()



#ifdef SELFTEST
int main(int argc, char *argv[]) {
    portable_timer_t pt;
    int i;


    pt_start(&pt);
    assert(pt_is_running(&pt));
    for (i = 0;i < 5;i ++) {
        int j;
        printf("Timer running for %lu us.\n", pt_elapsed_us(&pt));
        pt_sleep_ms(i * 500);
    } // for i
    
    pt_stop(&pt);
    assert(!pt_is_running(&pt));
    printf("Timer running for %lu us.\n", pt_elapsed_us(&pt));
    assert(pt_elapsed_us(&pt)/1000 == pt_elapsed_ms(&pt));
} // end main()
#endif // SELFTEST
