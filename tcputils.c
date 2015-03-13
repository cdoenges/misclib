/** Simple TCP communication utility functions.


    @file tcputils.c
    @ingroup misclib

    @author Christian D&ouml;nges <cd@platypus-projects.de>

    @note The master repository for this file is at
     <a href="https://github.com/cdoenges/misclib">https://github.com/cdoenges/misclib</a>


    LICENSE

    This software is open source software under the "Simplified BSD License"
    as approved by the Open Source Initiative (OSI)
    <http://opensource.org/licenses/bsd-license.php>:


    Copyright (c) 2011-2015, Christian Doenges (Christian D&ouml;nges) All rights
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



    COMPILING
    In order to compile the module with the test harness, use

    $ gcc -Wall -DTCPTEST -o tcputils tcputils.c itoa.c logging.c hex.c

    or whatever convention your compiler uses.


    PORTING
    When linking on Win32, make sure you link wsock32.lib.

    Compile with: /MT /D "_X86_" /D "_CRT_USE_WINAPI_FAMILY_DESKTOP_APP" /c


 */

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include "itoa.h" // itoa() is present only on Windows.
#else
#ifdef TEST
#ifndef _WINDOWS_
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif // _WINDOWS_
#include <process.h>    /* _beginthread, _endthread */
#endif // TEST
#include <io.h>
// link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")
#endif // _WIN32
#include <signal.h>
#include <sys/types.h>

#include "logging.h"
#include "tcputils.h"


/*
 * Compile-time settings for the module.
 */


/** Define to enable dumpin the communicated TCP data to the log
   (at LOGLEVEL_DEBUG2). If not defined, the code will not be compiled
   in.
 */
#define FTR_TCP_LOG_CONTENT

/** The number of data bytes per line to log. MUST be small enough to fit
   into FTR_TCP_LC_BUFFER_SIZE. */
#define FTR_TCP_LC_LINE_WIDTH 0x10


#ifdef FTR_TCP_LOG_CONTENT
/** Enables content logging if true, disables if false. */
static bool logContentEnabled = true;
#endif // FTR_TCP_LOG_CONTENT


static bool isInitialized = false;

#ifdef _WIN32
/** Windows uses strange "secure" functions. */
#define snprintf(x, y, ...) _snprintf_s(x, y, y, ## __VA_ARGS__)
/** Windows Sleep() is in milliseconds, POSIX sleep() in seconds. */
#define sleep(x) Sleep(x*1000L)
#define USE_CRLF true
#else
// Windows uses closesocket() instead of close() to close a socket.
#define closesocket close
/** Windows uses a proprietary SOCKET definition instead of the POSIX int for file handles. */
typedef int SOCKET;
/** Windows terminates threads explicitly. */
#define _endthreadex(x)
#define USE_CRLF false
#endif // !_WIN32

#ifdef _WIN32
static WSADATA wsaData;


/** Returns a string representation of a Winsock API error code.

   @param errorCode The winsock error code.
   @return A string representation of the error.

   @note The pointer to the string representation is valid only until the
        next call to this function. If you need to preserve the error message,
        copy the string to a buffer you control.
 */
static char const *winsock_strerror(int errorCode) {
    WCHAR wtext[1024];
    static char ctext[1024];
    size_t csize;


    // Convert to a wide string representation.
    //lint -e{64} Lint does not like argument 5.
    (void) FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                  NULL,
                  (DWORD) errorCode,
                  0,
                  (LPWSTR) wtext,
                  sizeof(wtext),
                  NULL);

    // Convert the wide string to a multi-byte string.
    (void) wcstombs_s(&csize, ctext, sizeof(ctext), wtext, sizeof(ctext));

    // Remove CRLF at end of string.
    *(ctext + strlen(ctext) - 2) = '\0';

    return ctext;
} // winsock_error_string()
#endif // _WIN32



/** Logs an error with the message and a description of the cause.

    The message and the description will be separated by a colon ':'.
    The log entry will be terminated.

   @param message A string describing what the application was trying to do.

    Example:
        tcp_log_error("bind() failed");

        might yield:
            bind() failed: address in use.
 */
