/** Logging facilities.


    @file logging.c
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


#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <errno.h>
#else
#include <sys/errno.h>
#endif // !_WIN32
#include "logging.h"



/** The file handle of the logfile to use or NULL if no file is currently in use. */
static FILE *logFileHandle = NULL;
/** All message of the specified or higher level will be output to the log
 * file (if open).
 */
static log_level_t logLevelFile = LOGLEVEL_INFO;
/** All message of the specified or higher level will be output to stderr. */
static log_level_t logLevelStderr = LOGLEVEL_ERROR;
/** All message of the specified or higher level will be output to stdout. */
static log_level_t logLevelStdout = LOGLEVEL_WARNING;
/** If true, messages sent to stderr will be suppressed on stdout. */
static bool logSuppressStdout = true;



bool log_openLogfile(char const *filename, bool append) {
    if (NULL != logFileHandle) {
        if (log_closeLogfile()) {
            return true;
        }
    }

    if (append) {
        logFileHandle = fopen(filename, "wa");
    } else {
        logFileHandle = fopen(filename, "w");
    }

    if (NULL == logFileHandle) {
        fprintf(stderr, "ERROR: Unable to open logfile '%s' for write: %s\n", filename, strerror(errno));
        return true;
    }

    return false;
} // log_openLogfile()



bool log_closeLogfile(void) {
    assert(NULL != logFileHandle);

    if (fclose(logFileHandle) != 0) {
        logFileHandle = NULL;
        log_logMessage(LOGLEVEL_ERROR, 
                       "Unable to close logfile: %s",
                       strerror(errno));
        return true;
    }
    logFileHandle = NULL;
    return false;
} // log_closeLogfile()



void log_setFileLevel(log_level_t level) {
    assert((level >= LOGLEVEL_NONE) && (level < LOGLEVEL_ALWAYS));

    logLevelFile = level;
} // log_setFileLevel()



void log_setStderrLevel(log_level_t level) {
    assert((level >= LOGLEVEL_NONE) && (level < LOGLEVEL_ALWAYS));

    logLevelStderr = level;
} // log_setStderrLevel()



void log_setStdoutLevel(log_level_t level) {
    assert((level >= LOGLEVEL_NONE) && (level < LOGLEVEL_ALWAYS));

    logLevelStdout = level;
} // log_setStdoutLevel()



void log_setStdoutSupression(bool suppress) {
    logSuppressStdout = suppress;
} // log_setStdoutSupression()



void log_logVMessageContinue(log_level_t level, char const *format, va_list arglist) {
    assert((level > LOGLEVEL_NONE) && (level <= LOGLEVEL_ALWAYS));
    assert(NULL != format);

    if (logLevelStderr <= level) {
        (void) vfprintf(stderr, format, arglist);
    }
    if (logLevelStdout <= level) {
        // If suppression is active, log only what has not been output to stderr.
        if (!logSuppressStdout || (logLevelStderr > level)) {
            (void) vprintf(format, arglist);
        }
    }
    if ((logLevelFile <= level) && (NULL != logFileHandle)) {
        (void) vfprintf(logFileHandle, format, arglist);
        (void) fflush(logFileHandle);
    }
} // log_logVMessageContinue()



void log_logMessageContinue(log_level_t level, char const *format, ...) {
    //lint --e{438} args is changed by the macros.
    va_list args;

    //lint -e{530} va_start initializes 'args'
    va_start(args, format);
    log_logVMessageContinue(level, format, args);
    va_end(args);
} // log_logMessageContinue()



void log_logVMessageStart(log_level_t level, char const *format, va_list arglist) {
    char const *prepend_string;


    assert((level > LOGLEVEL_NONE) && (level <= LOGLEVEL_ALWAYS));
    assert(NULL != format);

    // Note: this could be made more efficient using table-lookup in a 
    // string table. The switch/case has the advantage of causing a run-time
    // error if the strings and log_level_t get out of sync.
    switch (level) {
        case LOGLEVEL_DEBUG3:
            prepend_string = "DEBUG3: ";
            break;
        case LOGLEVEL_DEBUG2:
            prepend_string = "DEBUG2: ";
            break;
        case LOGLEVEL_DEBUG1:
            prepend_string = "DEBUG1: ";
            break;
        case LOGLEVEL_DEBUG:
            prepend_string = "DEBUG: ";
            break;
        case LOGLEVEL_INFO:
            prepend_string = "INFO: ";
            break;
        case LOGLEVEL_WARNING:
            prepend_string = "WARNING: ";
            break;
        case LOGLEVEL_ERROR:
            prepend_string = "ERROR: ";
            break;
        case LOGLEVEL_FATAL:
            prepend_string = "FATAL ERROR: ";
            break;
        case LOGLEVEL_ALWAYS:
            prepend_string = "ALWAYS: ";
            break;
        case LOGLEVEL_NONE:
        default:
            //-e{224,225} Constant value boolean used intentionally.
            assert(false);
    } // switch (level)
    log_logMessageContinue(level, prepend_string);

    log_logVMessageContinue(level, format, arglist);
} // log_logVMessageStart()



void log_logMessageStart(log_level_t level, char const *format, ...) {
    //lint --e{438} args is changed by the macros.
    va_list args;

    //lint -e{530} va_start initializes 'args'
    va_start(args, format);
    log_logVMessageStart(level, format, args);
    va_end(args);
} // log_logMessageStart()



void log_logMessage(log_level_t level, char const *format, ...) {
    //lint --e{438} args is changed by the macros.
    va_list args;

    va_start(args, format);
    log_logVMessageStart(level, format, args);
    va_end(args);
    log_logMessageContinue(level, "\n");
} // log_logMessage()

