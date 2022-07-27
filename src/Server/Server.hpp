/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/27 11:08:47 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/27 11:39:17 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef SERVER_HPP
# define SERVER_HPP
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/poll.h>
# include <fcntl.h>
# include "ServerSection.hpp"
# include "SocketAddress.hpp"
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

	void init(void);
	void run(void);

public: // Attributes

    // The server's own copy of the config
    ft::ServerSection config;

private:

	// Network part
	int32_t serverFD;
	ft::SocketAddress address;

    // Poll
	int32_t numFds;
	pollfd* pollfds;
	nfds_t nfds;

	std::map<int32_t, time_t> timeout;
};
}
#endif
