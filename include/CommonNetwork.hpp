/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CommonNetwork.hpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 17:40:38 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/30 15:07:57 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

// Networking related stuff, this is a translation layer from good ol' C.

#pragma once

#ifndef NETWORK_HPP
# define NETWORK_HPP
# include "Common.hpp"
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/poll.h>
# include <fcntl.h>
# include <map>
# include "Exceptions.hpp"
FT_BEGIN

// More sensible naming ... Thanks cstdlib :)
# define IPV4 PF_INET
# define TCP SOCK_STREAM
# define MASTER_SOCKET 0

// C Network function & type wrappers
//////////////////////////////////////////

/**
 * Socket address, internet style.
 *
 * Wrapper for struct sockaddr_in.
 */
struct SocketAddress
{
public: // Ctor
	SocketAddress() = default;
	SocketAddress(uint8_t inFamily, uint16_t inPort, uint32_t inAddress)
	{
		addressFamily = inFamily;
		port = inPort;
		address = inAddress;
	}

public: // Functions

	// Converts the wrapper to the C Style type.
	struct sockaddr_in getCStyle(void) const
	{
		struct sockaddr_in out = {
			.sin_len = length,
			.sin_family = addressFamily,
			.sin_port = port,
			.sin_addr.s_addr = address,
		};

		std::memset(out.sin_zero, 0, sizeof(out.sin_zero));
		return (out);
	}

	// Gets the size of the Socket Address.
	constexpr size_t getSize(void) const
	{
		return (sizeof(SocketAddress));
	}

public: // Attributes
	uint8_t		length;
	uint8_t		addressFamily;
	uint16_t	port;
	uint32_t	address;
protected:
	char		zero[8]; // Literally useless ?
};

//////////////////////////////////////////

// Socket creates an endpoint for communication and returns a descriptor.
int32_t socket(int32_t Domain, int32_t Type, int32_t Protocol);

// Assigns a name to an unnamed socket, requests that address be assigned to the socket.
void bind(int32_t socketFD, SocketAddress* address);

// Defines the maximum length for the queue of pending connections.
void listen(int32_t socketFD, int32_t BackLog);

// Extracts the first connection request on the queue of pending connections
int32_t accept(int32_t socketFD, SocketAddress* address);

// Examines a set of file descriptors to see if some of them are ready for I/O or if certain events have occurred on them.
int32_t poll(struct pollfd fds[], size_t size, int32_t timeout);

// Provides for control over descriptors.  The argument fildes is a descriptor to be operated on by cmd as follows
template<typename... Args>
int32_t fcntl(int32_t fd, int32_t cmd, Args... args)
{
	int32_t Value = ::fcntl(fd, cmd, std::forward<Args>(args)...);
	if (Value < 0)
		throw ft::GenericErrnoException();
	return (Value);
}

// Send a message from a socket.
ssize_t send(int32_t socket, const void* buffer, size_t length, int32_t flags);

// Receive a message from a socket.
ssize_t receive(int32_t socket, void* buffer, size_t length, int32_t flags);

// Manipulate the options associated with a socket.
int32_t setSocketOption(int32_t socket, int32_t level, int32_t optionName, bool optionValue, size_t optionLen);

// Inet_ntop
std::string inet_ntop(SocketAddress& address);

// HTTP Status codes
//////////////////////////////////////////

/**
 * @brief Get the Content Type of a file
 * 
 * @param ext entire file name. yes its named ext, that is a lie
 * @return std::string& 
 */
std::string& getContentType(std::string ext);

/**
 * HTTP status codes with their definitions
 *
 * @see https://bit.ly/3No2GDN
 */
const std::map<uint16_t, std::string>& getStatusCodes();

FT_END
#endif