static void tcp_log_error(char const *message) {
#ifdef _WIN32
    int errorCode = WSAGetLastError();  // Obtain the last WinSock error code.
    int level = LOGLEVEL_ERROR;
    char const *errorStr = "???";

    switch (errorCode) {
        case WSA_INVALID_HANDLE:
            errorStr = "WSA_INVALID_HANDLE";
            break;

        case WSA_NOT_ENOUGH_MEMORY:
            errorStr = "WSA_NOT_ENOUGH_MEMORY";
            break;

        case WSA_INVALID_PARAMETER:
            errorStr = "WSA_INVALID_PARAMETER";
            break;

        case WSA_OPERATION_ABORTED:
            errorStr = "WSA_OPERATION_ABORTED";
            break;

        case WSA_IO_INCOMPLETE:
            errorStr = "WSA_IO_INCOMPLETE";
            break;

        case WSA_IO_PENDING:
            errorStr = "WSA_IO_PENDING";
            break;

        case WSAEINTR:
            errorStr = "WSAEINTR";
            break;

        case WSAEBADF:
            errorStr = "WSAEBADF";
            break;

        case WSAEACCES:
            errorStr = "WSAEACCES";
            break;

        case WSAEFAULT:
            errorStr = "WSAEFAULT";
            break;

        case WSAEINVAL:
            errorStr = "WSAEINVAL";
            break;

        case WSAEMFILE:
            errorStr = "WSAEMFILE";
            break;

        case WSAEWOULDBLOCK:
            level = LOGLEVEL_INFO;
            errorStr = "WSAEWOULDBLOCK";
            break;

        case WSAEINPROGRESS:
            errorStr = "WSAEINPROGRESS ";
            break;

        case WSAEALREADY:
            errorStr = "WSAEALREADY";
            break;

        case WSAENOTSOCK:
            errorStr = "WSAENOTSOCK";
            break;

        case WSAECONNABORTED:
            errorStr = "WSAECONNABORTED";
            break;

        case WSAEDESTADDRREQ:
            errorStr = "WSAEDESTADDRREQ";
            break;

        case WSAEMSGSIZE:
            errorStr = "WSAEMSGSIZE";
            break;

        case WSAEPROTOTYPE:
            errorStr = "WSAEPROTOTYPE";
            break;

        case WSAENOPROTOOPT:
            errorStr = "WSAENOPROTOOPT";
            break;

        case WSAEPROTONOSUPPORT:
            errorStr = "WSAEPROTONOSUPPORT";
            break;

        case WSAESOCKTNOSUPPORT:
            errorStr = "WSAESOCKTNOSUPPORT";
            break;

        case WSAEOPNOTSUPP:
            errorStr = "WSAEOPNOTSUPP";
            break;

        case WSAEPFNOSUPPORT:
            errorStr = "WSAEPFNOSUPPORT";
            break;

        case WSAEADDRINUSE:
            errorStr = "WSAEADDRINUSE";
            break;

        default:
            break;
    } // switch errorCode

    // Windows nicely formats error messages by appending a period.
    log_logMessage((level, "%s: %s(%d) - %s",
                   message, errorStr, errorCode,
                   winsock_strerror(errorCode)));
#else // POSIX
    log_logMessage(LOGLEVEL_ERROR, "%s: %s.",
                   message,
                   strerror(errno));
#endif // POSIX
} // tcp_log_error()



#ifdef FTR_TCP_LOG_CONTENT
void tcp_log_data(char const *pData, size_t nrOfBytes, char const *prefix) {
    if (logContentEnabled) {
        log_logData(LOGLEVEL_DEBUG2, pData, nrOfBytes, prefix, FTR_TCP_LC_LINE_WIDTH);
    }
} // tcp_log_data()
#endif // FTR_TCP_LOG_CONTENT



bool tcp_set_socket_nonblocking(int theSocket) {
#ifndef _WIN32
    int flags;

      if ((flags = fcntl(theSocket, F_GETFL, 0)) != -1) {
         fcntl(theSocket, F_SETFL, flags | O_NONBLOCK);
      } else {
         tcp_log_error("fcntl(O_NONBLOCK) failed");
         return false;
      }
#else
    unsigned long nonblocking = 1;

    if (0 != ioctlsocket(theSocket, FIONBIO, &nonblocking)) {
        tcp_log_error("ioctlsocket(FIONBIO, 1) failed");
        return false;
    }
#endif // _WIN32

    return true;
} // tcp_set_socket_nonblocking()



bool tcp_set_socket_blocking(int theSocket) {
#ifndef _WIN32
    int flags;

      if ((flags = fcntl(theSocket, F_GETFL, 0)) != -1) {
         fcntl(theSocket, F_SETFL, flags & ~O_NONBLOCK);
      } else {
         tcp_log_error("fcntl(~O_NONBLOCK) failed");
         return false;
      }
#else
    unsigned long blocking = 0;

    if (0 != ioctlsocket(theSocket, FIONBIO, &blocking)) {
        tcp_log_error("ioctlsocket(FIONBIO, 0) failed");
        return false;
    }
#endif // _WIN32

    return true;
} // tcp_set_socket_blocking()



int tcp_init(void) {
    if (isInitialized) {
        return 0;
    }

#ifdef _WIN32
    // Attempt to get WinSock version 2.2
    // This is not required by POSIX-compliant systems.
    if (WSAStartup(MAKEWORD(2,2), &wsaData) < 0) {
        tcp_log_error("Windows failed to provide WinSook v2.2 API");
        return -1;
    }
#endif

    isInitialized = true;
    return 0;
} // tcp_init()



