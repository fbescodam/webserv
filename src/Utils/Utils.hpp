/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Utils.hpp                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/27 10:40:21 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/08/18 15:32:48 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef UTILS_HPP
# define UTILS_HPP
# define WHITESPACE " \t\r\n\t\f\v"
# define NONE 0
# define GREEN "\033[32;1m"
# define BLACK "\033[30;1m"
# define RED "\033[31;1m"
# define RESET "\033[0m"
# define BUFF_SIZE 4096
# define CONN_TIMEOUT 30 // in seconds
# define MAX_CLIENTS 60
# define DEFAULT_LIMIT_BODY_SIZE 8000000 // 8MB
# define IPV4 PF_INET
# define TCP SOCK_STREAM
# define MASTER_SOCKET 0
# define WRITE 1
# define READ 0
# include <string>
# include <poll.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/poll.h>
# include <fcntl.h>
# include <unistd.h>
# include <map>
# include <sstream>
# include <vector>
# include <algorithm>
# include "Exceptions.hpp"
# include "SocketAddress.hpp"
namespace ft {

typedef int32_t fd_t;

// NOTE: Cant be bothered with templates just yet ...

//////////////////////////////////////////
// COMMON
//////////////////////////////////////////

/**
 * Remove whitespace characters from the beginning and end of a string.
 *
 * @param string The string to trim. Warning: gets modified
 */
void trim(std::string& string);

/**
 * Slices a string into two parts at the first occurence of the delimiter.
 *
 * @param string The string to split.
 * @param delim The delimiter.
 * @param output The output.
 */
void slice(const std::string& string, std::string delim, std::pair<std::string, std::string>& output);

/**
 * @brief Split a string into many parts
 *
 * @param string
 * @param delim
 * @param outVec
 */
void split(const std::string& string, char delim, std::vector<std::string>& outVec);

/**
 * @brief Check if a string ends with another string
 *
 * @param value
 * @param ending
 * @return true
 * @return false
 */
bool endsWith(const std::string& value, const std::string& ending);

/**
 * @brief Make a string all-lowercase
 *
 * @param string
 */
void tolower(std::string& string);

//////////////////////////////////////////
// NETWORK
//////////////////////////////////////////

// Socket creates an endpoint for communication and returns a descriptor.
ft::fd_t socket(int32_t domain, int32_t type, int32_t protocol);

// Assigns a name to an unnamed socket, requests that address be assigned to the socket.
void bind(int32_t socketFD, SocketAddress* address);

// Defines the maximum length for the queue of pending connections.
void listen(int32_t socketFD, int32_t BackLog);

// Extracts the first connection request on the queue of pending connections
ft::fd_t accept(int32_t socketFD, SocketAddress* address);

// Examines a set of file descriptors to see if some of them are ready for I/O or if certain events have occurred on them.
int32_t poll(struct pollfd fds[], size_t size, int32_t timeout);

// Receive a message from a socket.
ssize_t receive(ft::fd_t socket, void* buffer, size_t length, int32_t flags);

// Manipulate the options associated with a socket.
int32_t setSocketOption(ft::fd_t socket, int32_t level, int32_t optionName, bool optionValue, size_t optionLen);

// Inet_ntop
std::string inet_ntop(SocketAddress& address);

//////////////////////////////////////////
// UNIX
//////////////////////////////////////////

// pipe wrapper.
void pipe(int32_t fds[2]);

// fork wrapper.
int32_t fork(void);

// Execve wrapper.
int32_t execve(const std::string& file, char* const* argv, char* const* envp);

// dup2 wrapper.
int32_t dup2(int32_t fdsA, int32_t fdsB);

// Provides for control over descriptors.  The argument fildes is a descriptor to be operated on by cmd as follows
template<typename... Args>
int32_t fcntl(int32_t fd, int32_t cmd, Args... args)
{
	int32_t Value = ::fcntl(fd, cmd, std::forward<Args>(args)...);
	if (Value < 0) throw ft::GenericErrnoException();
	return (Value);
}

//////////////////////////////////////////
// MISC
//////////////////////////////////////////

/**
 * @brief Get the list of exisiting status codes.
 *
 * @return const std::map<uint16_t, std::string>&
 */
const std::map<uint16_t, std::string>& getStatusCodes();

/**
 * @brief Retrieve the content type value based on the given extension.
 *
 * @param ext The file extension, include the '.' in it.
 * @return std::string& The content type.
 */
std::string getContentType(std::string ext);

}
#endif
