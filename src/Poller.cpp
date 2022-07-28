/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Poller.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/28 15:48:13 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/28 18:08:11 by lde-la-h      ########   odam.nl         */
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
	struct { bool operator()(pollfd& a, pollfd& b) const { return a.fd > 0; } } sortPoll;
	std::sort(this->pollfds.begin(), this->pollfds.end(), sortPoll);

	// struct { bool operator()(pollfd& a, pollfd& b) const { return a.fd > 0; } } sortConn;
	// std::sort(this->pollfds.begin(), this->pollfds.end(), sortConn)

	// Go over each connection and look for timeouts
	time_t now = std::time(nullptr);
	for (size_t i = 0; i < MAX_CLIENTS; i++)
	{
		if (now - this->connections[i].lastActivity > 30000) // TODO: define timeout in config file
			this->closeConnection(this->pollfds.at(i), this->connections.at(i));
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
		// Accept this connection.
		ft::fd_t clientSocket = ft::accept(server.getSocket(), &server.getAddress());
		ft::fcntl(clientSocket, F_SETFL, O_NONBLOCK); // Set the clientSocket to non-blocking mode for use with poll

		// Find available file descriptor
		for (ft::fd_t i = this->servers.size() - 1; i < this->pollfds.size(); i++)
		{
			// Already in use, skip
			if (this->pollfds[i].fd != -1) continue;

			std::cout << GREEN << "Accepting incoming connection" << RESET << std::endl;

			// Populate pollfd
			this->pollfds[i].fd = clientSocket;
			this->pollfds[i].events = POLLIN;
			this->pollfds[i].revents = NONE;

			// Populate connection struct
			this->connections[i].lastActivity = std::time(nullptr);
			this->connections[i].server = nullptr; // don't know yet
			this->connections[i].ipv4 = ft::inet_ntop(server.getAddress());

			// Increment the active connection count
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
		return;
	}
	conn.lastActivity = std::time(nullptr);
}

//////////////////////////////////////////

void ft::Poller::pollOutEvent(pollfd& fd, Connection& conn)
{
	conn.lastActivity = std::time(nullptr);
}

//////////////////////////////////////////

void ft::Poller::closeConnection(pollfd& fd, Connection& conn)
{
	close(fd.fd);
	fd.fd = -1;
	this->resetConnection(conn);
}

//////////////////////////////////////////

void ft::Poller::resetConnection(Connection& conn)
{
	conn.lastActivity = NONE;
	conn.server = nullptr;
	conn.ipv4 = nullptr;
}