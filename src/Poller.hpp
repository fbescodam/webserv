/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Poller.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/28 15:48:18 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/08/16 11:36:22 by lde-la-h      ########   odam.nl         */
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
# include "GlobalConfig.hpp"
# include "Server.hpp"
# include "Response.hpp"
# include "Request.hpp"
# include "Connection.hpp"

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

public: // Functions
	void setServers(const size_t amount);
	void pollAll(void);
    ft::Server* getFirstServerOfPort(uint16_t port);

private: // Functions
	bool acceptIncoming(const ft::Server& server);
	void pollInEvent(ft::Connection& conn);
	void pollOutEvent(ft::Connection& conn);
	void closeConnection(ft::Connection& conn);		// Close, then reset a connection
	void resetConnection(ft::Connection& conn);		// Reset a connection without setting the pollfd to -1
	void deleteReqRes(ft::Connection& conn);		// Reset request and response instances for connection
	void resolveConnection(ft::Connection& conn);	// Resolve a connection: close it or keep it alive based on the headers in the response
	const ft::Socket* createSocket(const uint16_t port);
	const ft::Socket* getSocketByPort(const uint16_t port) const;

private: // Attributes
	std::vector<ft::Server>&				servers;				// A reference to the vector list of servers defined in main
	const ft::GlobalConfig&					globalConfig;			// A reference to the global configuration for webserv defined in main
	size_t									activeClients;			// The amount of active client connections right now.
	size_t									reservedSocketAmount;	// The amount of sockets that are reserved for incoming requests to servers
	std::vector<Socket*>					sockets;				// A vector containing all the sockets (one per defined port in the config file)
	std::array<ft::Connection, MAX_CLIENTS>	connections;			// An array containing all our connections. The first few are reserved for incoming connections for the server, one per server.
	std::array<pollfd, MAX_CLIENTS>			pollfds;				// An array containing all pollfds. The first few are reserved for incoming connections for the server, one per server.
	char									buffer[BUFF_SIZE];		// The buffer for reading from a pollInEvent
};

}
#endif
