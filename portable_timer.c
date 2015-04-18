/** Portable (between MS Windows and Unix) timer routine with microsecond resolution.


    Compile with the symbol <tt>SELFTEST</tt> and run to perform basic tests.

    <code>gcc -DSELFTEST -o pt portable_timer.c</code>


    @file portable_timer.c
    @ingroup misclib

    @author Christian D&ouml;nges <cd@platypus-projects.de>

    @note The master repository for this file is at
     <a href="https://github.com/cdoenges/misclib">https://github.com/cdoenges/misclib</a>


    LICENSE

    This software is open source software under the "Simplified BSD License"
    as approved by the Open Source Initiative (OSI)
    <http://opensource.org/licenses/bsd-license.php>:


    Copyright (c) 2010, 2015 Christian Doenges (Christian D&ouml;nges) All rights
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
#include <stdio.h>
#include <string.h>
#include <time.h>

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
    // Ignore the error, if any.
    (void) gettimeofday(&pt->startTime, NULL);
#endif // !WIN32
} // end pt_start()



void pt_stop(portable_timer_t *pt) {
    assert(NULL != pt);
    assert(pt->running);

    // Stop the timer and mark the stop time.
#ifdef WIN32
    QueryPerformanceCounter(&pt->endTime);
#else
    // Ignore the error, if any.
    (void) gettimeofday(&pt->endTime, NULL);
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
        // Ignore the error, if any.
        (void) gettimeofday(&pt->endTime, NULL);
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
#ifdef WIN32
    Sleep(milliseconds);
#else // requires _POSIX_C_SOURCE >= 199309L
    time_t seconds = (time_t) (milliseconds / 1000);
    struct timespec sleeptime = {
        seconds,
        (long) ((milliseconds - (unsigned long) seconds*1000) * 1000000UL) };
    if (nanosleep(&sleeptime, NULL) != 0) {
        // TODO Do something intelligent here.
    }
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
