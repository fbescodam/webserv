/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/27 11:08:47 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/08/11 19:49:51 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef SERVER_HPP
# define SERVER_HPP
# include "Socket.hpp"
# include "ServerSection.hpp"
# include "Connection.hpp"
# include "Response.hpp"
# include "Request.hpp"

namespace ft {

// Server is a running handler for receiving and sending http requests / responses.
class Server
{
public: // Ctor ~ Dtor
	Server(ft::ServerSection& inConfig);

public: // Functions
	void setSocket(const ft::Socket* socket);
	const ft::Socket* getSocket(void) const;

public: // Functions
	/**
	 * @brief Send a response to the current client with a given status code.
	 *
	 * @param poll The current connection.
	 * @param statusCode The status code to send back.
	 */
	void respondWithStatus(ft::Connection& conn, int32_t statusCode);
	void handleRequest(ft::Connection& conn);

private:
	void pollListen(void);
	void pollInEvent(pollfd* poll);
	void pollOutEvent(pollfd* poll);

public: // Attributes
	// The server's own copy of the config
	ft::ServerSection config;

private:

	// Network part
	ft::Socket*	socket;
};
}
#endif
