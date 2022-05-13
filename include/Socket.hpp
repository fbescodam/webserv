/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Socket.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/13 12:30:26 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/05/13 14:17:31 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

//= Socket wrapper header to make the usage more streamlined.

#ifndef SOCKET_HPP
# define SOCKET_HPP
# include <netinet/in.h>
# include <sys/socket.h>
# include "Exceptions.hpp"

//= Typedefs =//

typedef struct sockaddr_in IntSockAddr_t;

namespace ft
{
    // Communication domain protocol.
    enum class Domain : uint8_t
    {
        DM_IPV4 = PF_INET,
        DM_IPV6 = PF_INET6
    };

    // Internet packet protocols.
    enum class Protocol : uint8_t
    {
        // Provides sequenced, reliable, two-way connection based byte streams.
        PR_TCP = SOCK_STREAM,

        // Supports datagrams (connectionless, unreliable messages of a fixed (typically small) maximum length)
        PR_UDP = SOCK_DGRAM,

        // Provide access to internal network protocols and interfaces
        PR_RAW = SOCK_RAW,
    };
}
//= Functions =//
// Wrapper functions to add documentation and remove the "C'ness" from it.

namespace ft
{
    /**
     * Creates an endpoint for communication and returns a descriptor.
     *
     * TODO: Clear up last param
     * 
     * @param[in] Domain Specifies a communications domain within which communication will take place.
     * @param[in] Protocol Specifies the semantics of communication, aka protocol model (TCP, UDP, RAW)
     * @param[in] SocketProtocol Specifies a particular protocol to be used with the socket. (?)
     * 
     * @return -1 is returned if an error occurs, otherwise its a descriptor referencing the socket.
     */
    int32_t Socket(ft::Domain Domain, ft::Protocol Protocol, int32_t SocketProtocol)
    {
        int32_t fd = socket(static_cast<int32_t>(Domain), static_cast<int32_t>(Protocol), SocketProtocol);
        if (fd < 0)
            throw ft::GenericErrnoExecption();
        return (fd);
    }

    /**
     * Bind assigns a name to an unnamed socket. When a socket is created with ft::Socket it exists in a name space (address family) 
     * but has no name assigned. It requests that address be assigned to the socket.
     * 
     * @param Socketfd 
     * @param Address 
     * @param AddressLength 
     */
    void Bind(int32_t Socketfd, IntSockAddr_t* Address, size_t AddressLength)
    {
        if (bind(Socketfd, reinterpret_cast<struct sockaddr*>(Address), AddressLength) < 0)
            throw ft::GenericErrnoExecption();
    }

    /**
     * Listen for connections on a socket.
     * 
     * @param Socketfd The socket file descriptor.
     * @param BackLog Defines the maximum length for the queue of pending connections.
     */
    void Listen(int32_t Socketfd, int32_t BackLog)
    {
        if (listen(Socketfd, BackLog) < 0)
            throw ft::GenericErrnoExecption();
    }
}

#endif