int tcp_client_connect(char const *hostname, int port) {
    SOCKET sd;
    struct sockaddr_in serverAddress;
    struct  hostent *host;


    log_logMessage((LOGLEVEL_DEBUG1, "tcp_client_connect('%s', %d)", hostname, port));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons((unsigned short) port);
    if ((host = gethostbyname(hostname)) == NULL) {
        // The 'hostname' is not a resolvable name, so try it as an IP address.
        serverAddress.sin_addr.s_addr = inet_addr(hostname);
        host = gethostbyaddr((char *)&serverAddress.sin_addr.s_addr,
                             sizeof(serverAddress.sin_addr.s_addr),
                             AF_INET);
    }
    if (NULL == host) {
        // We failed to get a host address.
        tcp_log_error("tcp_client_connect(): Unable to get host address");
        return -1;
    }
    memcpy(&(serverAddress.sin_addr.s_addr), host->h_addr, (unsigned short) host->h_length);

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        // An error occurred, errno will specify the reason.
        tcp_log_error("tcp_client_connect(): Unable to create client socket");
        return -2;
    }

    if (connect(sd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) != 0) {
        // The connection failed.
        tcp_log_error("tcp_client_connect(): Unable to connect to server");
        (void) closesocket(sd);
        return -3;
    }

    return (int) sd;
} // tcp_client_connect()



int tcp_close(int theSocket) {
    return closesocket((SOCKET) theSocket);
} // tcp_close()



ssize_t tcp_receive(int theSocket, char *pRxBuffer, size_t rxSize) {
    ssize_t rs;

    rs = recv((SOCKET) theSocket, pRxBuffer, (int) rxSize, 0);
    if (rs < 0) {
        tcp_log_error("recv() failed: ");
        return rs;
    }

#ifdef FTR_TCP_LOG_CONTENT
    tcp_log_data(pRxBuffer, rs, "<--[tcp]");
#endif // FTR_TCP_LOG_CONTENT

    if (0 == rs) {
        // Peer disconnected.
        (void) closesocket((SOCKET) theSocket);
    }


    return rs;
} // tcp_receive()



ssize_t tcp_recv_with_timeout(int theSocket,
                              char *pRxBuffer, size_t rxSize,
                              unsigned timeout_ms) {
   fd_set readset;
   int result;
   struct timeval tv;

   // Initialize the set
   FD_ZERO(&readset);
   FD_SET(theSocket, &readset);

   // Configure the timeout for the socket.
   tv.tv_sec = 0;
   tv.tv_usec = timeout_ms * 1000;
   result = select(theSocket+1, &readset, NULL, NULL, &tv);
   if (result < 0) {
      tcp_log_error("Select on socket failed");
      return -1;
   } else if ((result > 0) && FD_ISSET(theSocket, &readset)) {
      ssize_t rs;

      tcp_set_socket_nonblocking(theSocket);

      // Receive with timeout.
      rs = recv(theSocket, pRxBuffer, rxSize, 0);
#ifdef FTR_TCP_LOG_CONTENT
      tcp_log_data(pRxBuffer, rs, "<--[tcp]");
#endif // FTR_TCP_LOG_CONTENT

     tcp_set_socket_blocking(theSocket);

      return rs;
   }
   return -2;
} // tcp_recv_with_timeout()



int tcp_send(int theSocket, char const *pTxBuffer, size_t txSize) {
    if (send((SOCKET) theSocket, pTxBuffer, (int) txSize, 0) == -1) {
        // An error occurred while sending.
        tcp_log_error("Sending to socket failed");
        return -1;
    }

#ifdef FTR_TCP_LOG_CONTENT
    tcp_log_data(pTxBuffer, txSize, "[tcp]-->");
#endif // FTR_TCP_LOG_CONTENT

    return 0;
} // tcp_send()



ssize_t tcp_send_and_receive(int theSocket,
        char const *pTxBuffer, size_t txSize,
        char *pRxBuffer, size_t rxSize) {

    if (tcp_send(theSocket, pTxBuffer, txSize) < 0) {
        return -1;
    }

    memset(pRxBuffer, 0x00, rxSize);
    return tcp_receive(theSocket, pRxBuffer, rxSize);
} // tcp_send_and_receive()



