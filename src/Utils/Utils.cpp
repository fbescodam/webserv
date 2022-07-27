/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Utils.cpp                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/27 10:41:28 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/27 11:38:56 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"

void ft::trim(std::string& string)
{
	string.erase(0, string.find_first_not_of(WHITESPACE)); // trim whitespace from left
	string.erase(string.find_last_not_of(WHITESPACE) + 1); // trim whitespace from right
}

void ft::slice(const std::string& string, std::string delim, std::pair<std::string, std::string>& output)
{
	size_t start, end = 0;
	start = string.find_first_of(delim);
	if (start == std::string::npos)
		throw ft::DelimiterNotFoundException();
	output.first = string.substr(0, start);
	output.second = string.substr(start + 1);
}

//////////////////////////////////////////

int32_t ft::socket(int32_t domain, int32_t type, int32_t protocol)
{
	int32_t fd = ::socket(domain, type, protocol);
	if (fd < 0)
		throw ft::GenericErrnoException();
	return (fd);
}

void ft::bind(int32_t socketFD, ft::SocketAddress* address)
{
	socklen_t length = address->getSize();

	if (::bind(socketFD, reinterpret_cast<sockaddr*>(address), length) < 0)
		throw ft::GenericErrnoException();
}

void ft::listen(int32_t socketFD, int32_t BackLog = 128)
{
	if (::listen(socketFD, BackLog) < 0)
		throw ft::GenericErrnoException();
}

int32_t ft::accept(int32_t socketFD, ft::SocketAddress* address)
{
	socklen_t length = address->getSize();

	int32_t fd = ::accept(socketFD, reinterpret_cast<sockaddr*>(address), &length);
	if (fd < 0)
		throw ft::GenericErrnoException();
	return (fd);
}

int32_t ft::poll(struct pollfd fds[], size_t size, int32_t timeout)
{
	int32_t FDs = ::poll(fds, size, timeout);
	if (FDs < 0)
		throw ft::GenericErrnoException();
	return (FDs);
}

int32_t ft::setSocketOption(int32_t socket, int32_t level, int32_t optionName, bool optionValue, size_t optionLen)
{
	int32_t Value = setsockopt(socket, level, optionName, &optionValue, optionLen);
	if (Value == -1)
		throw ft::GenericErrnoException();
	return (Value);
}

std::string ft::inet_ntop(SocketAddress& address) 
{
	// 4 bytes, IPV4
	static char buf[sizeof(int32_t) * 4];
	sockaddr_in temp = address.getCStyle();

	if (::inet_ntop(AF_INET, &temp, buf, sizeof(buf)) == nullptr)
		return "";
	return std::string(buf);
}
