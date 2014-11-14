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


    Copyright (c) 2011-2014, Christian Doenges (Christian D&ouml;nges) All rights
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

    $ gcc -DTEST -o tcputils tcputils.c

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
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include "itoa.h" // itoa() is present only on Windows.
#else
#include <io.h>
#include <Winsock2.h>
#include <Ws2tcpip.h>
// link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")
#ifdef TEST
#include <windows.h>
#include <process.h>    /* _beginthread, _endthread */
#endif // TEST
#endif // _WIN32
#include <signal.h>
#include <sys/types.h>

#include "logging.h"
#include "tcputils.h"


static bool isInitialized = false;

#ifndef _WIN32
// Windows uses closesocket() instead of close() to close a socket.
#define closesocket close
/** Windows uses a proprietary SOCKET definition instead of the POSIX int for file handles. */
typedef int SOCKET;
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
static char const *winsock_strerror(void) {
    WCHAR wtext[1024];
    static char ctext[1024];
    size_t csize;
    int error = WSAGetLastError();  // Obtain the last WinSock error code.

    // Convert to a wide string representation.
    //lint -e{64} Lint does not like argument 5.
    (void) FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                  NULL,
                  (DWORD) error,
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
    // Windows nicely formats error messages by appending a period.
    log_logMessage(LOGLEVEL_ERROR, "%s: %s",
                   message,
                   winsock_strerror());
#else // POSIX
    log_logMessage(LOGLEVEL_ERROR, "%s: %s.",
                   message,
                   strerror(errno));
#endif // POSIX
} // tcp_log_error()



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
        tcp_log_error("Unable to get host address");
        return -1;
    }
    memcpy(&(serverAddress.sin_addr.s_addr), host->h_addr, (unsigned short) host->h_length);

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        // An error occurred, errno will specify the reason.
        tcp_log_error("Unable to create client socket");
        return -2;
    }

    if (connect(sd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) != 0) {
        // The connection failed.
        tcp_log_error("Unable to connect to server");
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
    if (0 == rs) {
        // Peer disconnected.
        (void) closesocket((SOCKET) theSocket);
    }

    return rs;
} // tcp_receive()



