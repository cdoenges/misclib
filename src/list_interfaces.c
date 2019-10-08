/** List all network interfaces.

    @file list_interfaces.c
    @ingroup misclib


    @author Christian D&ouml;nges <cd@platypus-projects.de>

    @note The master repository for this file is at
     <a href="https://github.com/cdoenges/misclib">https://github.com/cdoenges/misclib</a>

    LICENSE
    SPDX-License-Identifier: BSD-3-Clause

    This software is open source software under the "Simplified BSD License"
    as approved by the Open Source Initiative (OSI)
    <http://opensource.org/licenses/bsd-license.php>:


    Copyright (c) 2019, Christian Doenges (Christian D&ouml;nges) All rights
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

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#ifdef HAVE_SYS_SOCKIO_H
#include <sys/sockio.h>
#endif

#include <net/if.h>
#include <netinet/in.h>



/** Iterate all network interfaces and output each that has an IP address.
  */
bool list_interfaces(char *error_buffer, size_t error_buffer_size, bool debug) {
    int socket_fd;

    *error_buffer = '\0';   // Make sure the error buffer is clean.

    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)) < 0) {
        (void) snprintf(error_buffer, error_buffer_size, "socket(AF_INET, SOCK_DGRAM, IPPROTO_IP) failed: %s", strerror(errno));
        return false;
    }

    char ibuf[sizeof(struct ifreq) * 512u];
    struct ifconf ifc;
    ifc.ifc_buf = ibuf;
    ifc.ifc_len = sizeof(ibuf);

    if (ioctl(socket_fd, SIOCGIFCONF, (char *)&ifc) < 0) {
        if (EINVAL == errno) {
            (void) snprintf(error_buffer, error_buffer_size, "Not enough space for struct ifreq buffer (%u bytes)", ifc.ifc_len);
        } else {
            (void) snprintf(error_buffer, error_buffer_size, "ioctl(SIOCGIFCONF) failed: %s", strerror(errno));
        }
        (void) close(socket_fd);
        return false;
    } // if !ioctl(SIOCGIFCONF)

    for (int i = 0;i < ifc.ifc_len;i ++) {
        if (0 == (i & 0xf)) {
            printf("\n%04x  ", i);
        }
        printf("%02x ", *((unsigned char *)ifc.ifc_buf + i));
    } // for i
    printf("\n");

    struct ifreq *ifrp, *ifend, *ifnext;
    ifend = (struct ifreq *)(ifc.ifc_buf + ifc.ifc_len);
    for (ifrp = (struct ifreq *)(ifc.ifc_buf);ifrp < ifend;ifrp = ifnext) {
#ifdef _SIZEOF_ADDR_IFREQ
        unsigned ifr_size = _SIZEOF_ADDR_IFREQ(*ifrp);
        ifnext = (struct ifreq *)((char *)ifrp + ifr_size);
#else
#ifdef HAVE_SOCKADDR_SA_LEN
        unsigned ifr_size = ifrp->ifr_addr.sa_len + sizeof(ifrp->ifr_name);
        if (n < sizeof(*ifrp)) {
            ifnext = ifrp + 1;
        } else {
            ifnext = (struct ifreq *)((char *)ifrp + ifr_size);
        }
#else
        ifnext = ifrp + 1;
#endif // !HAVE_SOCKADDR_SA_LEN
#endif // _SIZEOF_ADDR_IFREQ

        if ('\0' == *ifrp->ifr_name) {
            // Skip anonymous inferface.
            continue;
        }
        if (debug) {
            printf("Found interface '%s' ", ifrp->ifr_name);
            for (unsigned i = 0;i < sizeof(ifrp->ifr_name);++ i) {
                printf("%02x ", (unsigned char) (ifrp->ifr_name[i]));
            }
            printf("\n");
        } // if debug


        // Use a fresh ifreq instance because SIOCGIFFLAGS will overwrite the
        // existing structure, since it uses a union.
        struct ifreq ifr;
        strncpy(ifr.ifr_name, ifrp->ifr_name, sizeof(ifr.ifr_name));
        if (ioctl(socket_fd, SIOCGIFFLAGS, (char *)&ifr) < 0) {
            if (errno == ENXIO) {
                // Ignore "No such device or address."
                if (debug) {
                    printf("Interface '%s': no such device or address.\n", ifrp->ifr_name);
                } // if debug
                continue;
            }
            (void) snprintf(error_buffer, error_buffer_size, "ioctl(%s, SIOCGIFFLAGS) failed: %s", ifr.ifr_name, strerror(errno));
            (void) close(socket_fd);
            return false;
        } // if !ioctl(SIOCGIFFLAGS)

        if (0 == (ifr.ifr_flags & IFF_UP)) {
            // Ignore an interface that is down.
            if (debug) {
                printf("Interface '%s' is down.\n", ifrp->ifr_name);
            } // if debug
            continue;
        }

        if (ioctl(socket_fd, SIOCGIFADDR, (char *)&ifr) < 0) {
            (void) snprintf(error_buffer, error_buffer_size, "ioctl(%s, SIOCGIFADDR) failed: %s", ifr.ifr_name, strerror(errno));
            (void) close(socket_fd);
            return false;
        }

        char address_str[INET6_ADDRSTRLEN] = "ERROR";
        switch (ifrp->ifr_addr.sa_family) {
            case AF_UNSPEC:
                strcpy(address_str, "AF_UNSPEC");
                break;
            case AF_UNIX:
                strcpy(address_str, "AF_UNIX");
                break;
#if AF_LOCAL != AF_UNIX
            case AF_LOCAL:
                strcpy(address_str, "AF_LOCAL");
                break;
#endif // AF_LOCAL
            case AF_INET:
                inet_ntop(AF_INET, &(((struct sockaddr_in *)&ifrp->ifr_addr)->sin_addr), address_str, sizeof(address_str));
                break;
#ifdef AF_AX25
            case AF_AX25:
                strcpy(address_str, "AF_AX25");
                break;
#endif // AF_AX25
            case AF_IPX:
                strcpy(address_str, "AF_IPX");
                break;
            case AF_APPLETALK:
                strcpy(address_str, "AF_APPLETALK");
                break;
#ifdef AF_NETROM
            case AF_NETROM:
                strcpy(address_str, "AF_NETROM");
                break;
#endif // AF_NETROM
#ifdef AF_BRIDGE
            case AF_BRIDGE:
                strcpy(address_str, "AF_BRIDGE");
                break;
#endif // AF_BRIDGE
#ifdef AF_ATMPVC
            case AF_ATMPVC:
                strcpy(address_str, "AF_ATMPVC");
                break;
#endif // AF_ATMPVC
#ifdef AF_X25
            case AF_X25:
                strcpy(address_str, "AF_X25");
                break;
#endif // AF_X25
            case AF_INET6:
                inet_ntop(AF_INET, &(((struct sockaddr_in6 *)&ifrp->ifr_addr)->sin6_addr), address_str, sizeof(address_str));
                break;
#ifdef AF_ROSE
            case AF_ROSE:
                strcpy(address_str, "AF_ROSE");
                break;
#endif // AF_ROSE
            case AF_DECnet:
                strcpy(address_str, "AF_DECnet");
                break;
#ifdef AF_NETBEUI
            case AF_NETBEUI:
                strcpy(address_str, "AF_NETBEUI");
                break;
#endif // AF_NETBEUI
#ifdef AF_SECURITY
            case AF_SECURITY:
                strcpy(address_str, "AF_SECURITY");
                break;
#endif // AF_SECURITY
#ifdef AF_KEY
            case AF_KEY:
                strcpy(address_str, "AF_KEY");
                break;
#endif // AF_KEY
#ifdef AF_NETLINK
            case AF_NETLINK:
                strcpy(address_str, "AF_NETLINK");
                break;
#endif // AF_NETLINK
#ifdef AF_PACKET
            case AF_PACKET:
                strcpy(address_str, "AF_PACKET");
                break;
#endif // AF_PACKET
#ifdef AF_ASH
            case AF_ASH:
                strcpy(address_str, "AF_ASH");
                break;
#endif // AF_ASH
#ifdef AF_ECONET
            case AF_ECONET:
                strcpy(address_str, "AF_ECONET");
                break;
#endif // AF_ECONET
#ifdef AF_ATMSVC
            case AF_ATMSVC:
                strcpy(address_str, "AF_ATMSVC");
                break;
#endif // AF_ATMSVC
#ifdef AF_RDS
            case AF_RDS:
                strcpy(address_str, "AF_RDS");
                break;
#endif // AF_RDS
            case AF_SNA:
                strcpy(address_str, "AF_SNA");
                break;
#ifdef AF_IRDA
            case AF_IRDA:
                strcpy(address_str, "AF_IRDA");
                break;
#endif // AF_IRDA
#ifdef AF_PPPOX
            case AF_PPPOX:
                strcpy(address_str, "AF_PPPOX");
                break;
#ifdef AF_WANPIPE
#endif // AF_PPPOX
            case AF_WANPIPE:
                strcpy(address_str, "AF_WANPIPE");
                break;
#ifdef AF_LLC
#endif // AF_WANPIPE
            case AF_LLC:
                strcpy(address_str, "AF_LLC");
                break;
#endif // AF_LLC
#ifdef AF_CAN
            case AF_CAN:
                strcpy(address_str, "AF_CAN");
                break;
#endif // AF_CAN
#ifdef AF_TIPC
            case AF_TIPC:
                strcpy(address_str, "AF_TIPC");
                break;
#endif // AF_TIPC
#ifdef AF_BLUETOOTH
            case AF_BLUETOOTH:
                strcpy(address_str, "AF_BLUETOOTH");
                break;
#ifdef AF_IUCV
#endif // AF_BLUETOOTH
            case AF_IUCV:
                strcpy(address_str, "AF_IUCV");
                break;
#endif // AF_IUCV
#ifdef AF_RXRPC
            case AF_RXRPC:
                strcpy(address_str, "AF_RXRPC");
                break;
#endif // AF_RXRPC
            case AF_ISDN:
                strcpy(address_str, "AF_ISDN");
                break;
#ifdef AF_PHONET
            case AF_PHONET:
                strcpy(address_str, "AF_PHONET");
                break;
#endif // AF_PHONET
#ifdef AF_IEEE802154
            case AF_IEEE802154:
                strcpy(address_str, "AF_IEEE802154");
                break;
#endif // AF_IEEE802154
            case AF_MAX:
                strcpy(address_str, "AF_MAX");
                break;
            default:
                if (debug) {
                    printf("Interface '%s' has protocol %d\n", ifr.ifr_name, ifrp->ifr_addr.sa_family);
                }
                strcpy(address_str, "PROTOCOL NOT SUPPORTED");
                break;
        } // switch sa_family

        printf("Interface '%s' has address %s\n", ifr.ifr_name, address_str);
    } // for ifrp

    (void) close(socket_fd);
    return true;
} // list_interfaces()


#ifdef UNITTEST
int main(int argc, char *argv[]) {
    char error_buffer[1024];

    if (!list_interfaces(error_buffer, sizeof(error_buffer), true)) {
        fprintf(stderr, "ERROR: %s\n", error_buffer);
    }
} // main()
#endif // UNITTEST
