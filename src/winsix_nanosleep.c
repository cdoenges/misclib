/** A MS Windows replacement for the POSIX nanosleep() function.

   @file winsix_nanosleep.c
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
#ifdef _WIN32
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>
#include <windows.h>


int nanosleep(const struct timespec *rqtp, struct timespec *rmtp) {
    HANDLE timer;
    int64_t ns100;
    LARGE_INTEGER li;
    DWORD rv;


    // Create a timer.
    timer = CreateWaitableTimer(NULL, //default security attributes
                                TRUE, // manual reset timer
                                NULL);// no name
    if (0 == timer) {
        // There is no errno value corresponding to this WinAPI failure, so
        // report that the operation is not supported.
        errno = ENOSYS;
        return -1;
    }

    // Set the timer to the specified time at 100 ns resolution.
    ns100 = rqtp->tv_sec * 10000000LL + rqtp->tv_nsec / 100LL;
    li.QuadPart = -ns100;
    if (0 == SetWaitableTimer(timer, // the timer
                              &li,   // time
                              0,     // period
                              NULL,  // completion routine
                              NULL,  // completion arguments
                              FALSE)) { // restore from suspend
        CloseHandle(timer);
        errno = ENOSYS;
        return -1;
    }

    // Start the timer and wait for it to complete.
    rv = WaitForSingleObject(timer,      // the timer
                             INFINITE);  // wait until timer is done
    switch (rv) {
        case WAIT_OBJECT_0:
            // All is well.
            errno = 0;
            break;

        case WAIT_ABANDONED:
            // POSIX requires us to return the remaining time in rmtp,
            // but I do not see a way to get the remaining time using a
            // Windows API.
            errno = EINTR;
            break;

        case WAIT_TIMEOUT:
            // This should never occurr: the timer did not finish but the
            // INFINITE waiting time has elapsed.
            //-fallthrough
        default:
            // A generic error occurred.
            // Use Windows API function GetLastError() to get more information.
            // Since there is no way to use this in a POSIX-compliant way,
            // ignore the details.
            errno = ENOSYS;
            break;
    } // switch rv

    // Clean up.
    CloseHandle(timer);

    return 0;
} // nanosleep()
#endif //_WIN32
