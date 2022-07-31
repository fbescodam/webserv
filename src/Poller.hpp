/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Poller.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/28 15:48:18 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/31 14:34:27 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef POLLER_HPP
# define POLLER_HPP
# include "Poller.hpp"
# include <vector>
# include <array>
# include <poll.h>
# include <algorithm>
# include <deque>
# include "Utils.hpp"
# include "Server.hpp"
# include "GlobalConfig.hpp"

namespace ft {

/**
 * @brief the Poller class handles all the polling and attaches servers to each.
 *
 */
class Poller
{
public: // Ctor ~ Dtor
	Poller(std::vector<ft::Server>& servers, const ft::GlobalConfig& globalConfig);
	~Poller();

public: // Types
	// A connection to a client for example.
	struct Connection
	{
		pollfd*		poll;			// The pollfd used for this connection
		ft::Server*	server;			// The server to which this connection belongs.
		time_t		lastActivity;	// The timestamp pointing to the last time there was activity on this connection.
		std::string	ipv4;			// Client IP.
	};

	struct Socket
	{
		ft::SocketAddress	addr;	// The socket's address
		ft::fd_t			fd;		// The socket's file descriptor
	};

public: // Functions
	void setServers(const size_t amount);
	void pollAll(void);

private: // Functions
	bool acceptIncoming(const ft::Server& server);
	void pollInEvent(Connection& conn);
	void pollOutEvent(Connection& conn);
	void closeConnection(Connection& conn);		// Close, then reset a connection
	void resetConnection(Connection& conn);		// Reset a connection without setting the pollfd to -1
	const ft::Poller::Socket* createSocket(const uint16_t port);
	const ft::Poller::Socket* getSocketByPort(const uint16_t port) const;

private: // Attributes
	std::vector<ft::Server>&			servers;				// A reference to the vector list of servers defined in main
	const ft::GlobalConfig&				globalConfig;			// A reference to the global configuration for webserv defined in main
	size_t								activeClients;			// The amount of active client connections right now.
	size_t								reservedSocketAmount;	// The amount of sockets that are reserved for incoming requests to servers
	std::vector<Socket*>				sockets;				// A vector containing all the sockets (one per defined port in the config file)
	std::array<Connection, MAX_CLIENTS>	connections;			// An array containing all our connections. The first few are reserved for incoming connections for the server, one per server.
	std::array<pollfd, MAX_CLIENTS>		pollfds;				// An array containing all pollfds. The first few are reserved for incoming connections for the server, one per server.
	char								buffer[BUFF_SIZE];		// The buffer for reading from a pollInEvent
};

}
#endif
