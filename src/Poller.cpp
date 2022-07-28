/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Poller.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/28 15:48:13 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/28 17:01:38 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Poller.hpp"

ft::Poller::Poller(const std::vector<ft::Server>& servers) : servers(servers)
{
	// first few pollfds are reserved for the server sockets, initialize them here
	for (size_t i = 0; i < this->servers.size(); i++)
	{
		this->pollfds[i].fd = servers.at(i).getSocket();
		this->pollfds[i].events = POLLIN;
	}
}

//////////////////////////////////////////

bool ft::Poller::acceptIncoming(const ft::Server& server, const pollfd& fd)
{
	if (this->activeClients >= MAX_CLIENTS)
	{
		std::cout << RED << "Refusing incoming connection, max clients has been reached" << RESET << std::endl;
		return (false);
	}
	try
	{
		ft::fd_t clientSocket = ft::accept(server.getSocket(), &server.getAddress());
		ft::fcntl(clientSocket, F_SETFL, O_NONBLOCK);

		// Find available file descriptor
		for (ft::fd_t i = this->servers.size() - 1; i < this->pollfds.size(); i++)
		{
			pollfd* poll = &this->pollfds[i];

			// Already in use, skip
			if (poll->fd != -1) continue;

			std::cout << GREEN << "Accepting incoming connection" << RESET << std::endl;

			poll->fd = clientSocket;
			poll->events = POLLIN;
			poll->revents = NONE;
			this->activeClients++;

			return (true);
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << RED << e.what() << RESET << '\n';
		return (false);
	}

	// This should never happen
	std::cout << RED << "Refusing incoming connection, no unused pollfd found" << RESET << std::endl;
	return (false);
}

void ft::Poller::pollAll(void)
{
	try { ft::poll(this->pollfds.data(), this->activeClients + this->servers.size(), -1); }
	catch (GenericErrnoException& e)
	{
		std::cerr << RED << "Webserv: " << e.what() << RESET << std::endl;
		exit(EXIT_FAILURE);
	}

	// Check the result for all the pollers
	for (size_t i = 0; i < this->activeClients + this->servers.size(); i++)
	{
		pollfd& fd = this->pollfds[i];

		// First few polls are the listening thingies (one for each server), attaches new clients to a pollfd.
		// When a POLLIN event happens here, we need to specify which pollfd to use for this connection.

		if (i < this->servers.size() && (pollfds[i].revents & POLLIN))
			this->acceptIncoming(this->servers.at(i), fd);

		// TODO: attach this connection to a free pollfd
	}
}


//////////////////////////////////////////