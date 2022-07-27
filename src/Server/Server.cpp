/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/27 11:08:42 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/27 18:53:56 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

ft::Server::Server(ft::ServerSection& inConfig) : config(inConfig) { }

//////////////////////////////////////////

void ft::Server::init(void)
{
	try
	{
		//this->config.getValueAsList("server_names", serverNames);
		this->address = ft::SocketAddress(AF_INET, htons(this->config.returnValueAsInt("listen")), INADDR_ANY); // needs config values
		this->serverFD = ft::socket(IPV4, TCP, NONE);
		
		ft::setSocketOption(this->serverFD, SOL_SOCKET, SO_REUSEADDR, true, sizeof(int32_t)); // make kernel release socket after exit
		ft::bind(this->serverFD, &this->address);
		ft::listen(this->serverFD, MAX_CLIENTS);
		ft::fcntl(this->serverFD, F_SETFL, O_NONBLOCK);
	}
	catch(const std::exception& e)
	{
		std::cerr << "Webserv: Failed to create socket for server" << std::endl;
        exit(EXIT_FAILURE);
	}

	this->nfds = 0;
	try { this->pollfds = new pollfd[MAX_CLIENTS]; }
	catch(const std::exception& e)
	{
		std::cerr << "Webserv: Failed to create poll for server." << std::endl;
        exit(EXIT_FAILURE);
	}

	this->pollfds->fd = serverFD;
	this->pollfds->events = POLLIN;
}

void ft::Server::respondWithStatus(pollfd* poll, int32_t statusCode)
{
	// if (this->responses.find(poll->fd) != this->responses.end())
	// 	delete this->responses[poll->fd];
		
	// this->responses[poll->fd] = new ft::Response(code, &(this->config));
	// std::cout << "Pollout now" << std::endl;
	// poll->events = POLLOUT;
}

static void pollListen(void)
{

}

static void pollInEvent(pollfd* poll)
{

}

static void pollOutEvent(pollfd* poll)
{

}

void ft::Server::run(void)
{
	// Check our open fds for events.
	ft::poll(this->pollfds, this->nfds, 0);

	for (int i = 0; i < this->nfds; i++)
	{
		pollfd* poll = &this->pollfds[i];

		// Create new connection
		if (!i && (this->pollfds[0].revents & POLLIN))
			pollListen();
		// Pollfd is ready for reading.
		else if (poll->revents & POLLIN)
			pollInEvent(poll);
		// Pollfd is ready for writing.
		else if (poll->revents & POLLOUT)
			pollOutEvent(poll);
	}
}