int tcp_send(int theSocket, char const *pTxBuffer, size_t txSize) {
    if (send((SOCKET) theSocket, pTxBuffer, (int) txSize, 0) == -1) {
        // An error occurred while sending.
        tcp_log_error("Sending to socket failed");
        return -1;
    }
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


    log_logMessage(LOGLEVEL_DEBUG, "tcp_connect_to_server(%d)", port);
    assert((port >= 0) && (port <= 65535));
    if (NULL == itoa(port, portString, 10)) {
        log_logMessage(LOGLEVEL_ERROR, "itoa(%d) failed.", port);
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
        log_logMessage(LOGLEVEL_ERROR, "getaddrinfo failed: %d (%s)", gaiResult, gai_strerror(gaiResult));
        return -1;
    }

    // Create the socket.
    if ((sd = socket(AF_INET, SOCK_STREAM, (int) IPPROTO_TCP)) == -1) {
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
    log_logMessage(LOGLEVEL_DEBUG1, "Server socket running on port %d", port);

    // Wait until we get a connection.
    if ((sc = accept(sd, (struct sockaddr *) &client, &clientSize)) < 0) {
        // An error occurred.
        tcp_log_error("Server socket accept failed");
        (void) closesocket(sd);
        return -4;
    }
    log_logMessage(LOGLEVEL_DEBUG1, "Server socket accepted connection from %s:%d", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

    return (int) sc;
} // tcp_connect_to_server()



int tcp_server(int port, serverfunction pServerFunction, bool multiThreaded) {
    int sd;
    struct sockaddr_in server;


    log_logMessage(LOGLEVEL_DEBUG, "tcp_server(%d, @0x%p, %s)",
        port, pServerFunction, multiThreaded ? "true" : "false");

#ifdef _WIN32
    if (multiThreaded) {
        log_logMessage(LOGLEVEL_ERROR, "tcp_server() multi-threading not supported on win32.");
        return -5;
    }
#endif // _WIN32


    // Set up the socket.
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        // An error occurred, errno will specify the reason.
        log_logMessage(LOGLEVEL_ERROR, "tcp_server(): socket() failed: %s", strerror(errno));
        return -1;
    }

    if (bind(sd, (struct sockaddr *) &server, sizeof(server)) != 0) {
        // Binding to the socket failed.
        log_logMessage(LOGLEVEL_ERROR, "tcp_server(): bind() failed: %s", strerror(errno));
        (void) closesocket(sd);
        return -2;
    }

    if (listen(sd, 4) != 0) {
        // Listening failed.
        log_logMessage(LOGLEVEL_ERROR, "tcp_server(): listen() failed: %s", strerror(errno));
        (void) closesocket(sd);
        return -3;
    }

    log_logMessage(LOGLEVEL_INFO, "Server running on port %d", port);
    for (;;) {
        struct sockaddr_in client;
        socklen_t clientSize = sizeof(client);
        int sc;

        if ((sc = accept(sd, (struct sockaddr *) &client, &clientSize)) < 0) {
            // An error occurred.
            log_logMessage(LOGLEVEL_ERROR, "tcp_server(): accept() failed: %s", strerror(errno));
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
            closesocket(sc);
            if (result) {
                break;
            }
        }
    } // forever

    (void) closesocket(sd);
    return 0;
} // tcp_server()



#ifdef TEST
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


#ifdef _WIN32
int port = 10250;

void spawnEchoServer(void) {
 
    // This is the child process, which we use for the server.
    if (tcp_server(port, echoserver, false) < 0) {
        perror("Error in server: ");
        _endthreadex((unsigned)-1L);
    }
    printf("spawnEchoServer terminating\n");
    _endthreadex(0);
} // spawnEchoServer()



/** Test program to perform a simple walkthrough of all functions in the
   module.
 */
int main(int argc, char *argv[]) {
    char sendBuffer[1024], receiveBuffer[1024];
    int clientSocket;
    ssize_t receiveLength;
    uintptr_t serverThread;
 

    if (argc > 1) {
        port = (int) strtol(argv[1], NULL, 0);
        if (EINVAL == errno) {
            printf("Usage: %s <port>\n", argv[0]);
            exit(-1);
        }
    }

    printf("Using localhost:%d for server.\n", port);
    if (0 != tcp_init()) {
        perror("Unable to initialize TCP/IP subsystem.");
        exit(-1);
    }

    serverThread = _beginthread(spawnEchoServer, 0, NULL );
    if (0 == serverThread) {
        perror("_beginthread() failed.");
        exit(-1);
    }

    // This is the parent process, which we use for the client.
    clientSocket = tcp_client_connect("localhost", port);

    if (clientSocket < 0) {
        perror("Error while creating client socket: ");
        exit(clientSocket);
    }

    _snprintf_s(sendBuffer, sizeof(sendBuffer), sizeof(sendBuffer), "Teststring");
    receiveLength = tcp_send_and_receive(clientSocket,
                        sendBuffer, strlen(sendBuffer),
                        receiveBuffer, sizeof(receiveBuffer));
    receiveBuffer[receiveLength] = '\0';
    printf("<- '%s'\n", receiveBuffer);

    _snprintf_s(sendBuffer, sizeof(sendBuffer), sizeof(sendBuffer), "A much longer string that will serve as the test string.");
    receiveLength = tcp_send_and_receive(clientSocket,
                        sendBuffer, strlen(sendBuffer),
                        receiveBuffer, sizeof(receiveBuffer));
    receiveBuffer[receiveLength] = '\0';
    printf("<- '%s'\n", receiveBuffer);

    tcp_close(clientSocket);

    exit(0);
} // main()
#endif // TEST
#endif // _WIN32