int tcp_connect_to_server(int port) {
    SOCKET sd, sc;
    struct sockaddr_in server = { 0 };
    struct sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    char portString[10];    // much too long
    struct addrinfo *result = NULL, hints;
    int gaiResult;
#ifdef _WIN32
    int iOptval;
#endif // _WIN32


    log_logMessage((LOGLEVEL_DEBUG1, "tcp_connect_to_server(%d)", port));
    assert((port >= 0) && (port <= 65535));
    if (NULL == itoa(port, portString, 10)) {
        log_logMessage((LOGLEVEL_ERROR, "itoa(%d) failed.", port));
        return -6;
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = (int) IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the local address and port to be used by the server
    gaiResult = getaddrinfo(NULL, portString, &hints, &result);
    if (0 != gaiResult) {
        log_logMessage((LOGLEVEL_ERROR, "getaddrinfo failed: %d (%s)", gaiResult, gai_strerror(gaiResult)));
        return -1;
    }

    // Create the socket.
    if ((sd = socket(AF_INET, SOCK_STREAM, (int) IPPROTO_TCP)) == INVALID_SOCKET) {
        // An error occurred, errno will specify the reason.
        tcp_log_error("Server socket creation failed");
        return -1;
    }

#ifdef _WIN32
    // Set the exclusive address option
    // If you're really interested in the arcane details, check out
    // <http://msdn.microsoft.com/en-us/library/windows/desktop/ms740621(v=vs.85).aspx>
    // <http://msdn.microsoft.com/en-us/library/windows/hardware/ff570806(v=vs.85).aspx>
    // <http://msdn.microsoft.com/en-us/library/windows/desktop/cc150667(v=vs.85).aspx>
    iOptval = 1;
    if (setsockopt(sd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
                         (char *) &iOptval, sizeof (iOptval)) == SOCKET_ERROR) {
        tcp_log_error("setsockopt() failed");
        return -5;
    }
#endif // _WIN32

    // Configure and bind the socket.
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons((unsigned short) port);
    if (bind(sd, (struct sockaddr *) &server, sizeof(server)) != 0) {
        // Binding to the socket failed.
        tcp_log_error("Server socket bind() failed");
        (void) closesocket(sd);
        return -2;
    }

    // Tell the socket to listen for a connection.
    if (listen(sd, 4) != 0) {
        // Listening failed.
        tcp_log_error("Server socket listen failed");
        (void) closesocket(sd);
        return -3;
    }
    log_logMessage((LOGLEVEL_DEBUG1, "Server socket running on port %d", port));

    // Wait until we get a connection.
    if ((sc = accept(sd, (struct sockaddr *) &client, &clientSize)) < 0) {
        // An error occurred.
        tcp_log_error("Server socket accept failed");
        (void) closesocket(sd);
        return -4;
    }
    log_logMessage((LOGLEVEL_DEBUG1, "Server socket accepted connection from %s:%d", inet_ntoa(client.sin_addr), ntohs(client.sin_port)));

    return (int) sc;
} // tcp_connect_to_server()



int tcp_server(int port, serverfunction pServerFunction, bool multiThreaded) {
    int sd;
    struct sockaddr_in server;


    log_logMessage((LOGLEVEL_DEBUG1, "tcp_server(%d, @0x%p, %s))",
        port, pServerFunction, multiThreaded ? "true" : "false"));

#ifdef _WIN32
    if (multiThreaded) {
        log_logMessage((LOGLEVEL_ERROR, "tcp_server() multi-threading not supported on win32."));
        return -5;
    }
#endif // _WIN32


    // Set up the socket.
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        // An error occurred, errno will specify the reason.
        tcp_log_error("tcp_server(): socket() failed");
        return -1;
    }

    if (bind(sd, (struct sockaddr *) &server, sizeof(server)) != 0) {
        // Binding to the socket failed.
    tcp_log_error("tcp_server(): bind() failed");
        (void) closesocket(sd);
        return -2;
    }

    if (listen(sd, 4) != 0) {
        // Listening failed.
    tcp_log_error("tcp_server(): listen() failed");
        (void) closesocket(sd);
        return -3;
    }

    log_logMessage((LOGLEVEL_INFO, "Server running on port %d", port));
    for (;;) {
        struct sockaddr_in client;
        socklen_t clientSize = sizeof(client);
        int sc;

        if ((sc = accept(sd, (struct sockaddr *) &client, &clientSize)) < 0) {
            // An error occurred.
            tcp_log_error("tcp_server(): accept() failed");
            (void) closesocket(sd);
            return -4;
        }

#ifndef _WIN32
        if (multiThreaded) {
            pid_t childpid;
            childpid = fork();
            if (0 == childpid) {
                // Running the child process.
                (void) closesocket(sd);
                if (pServerFunction(sc, &client)) {
                    break;
                }
            } else {
                // Running the parent process.
                (void) closesocket(sc);
            }
        } else
#endif // !_WIN32
        {
            bool result = pServerFunction(sc, &client);
            (void) closesocket(sc);
            log_logMessage((LOGLEVEL_DEBUG1, "Closed connection on port %d", port));
            if (result) {
                break;
            }
        }
    } // forever

    log_logMessage((LOGLEVEL_INFO, "Terminating server running on port %d", port));
    (void) closesocket(sd);
    return 0;
} // tcp_server()



