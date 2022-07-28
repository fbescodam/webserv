/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/27 11:08:47 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/28 16:57:02 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef SERVER_HPP
# define SERVER_HPP
# include "ServerSection.hpp"
# include "Utils.hpp"
# define IPV4 PF_INET
# define TCP SOCK_STREAM
# define MASTER_SOCKET 0
# define MAX_CLIENTS 60

namespace ft {

// Server is a running handler for receiving and sending http requests / responses.
class Server
{
public: // Ctor ~ Dtor
	Server(ft::ServerSection& inConfig);

public: // Functions
	bool init(void);
	ft::fd_t getSocket(void) const;
	ft::SocketAddress& getAddress(void) const;

public: // Functions
	/**
	 * @brief Send a response to the current client with a given status code.
	 *
	 * @param poll The current connection.
	 * @param statusCode The status code to send back.
	 */
	void respondWithStatus(pollfd* poll, int32_t statusCode);

private:
	void pollListen(void);
	void pollInEvent(pollfd* poll);
	void pollOutEvent(pollfd* poll);

public: // Attributes
	// The server's own copy of the config
	ft::ServerSection config;

private:

	// Network part
	ft::fd_t			socket;
	ft::SocketAddress	address;
};
}
#endif
