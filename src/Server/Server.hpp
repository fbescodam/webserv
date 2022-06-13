/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/02 12:25:53 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/13 18:27:42 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef SERVER_HPP
# define SERVER_HPP
# include "Request.hpp"
# include "Response.hpp"
# include "ServerSection.hpp"
# include "CommonNetwork.hpp"
FT_BEGIN

// A single server that listens on a port and handles incoming requests.
class Server final
{
public: // Ctor ~ Dtor
	Server(ft::ServerSection& inConfig);

public: // Functions

	void init(void);
	void run(void);

	void pollListen(void);
	void pollInEvent(int32_t i);
	void pollOutEvent(int32_t i);

public: // Attributes

	// The servers current configuration. TODO: Const ?
	ft::ServerSection& config;

private:

	// Network part
	int32_t serverFD, clientSocket;
	ft::SocketAddress address;

	// Running part
	int32_t maxClients;
	int32_t numFds;
	pollfd* pollfds;
	nfds_t nfds;

	const char* hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
};

FT_END
#endif
