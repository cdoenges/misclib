/** Simple TCP communication utility functions.



    @file tcputils.h
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

 */

#ifndef TCPUTILS_H
#define TCPUTILS_H

#include <stdbool.h>
#include <string.h>



#ifndef _WIN32
#include <arpa/inet.h>
#include <sys/socket.h>

/** Windows uses this to specify an error during socket creation. */
#define INVALID_SOCKET -1

#else
#ifndef _WINDOWS_
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif // _WINDOWS_
//lint -e{451} WinSock2.h does not have an include guard that PC-Lint likes.
#include <WinSock2.h>
#include <Ws2tcpip.h>

/** Signed size_t.

   This type is used in POSIX systems, but MS Windows is not really POSIX
   compliant.
 */
typedef long ssize_t;
#endif // _WIN32



/** Server function that is called by tcp_server whenever a new connection
   is established.

   @param int sc - the socket used to communicate with the client.
   @param struct sockaddr_in *from - The client address.
   @return Shut down the server?
   @retval true Close down the server.
   @retval false Keep on accepting connections from clients.
 */
typedef bool serverfunction(int, struct sockaddr_in *);

typedef struct {
    /** The port to listen on. */
    int port;
    /** The function to call when a client connects to the socket.
       May be NULL.

       @note If you read from the socket at this point, it will probably block.
     */
    serverfunction *pFctConnect;
    /** The function to call when data is received by the socket. */
    serverfunction *pFctReceive;
    /** The function to call when a client disconnects to the socket.
       May be NULL.

       @note The socket is already closed at this point, and the client address
       will be NULL, so do not try to use the socket. It is provided only for
       reference purposes, so you can clean up internal data structures.
     */
    serverfunction *pFctDisconnect;
    // Private from here on.
    /** The server socket that will listen for incoming connections. */
    int listenSocket;
    /** The client socket that will communicate with a client. */
    int clientSocket;
    /** The address of the client (only valid while clientSocket is active). */
    struct sockaddr_in client;
    /** The size of client. */
    socklen_t clientSize;
} serversocket_t;



/** Set the socket into non-blocking mode.

    @param theSocket The filedescriptor of the socket to change.
    @return Was the socket set to non-blocking?
    @retval true The socket is non-blocking.
    @retval false Changing the socket to non-blocking failed.
 */
extern bool tcp_set_socket_nonblocking(int theSocket);



/** Set the socket into blocking mode.

    @param theSocket The filedescriptor of the socket to change.
    @return Was the socket set to blocking?
    @retval true The socket is blocking.
    @retval false Changing the socket to blocking failed.
 */
extern bool tcp_set_socket_blocking(int theSocket); 



/** Initializes the TCP/IP subsystem.

   @return Error status.
   @retval 0 All went well.
   @retval -1 An error occurred.
 */
extern int tcp_init(void);



/** Opens a TCP connection to the specified host and port.

   @param hostname The name of the host (or IP address) to connect to as a string.
   @param port The port to connect to.
   @return The socket connected to the server.
   @retval &lt;0 An error occurred. errno identifies the cause.
 */
extern int tcp_client_connect(char const *hostname, int port);




/** Opens a TCP server socket on the specified port.

   @param port The port to listen on.
   @return The server socket.
   @retval &lt;0 An error occurred. errno identifies the cause.
 */
extern int tcp_server_connect(int port);



/** Closes the given socket to release resources.

   @param theSocket The socket to diconnect and close.
   @return Error status.
   @retval 0 No error occurred.
   @retval -1 An error occurred, errno contains the details.
*/
extern int tcp_close(int theSocket);



/** Receives data from the socket.

   Receiving is synchronous, so the process will block while waiting for data.

   @param theSocket The connected socket to use.
   @param pRxBuffer The buffer that will contain the received data.
   @param rxSize The number of bytes that can be stored in the receive buffer.
   @return The number of bytes received and stored in the receive buffer.
   @retval 0 The peer disconnected, the socket was closed.
   @retval &lt;0 An error occurred. errno identifies the cause.
 */
extern ssize_t tcp_receive(int theSocket, char *pRxBuffer, size_t rxSize);



/** Sends data through the socket.

   @param theSocket The connected socket to use.
   @param pTxBuffer The buffer containing the data to send.
   @param txSize The number of bytes to send.
   @return Did an error occur?
   @retval 0 The data was sent without error.
   @retval &lt;0 An error occurred. errno identifies the cause.
 */
extern int tcp_send(int theSocket, char const *pTxBuffer, size_t txSize);



/** Sends data through the socket and receives a response.

   Receiving is synchronous, so the process will block while waiting for data.

   @param theSocket The connected socket to use.
   @param pTxBuffer The buffer containing the data to send.
   @param txSize The number of bytes to send.
   @param pRxBuffer The buffer that will contain the received data.
   @param rxSize The number of bytes that can be stored in the receive buffer.
   @return The number of bytes received and stored in the receive buffer.
   @retval 0 The peer disconnected, the socket was closed.
   @retval &lt;0 An error occurred. errno identifies the cause.
 */
extern ssize_t tcp_send_and_receive(int theSocket,
        char const *pTxBuffer, size_t txSize,
        char *pRxBuffer, size_t rxSize);



/** A simple server that listens on the specified port and calls the
   serverfunction whenever a new client connection is made.

   @param port The port the server will listen on.
   @param pServerFunction Pointer to the function that will handle the connection
      once it has been established.
   @param multiThreaded True if connections to the server spawn a new thread,
      false otherwise.
   @return Did an error occur?
   @retval 0 No error occurred.
   @retval &lt;0 An error occurred. Consult errno for details.
*/
extern int tcp_server(int port, serverfunction pServerFunction, bool multiThreaded);



/** A simple server that opens multiple ports and calls the corresponding
   serverfunction whenever there is a connection.

   This is a single-threaded implementation.

   @param nrOfSockets The number of sockets in the next parameter.
   @param serversockets An array of socket descriptions. The port and serverfunction
   must be set.
   @param pTimeout The timeout for any connection. The server will be shut down
   if no activity occurs for the timeout amount of time. Set to NULL to wait
   indefinitely. 
   @return Did an error occur?
   @retval 0 No error occurred.
   @retval &lt;0 An error occurred. Consult errno for details.
*/
extern int tcp_server_multiport(unsigned nrOfSockets,
                         serversocket_t serversockets[],
                         struct timeval *pTimeout);
#endif // TCPUTILS_H
