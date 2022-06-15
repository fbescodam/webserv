/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/02 12:34:20 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/15 10:44:37 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

ft::Server::Server(ft::ServerSection& inConfig) : config(inConfig) { }

//////////////////////////////////////////

// TODO: Actually read the config, for now hardcode to test
void ft::Server::init(void)
{
	try
	{
		this->address = ft::SocketAddress(AF_INET, htons(8080), INADDR_ANY); // needs config values
		this->serverFD = ft::socket(IPV4, TCP, NONE);
		ft::setSocketOption(this->serverFD, SOL_SOCKET, SO_REUSEADDR, true, sizeof(int32_t)); // make kernel release socket after exit
		ft::bind(this->serverFD, &this->address);
		ft::listen(this->serverFD, 128);
		ft::fcntl(this->serverFD, F_SETFL, O_NONBLOCK);
	}
	catch(const std::exception& e)
	{
		ft::exceptionExit(e, EXIT_FAILURE);
	}

	this->nfds = 0;
	this->maxClients = 5; // this should be gotten from config
	this->numFds = 1;

	try
	{
		this->pollfds = new pollfd[this->maxClients];
	}
	catch(const std::exception& e)
	{
		ft::exceptionExit(e, EXIT_FAILURE);
	}
	
	this->pollfds->fd = serverFD;
	this->pollfds->events = POLLIN;
}

void ft::Server::pollListen(void)
{
	std::cout << "\n//=/ ...Accepting connection... /=//\n" << std::endl;

	int32_t clientSocket = ft::accept(this->serverFD, &this->address);
	for (int j = 0; j < this->numFds; j++)
	{
		if (this->pollfds[j].fd == -1)
		{
			this->pollfds[j].fd = clientSocket;
			this->pollfds[j].events = POLLIN;
			this->pollfds[j].revents = 0;
			return;
		}
	}
	// TODO: What is the logic here ?
	this->pollfds[numFds].fd = clientSocket;
	this->pollfds[numFds].events = POLLIN;
	this->pollfds[numFds].revents = 0;
	this->numFds++;
}

void ft::Server::pollInEvent(int32_t i)
{
	char buffer[30000] = {0};
	ssize_t bytesrec = ft::receive(this->pollfds[i].fd, buffer, 30000, 0);
	if (bytesrec == 0)
	{
		std::cout << "Connection closed\n";
		close(this->pollfds[i].fd); // End of Exchange
		this->pollfds[i].fd = -1;
	}
	else
	{
		try
		{
			this->requests[this->pollfds[i].fd] = new ft::Request(buffer);
			this->requests[this->pollfds[i].fd]->display();
		}
		catch(const ft::InvalidCharException& e) // TODO: Implement this error for bad input
		{
			this->requests[this->pollfds[i].fd] = NULL;
			std::cout << e.what() << std::endl;
		}
		this->pollfds[i].events = POLLOUT;
	}
}

void ft::Server::pollOutEvent(int i)
{
	if (!this->requests[this->pollfds[i].fd])
	{
		auto res = ft::Response::getError(400);
		res.send(this->pollfds[i].fd);
		close(this->pollfds[i].fd);
	}
	else
	{
		ft::Response res(*this->requests[this->pollfds[i].fd]);
		res.send(this->pollfds[i].fd);
	}
	this->pollfds[i].events = POLLIN;
	std::cout << "//=/ Sent Response /=//" << std::endl;

}

void ft::Server::run(void)
{
	this->nfds = this->numFds; // TODO: Wtf?

	// Check our open fds for events
	ft::poll(this->pollfds, this->nfds, 0);
	for (int i = 0; i < this->numFds; i++)
	{
		// Pollfd is ready for listening
		if (!i && (this->pollfds[0].revents & POLLIN))
			this->pollListen();
		
		// Pollfd is ready for reading
		else if (this->pollfds[i].revents & POLLIN)
			this->pollInEvent(i);

		// Pollfd is ready for writing
		else if (this->pollfds[i].revents & POLLOUT)
			this->pollOutEvent(i);
	}
}

//////////////////////////////////////////
