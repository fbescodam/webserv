/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/02 12:34:20 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/03 14:43:55 by pvan-dij      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

ft::Server::Server(ft::ServerEntry& inConfig) : config(inConfig) { }

//////////////////////////////////////////

// TODO: Actually read the config, for now hardcode to test
void ft::Server::init(void)
{
	try
	{
		address = ft::SocketAddress(AF_INET, htons(8080), INADDR_ANY); // needs config values
		serverFD = ft::socket(IPV4, TCP, NONE);
		ft::setSocketOption(serverFD, SOL_SOCKET, SO_REUSEADDR, true, sizeof(bool)); // make kernel release socket after exit
		ft::bind(serverFD, &address);
		ft::listen(serverFD, 128);
		ft::fcntl(serverFD, F_SETFL, O_NONBLOCK);

		nfds = 0;
		maxClients = 5; // this should be gotten from config
		numFds = 1;
		pollfds = new pollfd[maxClients];
		pollfds->fd = serverFD;
		pollfds->events = POLLIN;
	}
	catch(const std::exception& e)
	{
		ft::exceptionExit(e, EXIT_FAILURE);
	}
}

void ft::Server::run(void)
{
	nfds = numFds;
	ft::poll(pollfds, nfds, 0); //check our open fds for events

	for (int i = 0; i < numFds; i++)
	{
		if (!i)
		{
			if (pollfds[0].revents & POLLIN)
				this->newSocket();
		}
		else
		{
			if ((pollfds + i)->revents & POLLIN) //pollfd is ready for reading
				this->pollInEvent(i);
			else if ((pollfds + i)->revents & POLLOUT) //pollfd is ready for writing
				this->pollOutEvent(i);
		}
	}
}

//////////////////////////////////////////
