/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/02 12:34:20 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/15 17:28:27 by pvan-dij      ########   odam.nl         */
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

void ft::Server::pollListen()
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
			this->timeout[this->pollfds[j].fd] = std::time(0);
			return;
		}
	}
	// TODO: What is the logic here ?
	this->pollfds[numFds].fd = clientSocket;
	this->pollfds[numFds].events = POLLIN;
	this->pollfds[numFds].revents = 0;
	this->timeout[this->pollfds[numFds].fd] = std::time(0);
	this->numFds++;
}

void ft::Server::pollInEvent(pollfd* poll)
{
	ssize_t bytesrec;
	std::string req;
	char buffer[30000] = {0};
	this->timeout[poll->fd] = std::time(0);

	while ((bytesrec = ft::receive(poll->fd, buffer, 30000, 0)) > 0)
		req += buffer;

	if (bytesrec == 0)
	{
		std::cout << "Connection closed-in\n";
		this->timeout.erase(poll->fd);
		close(poll->fd); // End of Exchange
		poll->fd = -1;
	}
	else
	{
		try
		{
			this->requests[poll->fd] = new ft::Request(req);
			this->requests[poll->fd]->display();
		}
		catch(const ft::InvalidCharException& e) // TODO: Implement this error for bad input
		{
			this->requests[poll->fd] = nullptr;
			std::cout << e.what() << std::endl;
		}
		poll->events = POLLOUT;
	}
}

void ft::Server::pollOutEvent(pollfd* poll)
{
	if (!this->requests[poll->fd])
	{
		auto res = ft::Response::getError(400);
		res.send(poll->fd);
		close(poll->fd);
	}
	else
	{
		ft::Response res(*this->requests[poll->fd]);
		res.send(poll->fd);
		delete this->requests[poll->fd];
	}
	poll->events = POLLIN;
	std::cout << "//=/ Sent Response /=//" << std::endl;

}

void ft::Server::cleanSocket(pollfd *poll)
{
	std::cout << "Connection closed-clean\n";
	this->timeout.erase(poll->fd);
	close(poll->fd);
	poll->fd = -1;
}

bool ft::Server::checkTimeout(pollfd *poll)
{
	return (this->timeout.find(poll->fd) != this->timeout.end() && \
	std::time(0) - this->timeout[poll->fd] > S_TIMEOUT);
}

void ft::Server::run(void)
{
	this->nfds = this->numFds; // TODO: Wtf?

	// Check our open fds for events
	ft::poll(this->pollfds, this->nfds, 0);
	for (int i = 0; i < this->numFds; i++)
	{
		pollfd* poll = &this->pollfds[i];
		if (poll->fd < 0)
			continue;
		// Pollfd is ready for listening
		if (!i && (this->pollfds[0].revents & POLLIN))
			this->pollListen();

		// Pollfd is ready for reading
		else if (poll->revents & POLLIN)
			this->pollInEvent(poll);

		// Pollfd is ready for writing
		else if (poll->revents & POLLOUT)
			this->pollOutEvent(poll);

		//connection timed out: 60 seconds
		if (i > 0 && this->checkTimeout(poll))
			this->cleanSocket(poll);
	}
}

//////////////////////////////////////////
