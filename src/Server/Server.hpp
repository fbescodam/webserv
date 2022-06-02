/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/02 12:25:53 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/02 14:11:32 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef SERVER_HPP
# define SERVER_HPP
# include "Request.hpp"
# include "Response.hpp"
# include "ServerEntry.hpp"
# include "CommonNetwork.hpp"
FT_BEGIN

// A single server that listens on a port and handles incoming requests.
class Server final
{
public: // Ctor ~ Dtor
	Server(ft::ServerEntry& inConfig);

public: // Functions

	void init(void);
	void run(void);

public: // Attributes

	// The servers current configuration. TODO: Const ?
	ft::ServerEntry& config;

private:

	// Network part
	int32_t serverFD, clientSocket, valRead;
	ft::SocketAddress address;

	// Running part
	int32_t maxClients;
	int32_t numFds;
	pollfd* pollfds;
	nfds_t nfds;
};

FT_END
#endif
