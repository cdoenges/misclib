/** Logging facilities.

    A simple model is used for logging: there is a finite number of possible
    output channels, each of which uses a filter to limit what messages are
    reproduced by the channel.
    
    Each message carries a level of importance from very low (DEBUG) to high
    (FATAL). If the level of the message is equal to or higher than the filter
    level, the channel reproduces the message. Otherwise the message is
    suppressed.


    @file logging.h
    @ingroup misclib

    @author Christian D&ouml;nges <cd@platypus-projects.de>

    @note The master repository for this file is at
     <a href="https://github.com/cdoenges/misclib">https://github.com/cdoenges/misclib</a>


    LICENSE
    
    This software is open source software under the "Simplified BSD License"
    as approved by the Open Source Initiative (OSI)
    <http://opensource.org/licenses/bsd-license.php>:


    Copyright (c) 2010-2014, Christian Doenges (Christian D&ouml;nges) All rights
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

#ifndef LOGGING_H
#define LOGGING_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

// Suppress warnings about these symbols not being used.
//lint -esym(714, log_openLogfile, log_closeLogfile, log_setFileLevel, log_setStderrLevel, log_setStdoutLevel, log_setStdoutSupression)
//lint -esym(714, log_logMessageContinue, log_logMessageStart, log_logVMessageContinue, log_logVMessageStart)
//lint -esym(759, log_openLogfile, log_closeLogfile, log_setFileLevel, log_setStderrLevel, log_setStdoutLevel, log_setStdoutSupression)
//lint -esym(759, log_logMessageContinue, log_logMessageStart, log_logVMessageContinue, log_logVMessageStart)


/** Message classification levels for logging. */
typedef enum {
    /** All messages will be ignored if this value is used for a filter.
     * Use of this level in a message is a bug.
     */
    LOGLEVEL_NONE = 0,
    /** Message is for debug purposes. It usually contains information
     * useful only to developers with access to the source code.
     */
    LOGLEVEL_DEBUG3,
    /** Informational message that may be displayed to the user if the most
     * verbose program behavior is desired.
     */
    LOGLEVEL_DEBUG2,
    /** Informational message that may be displayed to the user if 
     * extremely verbose program behavior is desired.
     */
    LOGLEVEL_DEBUG1,
    /** Informational message that may be displayed to the user if very
     * verbose program behavior is desired.
     */
    LOGLEVEL_DEBUG,
    /** Informational message that may be displayed to the user if verbose
     * program behavior is desired.
     */
    LOGLEVEL_INFO,
    /** Warning message showing that some behavior is not as expected.
     * Warnings are used if it is likely (but not guaranteed) that ignoring
     * the condition leading to the message will not prevent the program
     * from doing something sensible.
     */
    LOGLEVEL_WARNING,
    /** Error message informing the user that something went seriously
     * wrong. An error is generated if it is likely (but not guaranteed)
     * that program execution will not lead to a sensible result.
     */
    LOGLEVEL_ERROR,
    /** Error message informing the user that the program will terminate
     * because it can not recover from the error condition.
     */
     LOGLEVEL_FATAL,
     /** A message of this level will never be ignored. Use of this level
      * as a filter setting is an error.
      */
     LOGLEVEL_ALWAYS
} log_level_t;




/** Opens the specified log file for writing.
 *
 * If a logfile is currently open, it will be closed before opening the
 * specified file.
 *
 * @param filename The name (with path) of the file to open.
 * @param append true if the logfile shall be appended to, false if the
 *        previous content will be deleted before opening the file.
 * @return Did an error occur?
 * @retval false No error occurred.
 * @retval true And error occurred and the file could not be opened. Check errno for details.
 */
extern bool log_openLogfile(char const *filename, bool append);



/** Closes the log file currently in use.
 *
 * @note A logfile must be open when this function is called.
 *
 * @return Did an error occur?
 * @retval false No error occurred.
 * @retval true And error occurred and the file could not be closed. 
 *    Check errno for details. The file handle will be rendered unusable,
 *    so log_openLogfile() must be called to log to a file again.
 */
extern bool log_closeLogfile(void);



/** Sets the minimum message level logged to the log file (if open),
 * lower messages are ignored.
 *
 * The default is LOGLEVEL_INFO.
 *
 * @param level The minimum level to log to the log file.
 */
extern void log_setFileLevel(log_level_t level);



/** Sets the minimum message level logged to the standard error output,
 * lower messages are ignored.
 *
 * The default is LOGLEVEL_ERROR.
 *
 * @param level The minimum level to log to the standard output.
 */
extern void log_setStderrLevel(log_level_t level);



/** Sets the minimum message level logged to the standard output, lower
 * messages are ignored.
 *
 * The default is LOGLEVEL_WARNING.
 *
 * @param level The minimum level to log to the standard output.
 */