int tcp_server_multiport(unsigned nrOfSockets,
                         serversocket_t serversockets[],
                         struct timeval *pTimeout) {
    fd_set masterSet;
    unsigned i;
    int errorOccurred = 0;
    int maxFd = 0;    // Maximum file descriptor for select()


    // Create some fancy debug output.
    log_logMessageStart((LOGLEVEL_DEBUG1, "tcp_server_multiport(%u, [", nrOfSockets));
    for (i = 0;i < nrOfSockets;i ++) {
        log_logMessageContinue((LOGLEVEL_DEBUG1, "{ %d, @0x%p, @0x%p, @0x%p }",
                               serversockets[i].port,
                               serversockets[i].pFctConnect,
                               serversockets[i].pFctReceive,
                               serversockets[i].pFctDisconnect));
        if (i > 1) {
            log_logMessageContinue((LOGLEVEL_DEBUG1, ", "));
        }
    } // for i
    log_logMessageContinue((LOGLEVEL_DEBUG1, "])\n"));


    if (0 == nrOfSockets) {
        log_logMessage((LOGLEVEL_WARNING, "No server ports specified."));
        return 0;
    }

    FD_ZERO(&masterSet);

    for (i = 0;i < nrOfSockets;i ++) {
        struct sockaddr_in server;
        int option = 1;

        serversockets[i].clientSocket = INVALID_SOCKET;

        // Set up the socket.
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = htonl(INADDR_ANY);
        server.sin_port = htons(serversockets[i].port);
        if ((serversockets[i].listenSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
            // An error occurred, errno will specify the reason.
            log_logMessage((LOGLEVEL_ERROR,
                           "tcp_server_multiport(): socket() failed for socket #%d: %s",
                           i, strerror(errno)));
            errorOccurred = -1;
            break;
        }

        // Allow a previous socket to be re-used.
        if (setsockopt(serversockets[i].listenSocket,
                       SOL_SOCKET,
                       SO_REUSEADDR,
                       (char *)&option, sizeof(option)) != 0) {
            log_logMessage((LOGLEVEL_ERROR,
                           "tcp_server_multiport(): setsockopt() failed for socket #%d: %s",
                           i, strerror(errno)));
        }

        // Bind the socket to an address.
        if (bind(serversockets[i].listenSocket,
                 (struct sockaddr *) &server, sizeof(server)) != 0) {
            // Binding to the socket failed.
            log_logMessage((LOGLEVEL_ERROR,
                           "tcp_server_multiport(): bind() failed for socket #%d: %s",
                           i, strerror(errno)));
            errorOccurred = -2;
            break;
        }

        // Try to set non-blocking mode.
        if (!tcp_set_socket_nonblocking(serversockets[i].listenSocket)) {
            tcp_log_error("tcp_server_multiport(): setting non-blocking mode failed");
            errorOccurred = -3;
            break;
        }

        // Tell the socket to listen for a connection.
        // Allows no backlog because we do not multi-thread.
        if (listen(serversockets[i].listenSocket, 0) != 0) {
            tcp_log_error("tcp_server_multiport(): listen() failed");
            errorOccurred = -4;
            break;
        }

        // Add the socket to the listener sets for select().
        FD_SET(serversockets[i].listenSocket, &masterSet);
        if (maxFd <= serversockets[i].listenSocket) {
            maxFd = serversockets[i].listenSocket + 1;
        }
        log_logMessage((LOGLEVEL_INFO,
                       "Multi-port server listening on port %d with socket #%d",
                       serversockets[i].port, i));
    } // for i


    while (0 == errorOccurred) {
        struct timeval timeout;
        unsigned i;
        int nrSelectEvents;
        fd_set readSet, writeSet, errorSet;


        // Re-initialize because they may be changed by select()
        memcpy(&readSet, &masterSet, sizeof(masterSet));
        memcpy(&writeSet, &masterSet, sizeof(masterSet));
        memcpy(&errorSet, &masterSet, sizeof(masterSet));

        // Re-initialize timeout on every call because some implementations
        // of select() modify it.
        if (NULL != pTimeout) {
            timeout.tv_sec = pTimeout->tv_sec;
            timeout.tv_usec = pTimeout->tv_usec;
        } else {
            timeout.tv_sec = 0;
            timeout.tv_usec = 1000;
        }

        // Use select() to wait until something interesting occurs.
        nrSelectEvents = select(maxFd, &readSet, NULL /*&writeSet*/, &errorSet, &timeout);
        if (0 == nrSelectEvents) {
//            log_logMessage((LOGLEVEL_DEBUG1, "timeout in select()"));
            continue;
        }
        if (nrSelectEvents < 0) {
            tcp_log_error("tcp_server_multiport(): select failed");
            errorOccurred = -5;
            break;
        }

        // Now try to figure out which socket it was.
        for (i = 0;(i < nrOfSockets) && (nrSelectEvents > 0);i ++) {
            // Was it a listener socket?
            if (0 != FD_ISSET(serversockets[i].listenSocket, &readSet)) {
                nrSelectEvents--;
                // Accept the connection.
                log_logMessage((LOGLEVEL_DEBUG1,
                    "tcp_server_multiport(): server socket #%d received connection attempt.",
                    i));
                serversockets[i].clientSize = sizeof(serversockets[i].client);
                serversockets[i].clientSocket = accept(serversockets[i].listenSocket,
                                                       (struct sockaddr *) &serversockets[i].client,
                                                       &serversockets[i].clientSize);
                if (INVALID_SOCKET == serversockets[i].clientSocket) {
                    tcp_log_error("tcp_server_multiport(): accept() failed");
                    errorOccurred = -6;
                    break;
                }
                log_logMessage((LOGLEVEL_DEBUG1,
                               "tcp_server_multiport(): accepted connection from %s:%d",
                               inet_ntoa(serversockets[i].client.sin_addr),
                               ntohs(serversockets[i].client.sin_port)));

                // Try to set non-blocking mode.
                if (!tcp_set_socket_nonblocking(serversockets[i].clientSocket)) {
                    tcp_log_error("tcp_server_multiport(): setting non-blocking mode failed");
                    errorOccurred = -7;
                }

                // Add socket to sets.
                FD_SET(serversockets[i].clientSocket, &masterSet);
                if (maxFd <= serversockets[i].clientSocket) {
                    maxFd = serversockets[i].clientSocket + 1;
                 }

                 if (NULL != serversockets[i].pFctConnect) {
                    if (serversockets[i].pFctConnect(serversockets[i].clientSocket,
                                                     &serversockets[i].client)) {
                        log_logMessage((LOGLEVEL_DEBUG1, "tcp_server_multiport(): callback failed."));
                    }
                 }
            } // if listenSocket read

            if (0 != FD_ISSET(serversockets[i].clientSocket, &readSet)) {
                nrSelectEvents--;
                log_logMessage((LOGLEVEL_DEBUG1,
                               "tcp_server_multiport(): client socket #%d became readable.",
                               i));
                if (serversockets[i].pFctReceive(serversockets[i].clientSocket,
                                                 &serversockets[i].client)) {
                    log_logMessage((LOGLEVEL_DEBUG1, "tcp_server_multiport(): close client connection."));
                    FD_CLR(serversockets[i].clientSocket, &masterSet);
                    closesocket(serversockets[i].clientSocket);
                    if (NULL != serversockets[i].pFctDisconnect) {
                        serversockets[i].pFctDisconnect(serversockets[i].clientSocket, NULL);
                    }
                    serversockets[i].clientSocket = INVALID_SOCKET;
                }
            } // if clientSocket read

            if (0 != FD_ISSET(serversockets[i].listenSocket, &errorSet)) {
                int err = 0;
                socklen_t errlen = sizeof(err);

                nrSelectEvents--;
                if (getsockopt(serversockets[i].listenSocket,
                               SOL_SOCKET, SO_ERROR,
                               (char *) &err, &errlen) < 0) {
                    tcp_log_error("tcp_server_multiport(): getsockopt() failed");
                    errorOccurred = -8;
                }
                log_logMessage((LOGLEVEL_ERROR,
                               "tcp_server_multiport(): error on server socket: %s",
                               strerror(err)));
                closesocket(serversockets[i].listenSocket);
                serversockets[i].listenSocket = INVALID_SOCKET;
            } // if listenSocket error

            if (0 != FD_ISSET(serversockets[i].clientSocket, &errorSet)) {
                int err = 0;
                socklen_t errlen = sizeof(err);

                nrSelectEvents--;
                if (getsockopt(serversockets[i].clientSocket,
                               SOL_SOCKET, SO_ERROR,
                               (char *) &err, &errlen) < 0) {
                    tcp_log_error("tcp_server_multiport(): getsockopt() failed");
                    errorOccurred = -9;
                }
                if (err > 0) {
                    log_logMessage((LOGLEVEL_ERROR,
                        "tcp_server_multiport(): error on client socket #%d: %s",
                        i, strerror(err)));
                } else {
                    log_logMessage((LOGLEVEL_INFO,
                        "tcp_server_multiport() connection on socket #%d closed.",
                        i));
                }
                FD_CLR(serversockets[i].clientSocket, &masterSet);
                closesocket(serversockets[i].clientSocket);
                if (NULL != serversockets[i].pFctDisconnect) {
                    serversockets[i].pFctDisconnect(serversockets[i].clientSocket, NULL);
                }
                serversockets[i].clientSocket = INVALID_SOCKET;
            } // if clientSocket error
        } // for i
    } // while !errorOccurred


    // Close all open sockets to free used resources.
    for (i = 0;i < nrOfSockets;i ++) {
        if (0 != serversockets[i].clientSocket) {
            closesocket(serversockets[i].clientSocket);
            if (NULL != serversockets[i].pFctDisconnect) {
                serversockets[i].pFctDisconnect(serversockets[i].clientSocket,
                                                NULL);
            }
        }
        if (0 != serversockets[i].listenSocket) {
            closesocket(serversockets[i].listenSocket);
        }
    } // for i

    return errorOccurred;
} // tcp_server_multiport()



#ifdef TCPTEST
/** A simple server function that sends all received data back to the client.

   @param theSocket The socket to use for communications.
   @param from The client address.
 */
bool echoserver(int theSocket, struct sockaddr_in *from) {
    char rxTxBuffer[1024];

    printf("Serving %s:%d\n", inet_ntoa(from->sin_addr), ntohs(from->sin_port));

    for(;;) {
        int numRxBytes = recv(theSocket, rxTxBuffer, sizeof(rxTxBuffer), 0);

        if (numRxBytes > 0) {
            rxTxBuffer[numRxBytes] = '\0';
            printf("%s:%d -> '%s'\n",
                    inet_ntoa(from->sin_addr), ntohs(from->sin_port), rxTxBuffer);
            rxTxBuffer[0] = '*';
            if (send(theSocket, rxTxBuffer, numRxBytes, 0) != numRxBytes) {
                printf("Error while sending to %s:%d\n", inet_ntoa(from->sin_addr), ntohs(from->sin_port));
            }
            printf("%s:%d <- '%s'\n",
                    inet_ntoa(from->sin_addr), ntohs(from->sin_port), rxTxBuffer);
        } else {
            printf("Disconnected from %s:%d\n", inet_ntoa(from->sin_addr), ntohs(from->sin_port));
            (void) closesocket(theSocket);
            return true;
        }
    } // forever
} // echoserver()



bool multiechoserver(int theSocket,  struct sockaddr_in *from) {
    char rxTxBuffer[1024];
    int numRxBytes;


    log_logMessage((LOGLEVEL_DEBUG1,
                   "multiechoserver(%d): Serving %s:%d",
                   theSocket,
                   inet_ntoa(from->sin_addr), ntohs(from->sin_port)));

    numRxBytes = recv(theSocket, rxTxBuffer, sizeof(rxTxBuffer), 0);
    if (numRxBytes > 0) {
        rxTxBuffer[numRxBytes] = '\0';
        log_logMessage((LOGLEVEL_INFO,
                       "multiechoserver(%d): %s:%d -> '%s'",
                       theSocket,
                       inet_ntoa(from->sin_addr), ntohs(from->sin_port),
                       rxTxBuffer));
        rxTxBuffer[0] = '+';
        if (send(theSocket, rxTxBuffer, numRxBytes, 0) != numRxBytes) {
            log_logMessage((LOGLEVEL_ERROR,
                           "multiechoserver(%d): send() failed to %s:%d: %s",
                           theSocket,
                           inet_ntoa(from->sin_addr), ntohs(from->sin_port),
                           strerror(errno)));
        } else {
            log_logMessage((LOGLEVEL_INFO,
                           "multiechoserver(%d): %s:%d <- '%s'",
                           theSocket,
                           inet_ntoa(from->sin_addr), ntohs(from->sin_port),
                           rxTxBuffer));
        } // send() ok
    } else {
        log_logMessage((LOGLEVEL_INFO,
                       "multiechoserver(%d): Disconnected from %s:%d",
                       theSocket,
                       inet_ntoa(from->sin_addr), ntohs(from->sin_port)));
        return true;
    }

    return false;
} // multiechoserver()



static int port = 10255;

void spawnEchoServer(void) {
    // This is the child process, which we use for the server.
    printf("spawnEchoServer started\n");
    if (tcp_server(port, echoserver, false) < 0) {
        perror("Error in server: ");
        _endthreadex((unsigned)-1L);
    }
    printf("spawnEchoServer terminating\n");
    _endthreadex(0);
} // spawnEchoServer()



void spawnMultiEchoServer(void) {
    serversocket_t serversockets[2];


    // This is the child process, which we use for the server.
    printf("spawnMultiEchoServer started on port %d\n", port);
    memset(serversockets, 0, sizeof(serversockets));
    serversockets[0].port = port + 1;
    serversockets[0].pServerFunction = &multiechoserver;
    serversockets[1].port = port + 2;
    serversockets[1].pServerFunction = &multiechoserver;

    // This is the child process, which we use for the server.
    if (tcp_server_multiport(2, serversockets, NULL) < 0) {
        perror("Error in multiport server: ");
        _endthreadex((unsigned)-1L);
    }
    printf("spawnMultiEchoServer terminating\n");
    _endthreadex(0);
} // spawnMultiEchoServer()



/** Test program to perform a simple walkthrough of all functions in the
   module.
 */
int main(int argc, char *argv[]) {
    char sendBuffer[1024], receiveBuffer[1024];
    int clientSocket, cs1, cs2;
    ssize_t receiveLength;
#ifdef _WIN32
    uintptr_t serverThread;
#else
    pid_t serverThread;
#endif // !_WIN32


    if (argc > 1) {
        port = (int) strtol(argv[1], NULL, 0);
        if (EINVAL == errno) {
            printf("Usage: %s <port>\n", argv[0]);
            exit(-1);
        }
    }

    log_setStdoutLevel(LOGLEVEL_DEBUG1);
    printf("Using localhost:%d for server.\n", port);
    if (0 != tcp_init()) {
        perror("Unable to initialize TCP/IP subsystem.");
        exit(-1);
    }

#if 0
   //
   // Test single-port server.
   //
#ifdef _WIN32
    serverThread = _beginthread(spawnEchoServer, 0, NULL );
    if (0 == serverThread) {
        perror("_beginthread() failed.");
        exit(-1);
    }
#else
    if ((serverThread = fork()) == 0) {
        // This is the child process, which we use for the server.
        spawnEchoServer();
        // Won't return.
    }
#endif // !_WIN32

    // This is the parent process, which we use for the client.
    // Allow a little time to get the server running.
    sleep(1);

    clientSocket = tcp_client_connect("localhost", port);
    if (clientSocket < 0) {
        perror("Error while creating client socket: ");
        exit(clientSocket);
    }


    snprintf(sendBuffer, sizeof(sendBuffer), "Teststring");
    receiveLength = tcp_send_and_receive(clientSocket,
                        sendBuffer, strlen(sendBuffer),
                        receiveBuffer, sizeof(receiveBuffer));
    receiveBuffer[receiveLength] = '\0';
    printf("clientSocket <- '%s'\n", receiveBuffer);

    snprintf(sendBuffer, sizeof(sendBuffer), "A much longer string that will serve as the test string.");
    receiveLength = tcp_send_and_receive(clientSocket,
                        sendBuffer, strlen(sendBuffer),
                        receiveBuffer, sizeof(receiveBuffer));
    receiveBuffer[receiveLength] = '\0';
    printf("<- '%s'\n", receiveBuffer);

    tcp_close(clientSocket);
#endif // 0


    //
    // Test multi-port server.
    //
#ifdef _WIN32
    serverThread = _beginthread(spawnMultiEchoServer, 0, NULL);
    if (0 == serverThread) {
        perror("_beginthread() failed.");
        exit(-1);
    }
#else
    if ((serverThread = fork()) == 0) {
        // This is the child process, which we use for the server.
        spawnMultiEchoServer();
        return 0;
    }
#endif // !_WIN32

    // This is the parent process, which we use for the client.
    // Allow a little time to get the server running.
    sleep(10);

    cs1 = tcp_client_connect("localhost", port + 1);
    if (cs1 < 0) {
        perror("Error while creating client socket 1: ");
        exit(cs1);
    }
    sleep(1);
    cs2 = tcp_client_connect("localhost", port + 2);
    if (cs2 < 0) {
        perror("Error while creating client socket 2: ");
        exit(cs2);
    }


    snprintf(sendBuffer, sizeof(sendBuffer), "Test string for cs1");
    log_logMessage((LOGLEVEL_INFO, "cs1 -> '%s'\n", sendBuffer));
    receiveLength = tcp_send_and_receive(cs1,
                        sendBuffer, strlen(sendBuffer),
                        receiveBuffer, sizeof(receiveBuffer));
    receiveBuffer[receiveLength] = '\0';
    log_logMessage((LOGLEVEL_INFO, "cs1 <- '%s'", receiveBuffer));

    // Give it some more time.
    sleep(2);
    snprintf(sendBuffer, sizeof(sendBuffer), "Test string for cs2.");
    log_logMessage((LOGLEVEL_INFO, "cs2 -> '%s'", sendBuffer));
    receiveLength = tcp_send_and_receive(cs2,
                        sendBuffer, strlen(sendBuffer),
                        receiveBuffer, sizeof(receiveBuffer));
    receiveBuffer[receiveLength] = '\0';
    log_logMessage((LOGLEVEL_INFO, "cs2 <- '%s'", receiveBuffer));

    tcp_close(cs2);
    tcp_close(cs1);

    log_logMessage((LOGLEVEL_WARNING, "terminating main()"));
    exit(0);
} // main()
#endif // TCPTEST
