/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CommonNetwork.hpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 17:40:38 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/05/25 17:40:18 by lde-la-h      ########   odam.nl         */
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
	SocketAddress(uint8_t InFamily, uint16_t InPort, uint32_t InAddress)
	{
		AddressFamily = InFamily;
		Port = InPort;
		Address = InAddress;
	}

public: // Functions

	// Converts the wrapper to the C Style type.
	struct sockaddr_in GetCStyle(void) const
	{
		struct sockaddr_in Out = {
			.sin_len = Length,
			.sin_family = AddressFamily,
			.sin_port = Port,
			.sin_addr.s_addr = Address,
		};

		std::memset(Out.sin_zero, 0, sizeof(Out.sin_zero));
		return (Out);
	}

	// Gets the size of the Socket Address.
	constexpr size_t GetSize(void) const
	{
		return (sizeof(SocketAddress));
	}

public: // Attributes
	uint8_t		Length;
	uint8_t		AddressFamily;
	uint16_t	Port;
	uint32_t	Address;
protected:
	char		Zero[8]; // Literally useless ?
};

//////////////////////////////////////////

// Socket creates an endpoint for communication and returns a descriptor.
int32_t Socket(int32_t Domain, int32_t Type, int32_t Protocol)
{
	int32_t fd = socket(Domain, Type, Protocol);
	if (fd < 0)
		throw ft::GenericErrnoExecption();
	return (fd);
}

// Assigns a name to an unnamed socket, requests that address be assigned to the socket.
void Bind(int32_t Socketfd, SocketAddress* Address, size_t AddressLength)
{
	if (bind(Socketfd, reinterpret_cast<sockaddr*>(Address), AddressLength) < 0)
		throw ft::GenericErrnoExecption();
}

// Defines the maximum length for the queue of pending connections.
void Listen(int32_t Socketfd, int32_t BackLog = 128)
{
	if (listen(Socketfd, BackLog) < 0)
		throw ft::GenericErrnoExecption();
}

// Extracts the first connection request on the queue of pending connections
int32_t Accept(int32_t Socketfd, SocketAddress* Address)
{
	socklen_t Length = Address->GetSize();

	int32_t fd = accept(Socketfd, reinterpret_cast<sockaddr*>(Address), &Length);
	if (fd < 0)
		throw ft::GenericErrnoExecption();
	return (fd);
}

// Examines a set of file descriptors to see if some of them are ready for I/O or if certain events have occurred on them.
int32_t Poll(struct pollfd Fds[], size_t Size, int32_t Timeout)
{
	int32_t FDs = poll(Fds, Size, Timeout);
	if (FDs < 0)
		throw ft::GenericErrnoExecption();
	return (FDs);
}

// Provides for control over descriptors.  The argument fildes is a descriptor to be operated on by cmd as follows
template<typename... Args>
int32_t Fcntl(int32_t Fd, int32_t Cmd, Args... args)
{
	int32_t Value = fcntl(Fd, Cmd, std::forward<Args>(args)...);
	if (Value < 0)
		throw ft::GenericErrnoExecption();
	return (Value);
}

// Send a message from a socket.
ssize_t Send(int32_t Socket, const void* Buffer, size_t Length, int32_t Flags)
{
	ssize_t Value = send(Socket, Buffer, Length, Flags);
	if (Value == -1)
		throw ft::GenericErrnoExecption();
	return (Value);
}

// Receive a message from a socket.
ssize_t Receive(int32_t Socket, void* Buffer, size_t Length, int32_t Flags)
{
	ssize_t Value = recv(Socket, Buffer, Length, Flags);
	if (Value == -1)
		throw ft::GenericErrnoExecption();
	return (Value);
}

// Manipulate the options associated with a socket.
int32_t SetSocketOption(int32_t Socket, int32_t Level, int32_t OptionName, bool OptionValue, size_t OptionLen)
{
	int32_t Value = setsockopt(Socket, Level, OptionName, &OptionValue, OptionLen);
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
const std::map<uint16_t, std::string>& GetStatusCodes()
{
	static std::map<uint16_t, std::string> List = {
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

	return (List);
}

FT_END
#endif
