/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/02 12:25:53 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/14 21:20:46 by pvan-dij      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef SERVER_HPP
# define SERVER_HPP
# include "CGI.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "ServerSection.hpp"
# include "CommonNetwork.hpp"
FT_BEGIN

// A single server that listens on a port and handles incoming requests.
class Server
{
public: // Ctor ~ Dtor
	Server(ft::ServerSection& inConfig);

public: // Functions

	void init(void);
	void run(void);

	void pollListen(void);
	void pollInEvent(pollfd* poll);
	void pollOutEvent(pollfd* poll);
	void cleanSocket(pollfd *poll);
	bool checkTimeout(pollfd *poll);
	void resolveConnection(pollfd *poll);
	void generateOutStatus(pollfd *poll, int code);

public: // Attributes

	ft::ServerSection config;

	std::map<int32_t, ft::Response *> responses;
	std::map<int32_t, ft::Request *> requests;
	std::map<int32_t, std::string> req_buf;

private:

	// Network part
	int32_t serverFD;
	ft::SocketAddress address;

	//server info
	std::list<std::string> serverNames;

	// Running part
	int32_t maxClients;
	int32_t numFds;
	pollfd* pollfds;
	nfds_t nfds;
	std::map<int32_t, time_t> timeout;
};

FT_END
#endif
