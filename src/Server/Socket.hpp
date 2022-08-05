/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Socket.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/27 11:22:23 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/31 16:20:41 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef SOCKET_HPP
# define SOCKET_HPP
# include "Utils.hpp"
# include "SocketAddress.hpp"

namespace ft {

struct Socket
{
public: // Ctor

	/**
	 * @brief Construct a new Socket object.
	 * 
	 * @param[in] port The port on which to listen to.
	 * @param[in] max_clients The maximum amount of clients that can connect.
	 */
	Socket(const uint16_t port, const uint32_t max_clients)
	{
		this->fd = ft::socket(IPV4, TCP, NONE); // Create a new socket fd
		this->addr = ft::SocketAddress(AF_INET, htons(port), INADDR_ANY); // Create a new address on the defined port

		ft::setSocketOption(this->fd, SOL_SOCKET, SO_REUSEADDR, true, sizeof(int32_t)); // Make kernel release socket after exit
		ft::bind(this->fd, &this->addr); // Bind the socket to an address
		ft::listen(this->fd, max_clients); // Allow the socket to listen, set the maximum amount of clients
		ft::fcntl(this->fd, F_SETFL, O_NONBLOCK); // Set to non-blocking mode for use with poll
	}

public: // Attributes
	ft::SocketAddress	addr;	// The socket's address
	ft::fd_t			fd;		// The socket's file descriptor
};

}
#endif
