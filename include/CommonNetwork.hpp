/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CommonNetwork.hpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 17:40:38 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/05/26 09:41:10 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

// Networking related stuff, this is a translation layer from good ol' C.

#pragma once

#ifndef NETWORK_HPP
# define NETWORK_HPP
# include "Common.hpp"
# include "Exceptions.hpp"
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/poll.h>
# include <fcntl.h>
# include <map>
FT_BEGIN

// More sensible naming ... Thanks cstdlib :)
# define IPV4 PF_INET
# define TCP SOCK_STREAM

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
int32_t socket(int32_t Domain, int32_t Type, int32_t Protocol)
{
	int32_t fd = ::socket(Domain, Type, Protocol);
	if (fd < 0)
		throw ft::GenericErrnoExecption();
	return (fd);
}

// Assigns a name to an unnamed socket, requests that address be assigned to the socket.
void bind(int32_t socketFD, SocketAddress* address)
{
	socklen_t length = address->getSize();

	if (::bind(socketFD, reinterpret_cast<sockaddr*>(address), length) < 0)
		throw ft::GenericErrnoExecption();
}

// Defines the maximum length for the queue of pending connections.
void listen(int32_t socketFD, int32_t BackLog = 128)
{
	if (::listen(socketFD, BackLog) < 0)
		throw ft::GenericErrnoExecption();
}

// Extracts the first connection request on the queue of pending connections
int32_t accept(int32_t socketFD, SocketAddress* address)
{
	socklen_t length = address->getSize();

	int32_t fd = ::accept(socketFD, reinterpret_cast<sockaddr*>(address), &length);
	if (fd < 0)
		throw ft::GenericErrnoExecption();
	return (fd);
}

// Examines a set of file descriptors to see if some of them are ready for I/O or if certain events have occurred on them.
int32_t poll(struct pollfd fds[], size_t size, int32_t timeout)
{
	int32_t FDs = ::poll(fds, size, timeout);
	if (FDs < 0)
		throw ft::GenericErrnoExecption();
	return (FDs);
}

// Provides for control over descriptors.  The argument fildes is a descriptor to be operated on by cmd as follows
template<typename... Args>
int32_t fcntl(int32_t fd, int32_t cmd, Args... args)
{
	int32_t Value = ::fcntl(fd, cmd, std::forward<Args>(args)...);
	if (Value < 0)
		throw ft::GenericErrnoExecption();
	return (Value);
}

// Send a message from a socket.
ssize_t send(int32_t socket, const void* buffer, size_t length, int32_t flags)
{
	ssize_t Value = ::send(socket, buffer, length, flags);
	if (Value == -1)
		throw ft::GenericErrnoExecption();
	return (Value);
}

// Receive a message from a socket.
ssize_t receive(int32_t socket, void* buffer, size_t length, int32_t flags)
{
	ssize_t Value = ::recv(socket, buffer, length, flags);
	if (Value == -1)
		throw ft::GenericErrnoExecption();
	return (Value);
}

// Manipulate the options associated with a socket.
int32_t setSocketOption(int32_t socket, int32_t level, int32_t optionName, bool optionValue, size_t optionLen)
{
	int32_t Value = setsockopt(socket, level, optionName, &optionValue, optionLen);
	if (Value == -1)
		throw ft::GenericErrnoExecption();
	return (Value);	
}

// HTTP Status codes
//////////////////////////////////////////

/**
 * HTTP status codes with their definitions
 * 
 * @see https://bit.ly/3No2GDN
 */
const std::map<uint16_t, std::string>& getStatusCodes()
{
	static std::map<uint16_t, std::string> list = {
		// 1xx Informational
		{100, "Continue"},
		{101, "Switching Protocols"},
		{102, "Processing"},

		// 2xx Success
		{200, "OK"},
		{201, "Created"},
		{202, "Accepted"},
		{203, "Non-authoritative Information"},
		{204, "No Content"},
		{205, "Reset Content"},
		{206, "Partial Content"},
		{207, "Multi-Status"},
		{208, "Already Reported"},
		{226, "IM Used"},

		// 3xx Redirection
		{300, "Multiple Choices"},
		{301, "Moved Permanently"},
		{302, "Found"},
		{303, "See Other"},
		{304, "Not Modified"},
		{305, "Use Proxy"},
		{307, "Temporary Redirect"},
		{308, "Permanent Redirect"},

		// 4xx Client Error
		{400, "Bad Request"},
		{401, "Unauthorized"},
		{402, "Payment Required"},
		{403, "Forbidden"},
		{404, "Not Found"},
		{405, "Method Not Allowed"},
		{406, "Not Acceptable"},
		{407, "Proxy Authentication Required"},
		{408, "Request Timeout"},
		{409, "Conflict"},
		{410, "Gone"},
		{411, "Length Required"},
		{412, "Precondition Failed"},
		{413, "Payload Too Large"},
		{414, "Request-URI Too Long"},
		{415, "Unsupported Media Type"},
		{416, "Requested Range Not Satisfiable"},
		{417, "Expectation Failed"},
		{418, "I'm a teapot"},
		{421, "Misdirected Request"},
		{422, "Unprocessable Entity"},
		{423, "Locked"},
		{424, "Failed Dependency"},
		{426, "Upgrade Required"},
		{428, "Precondition Required"},
		{429, "Too Many Requests"},
		{431, "Request Header Fields Too Large"},
		{444, "Connection Closed Without Response"},
		{451, "Unavailable For Legal Reasons"},
		{499, "Client Closed Request"},
			
		//5xx Server Error
		{500, "Internal Server Error"},
		{501, "Not Implemented"},
		{502, "Bad Gateway"},
		{503, "Service Unavailable"},
		{504, "Gateway Timeout"},
		{505, "HTTP Version Not Supported"},
		{506, "Variant Also Negotiates"},
		{507, "Insufficient Storage"},
		{508, "Loop Detected"},
		{510, "Not Extended"},
		{511, "Network Authentication Required"},
		{599, "Network Connect Timeout Error"}
	};

	return (list);
}

FT_END
#endif
