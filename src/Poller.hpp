/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Poller.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/28 15:48:18 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/28 17:53:40 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef POLLER_HPP
# define POLLER_HPP
# include <vector>
# include <array>
# include <poll.h>
# include <algorithm>
# include "Utils.hpp"
# include "Server.hpp"

namespace ft {

/**
 * @brief the Poller class handles all the polling and attaches servers to each.
 *
 */
class Poller
{
public: // Ctor ~ Dtor
	Poller(const std::vector<ft::Server>& servers);
	~Poller();

protected: // Types

	// A connection to a client for example.
	struct Connection
	{
		ft::Server*	server;			// The server to which this connection belongs.
		time_t		lastActivity;	// The timestamp pointing to the last time there was activity on this connection.
		std::string	ipv4;			// Client IP.
	};

public: // Functions
	void setServers(const size_t amount);
	void pollAll(void);

private: // Functions
	bool acceptIncoming(const ft::Server& server);
	void pollInEvent(pollfd& fd, Connection& conn);
	void pollOutEvent(pollfd& fd, Connection& conn);
	void closeConnection(pollfd& fd, Connection& conn);
	void resetConnection(Connection& conn);

private: // Attributes
	size_t									activeClients;		// The amount of active client connections right now.
	const std::vector<ft::Server>&			servers;			// The vector list of servers
	std::array<Connection, MAX_CLIENTS + 1>	connections;		// An array containing all our connections. The first few are reserved for incoming connections for the server, one per server.
	std::array<pollfd, MAX_CLIENTS + 1>		pollfds;			// All of our pollfds used by poll wow so weird who would have thought that
	char									buffer[BUFF_SIZE];	// The buffer for reading from a pollInEvent
};

}
#endif