extern void log_setStdoutLevel(log_level_t level);



/** Sets suppression of messages sent to stderr on stdout.
 *
 * The default is to suppress messages on stdout that have been output
 * to stderr.
 *
 * @param suppress If true, messages output to stderr are not output to
 * stdout. Otherwise, only the log level on stdout decides if messages
 * will be output.
 */
extern void log_setStdoutSupression(bool suppress);



/** Logs the given message to whatever channels accept the specified log
 * level.
 *
 * The message is terminated with an EOL (End-Of-Line) marker (i.e. LF or CR+LF).
 *
 * @param level The level of the message.
 * @param format A format string as used by @see printf
 * @param ... A list of parameters as used by @see printf
 */
#ifdef FTR_EMBEDDED
#ifdef __C251__
// C251 5.56 (and earlier):
// Variadic macros—macros which allow variable length argument lists—are not
// supported in this release. 
#define log_logMessage()
#else
#define log_logMessage(l, f, ...)
#endif // !__C251__
#else
extern void log_logMessage(log_level_t level, char const *format, ...);
#endif // !FTR_EMBEDDED



/** Logs the start of a message to whatever channels accept the specified
 * log level.
 *
 * The message is not terminated with an EOL (End-Of-Line) marker
 * (i.e. LF or CR+LF). You should call #log_logMessageContinue as many times
 * as necessary with the final call containing the EOL marker.
 *
 * @param level The level of the message.
 * @param format A format string as used by @see printf
 * @param ... A list of parameters as used by @see printf
 */
#ifdef FTR_EMBEDDED
#ifdef __C251__
// C251 5.56 (and earlier):
// Variadic macros—macros which allow variable length argument lists—are not
// supported in this release. 
#define log_logMessageStart()
#else
#define log_logMessageStart(l, f, ...)
#endif // !__C251__
#else
extern void log_logMessageStart(log_level_t level, char const *format, ...);
#endif // !FTR_EMBEDDED



/** Logs the start of a message to whatever channels accept the specified
 * log level.
 *
 * The message is not terminated with an EOL (End-Of-Line) marker
 * (i.e. LF or CR+LF). You should call #log_logMessageContinue as many times
 * as necessary with the final call containing the EOL marker.
 *
 * @param level The level of the message.
 * @param format A format string as used by @see printf
 * @param arglist A list of parameters as used by @see vprintf
 */
#ifdef FTR_EMBEDDED
#define log_logVMessageStart(l, f, a)
#else
extern void log_logVMessageStart(log_level_t level, char const *format, va_list arglist);
#endif // !FTR_EMBEDDED



/** Continues logging the given message to whatever channels accept the
 * specified log level.
 *
 * The message is not prepended with the loglevel. You must have called
 * #log_logMessageStart to get the proper header before calling this
 * function.
 *
 * The message is not terminated with an EOL (End-Of-Line) marker
 * (i.e. LF or CR+LF).
 *
 * @param level The level of the message.
 * @param format A format string as used by @see printf
 * @param ... A list of parameters as used by @see printf
 */
#ifdef FTR_EMBEDDED
#ifdef __C251__
// C251 5.56 (and earlier):
// Variadic macros—macros which allow variable length argument lists—are not
// supported in this release. 
#define log_logMessageContinue()
#else
#define log_logMessageContinue(l, f, ...)
#endif // !__C251__
#else
extern void log_logMessageContinue(log_level_t level, char const *format, ...);
#endif // !FTR_EMBEDDED



/** Continues logging the given message to whatever channels accept the
 * specified log level.
 *
 * The message is not prepended with the loglevel. You must have called
 * #log_logMessageStart to get the proper header before calling this
 * function.
 *
 * The message is not terminated with an EOL (End-Of-Line) marker
 * (i.e. LF or CR+LF).
 *
 * @param level The level of the message.
 * @param format A format string as used by @see printf
 * @param arglist A list of parameters as used by @see vprintf
 */
#ifdef FTR_EMBEDDED
#define log_logVMessageContinue(l, f, a)
#else
extern void log_logVMessageContinue(log_level_t level, char const *format, va_list arglist);
#endif // !FTR_EMBEDDED
#endif // LOGGING_H



/** Logs the given data using loglevel LOGLEVEL_DEBUG2.
 *
 * @param level The level of the message.
 * @param pData The data that will be logged.
 * @param nrOfBytes The number of data bytes to log. May be split into
 *      multiple lines of output.
 * @param prefixStr A string of exactly 8 characters that will prefix the
 *      first line of output.
 * @param hexWidth The number of hex bytes to output per line.
 */
extern void log_logData(log_level_t level,
                        char const *pData,
                        size_t nrOfBytes,
                        char const *prefixStr,
                        size_t hexWidth);
