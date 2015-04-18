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

#ifndef LOGGING_H
#define LOGGING_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

// If you are in a cross-platform environment where one or more of the
// compilers does not support variadic macros, define the preprocessor symbol
// LOGGING_API_USES_VARIADIC_MACROS to be zero. This is usually a command-line
// option such as:
// -DLOGGING_API_USES_VARIADIC_MACROS=0
// If you are in an environment where all compilers support the feature, you
// should set LOGGING_API_USES_VARIADIC_MACROS to one.
// If you are unsure, you can try to auto-detect.
#ifndef LOGGING_API_USES_VARIADIC_MACROS
#if defined(__C51__) || defined(__C251__)
    // These compilers do not support variadic macros.
    // You should add any compiler that does not offer variadic macros.
    #define LOGGING_API_USES_VARIADIC_MACROS 0
#elif __STDC_VERSION__ >= 199901L
    // All modern compilers should support variadic macros.
    #define LOGGING_API_USES_VARIADIC_MACROS 1
#else
    // Unknown compiler behavior. We'll try the feature and if it does not work,
    // add the compiler to the list of compilers where this feature is disabled.
    #define LOGGING_API_USES_VARIADIC_MACROS 1
#endif // unkown compiler
#endif // !LOGGING_API_USES_VARIADIC_MACROS


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
 * @Note
 * Some embedded C compilers still do not support variadic macros, so we have
 * to trick these compilers by passing all arguments as a single argument
 * (using parenthesis) and expanding through the preprocessor.
 *
 * <B>Example</B>:
 * <pre>
 * log_logMessage((LOGLEVEL_WARNING, "Some compilers are %s", "stupid"));
 * </pre>
 *
 * @param level The level of the message.
 * @param format A format string as used by @see printf
 * @param ... A list of parameters as used by @see printf
 */
#if 0 == LOGGING_API_USES_VARIADIC_MACROS
    #ifdef FTR_EMBEDDED
        #define log_logMessage(log)
    #else
        #define log_logMessage(log) log_logMessage_impl log
        extern void log_logMessage_impl(log_level_t level, char const *format, ...);
    #endif // !FTR_EMBEDDED
#elif 1 == LOGGING_API_USES_VARIADIC_MACROS
    #ifdef FTR_EMBEDDED
        #define log_logMessage(level, format, ...)
    #else
        #define log_logMessage log_logMessage_impl
        extern void log_logMessage_impl(log_level_t level, char const *format, ...);
    #endif // !FTR_EMBEDDED
#else
    #error Illegal value for LOGGING_API_USES_VARIADIC_MACROS
#endif // LOGGING_API_USES_VARIADIC_MACROS



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
#if 0 == LOGGING_API_USES_VARIADIC_MACROS
    #ifdef FTR_EMBEDDED
        #define log_logMessageStart(log)
    #else
        #define log_logMessageStart(log) log_logMessageStart_impl log
        extern void log_logMessageStart_impl(log_level_t level, char const *format, ...);
    #endif // !FTR_EMBEDDED
#elif 1 == LOGGING_API_USES_VARIADIC_MACROS
    #ifdef FTR_EMBEDDED
        #define log_logMessageStart(level, format, ...)
    #else
        #define log_logMessageStart log_logMessageStart_impl
        extern void log_logMessageStart_impl(log_level_t level, char const *format, ...);
    #endif // !FTR_EMBEDDED
#else
    #error Illegal value for LOGGING_API_USES_VARIADIC_MACROS
#endif // LOGGING_API_USES_VARIADIC_MACROS



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
#if 0 == LOGGING_API_USES_VARIADIC_MACROS
    #ifdef FTR_EMBEDDED
        #define log_logMessageContinue(log)
    #else
        #define log_logMessageContinue(log) log_logMessageContinue_impl log
        extern void log_logMessageContinue_impl(log_level_t level, char const *format, ...);
    #endif // !FTR_EMBEDDED
#elif 1 == LOGGING_API_USES_VARIADIC_MACROS
    #ifdef FTR_EMBEDDED
        #define log_logMessageContinue(level, format, ...)
    #else
        #define log_logMessageContinue log_logMessageContinue_impl
        extern void log_logMessageContinue_impl(log_level_t level, char const *format, ...);
    #endif // !FTR_EMBEDDED
#else
    #error Illegal value for LOGGING_API_USES_VARIADIC_MACROS
#endif // LOGGING_API_USES_VARIADIC_MACROS



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
#endif // LOGGING_H
