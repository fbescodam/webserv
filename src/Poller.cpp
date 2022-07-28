/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Poller.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/28 15:48:13 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/28 17:43:15 by lde-la-h      ########   odam.nl         */
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
		Connection& conn = this->connections[i];

		// First few polls are the listening thingies (one for each server), attaches new clients to a pollfd.
		// When a POLLIN event happens here, we need to specify which pollfd to use for this connection.
		if (i < this->servers.size() && (fd.revents & POLLIN))
			this->acceptIncoming(this->servers.at(i));
		else if (fd.revents & POLLIN) // Client connection ready for reading
			this->pollInEvent(fd, conn);
		else if (fd.revents & POLLOUT) // Client connection ready for writing
			this->pollOutEvent(fd, conn);
	}

	// Shift all valid connections to the front.
	// While all inactive once get shifted to the back.
	for (size_t i = 0; i < this->pollfds.size(); i++)
	{
		// Connection is not active
		if (this->pollfds[i].fd == -1)
		{
			pollfd temp = this->pollfds[i];
			this->pollfds[i] = this->pollfds[i + 1];
			this->pollfds[i + 1] = temp;
		}
	}
}

//////////////////////////////////////////

bool ft::Poller::acceptIncoming(const ft::Server& server)
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
			pollfd& fd = this->pollfds[i];

			// Already in use, skip
			if (poll->fd != -1) continue;

			std::cout << GREEN << "Accepting incoming connection" << RESET << std::endl;

			fd->fd = clientSocket;
			fd->events = POLLIN;
			fd->revents = NONE;


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

//////////////////////////////////////////

void ft::Poller::pollInEvent(pollfd& fd, Connection& conn)
{
	ssize_t		brecv;		// Bytes received

	bzero(this->buffer, BUFF_SIZE); // Clear the buffer
	brecv = recv(fd.fd, this->buffer, BUFF_SIZE, NONE);
	if (brecv <= 0)
	{
		if (brecv < 0)
			std::cerr << RED << "Receive function has failed!" << RESET << std::endl;
		else
			std::cout << "Connection closed by peer" << std::endl;
		this->closeConnection(fd, conn);
	}
}

void ft::Poller::pollOutEvent(pollfd& fd, Connection& conn)
{

}

//////////////////////////////////////////

void ft::Poller::closeConnection(pollfd& fd, Connection& conn)
{
	close(fd.fd);
	fd.fd = -1;
}
