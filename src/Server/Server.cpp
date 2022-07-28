/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/27 11:08:42 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/28 12:50:29 by lde-la-h      ########   odam.nl         */
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
		
		// Make kernel release socket after exit
		ft::setSocketOption(this->serverFD, SOL_SOCKET, SO_REUSEADDR, true, sizeof(int32_t));
		ft::bind(this->serverFD, &this->address);
		ft::listen(this->serverFD, MAX_CLIENTS);
		ft::fcntl(this->serverFD, F_SETFL, O_NONBLOCK);
	}
	catch(const std::exception& e)
	{
		std::cerr << "Webserv: Failed to create socket for server" << std::endl;
		std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE); // TODO: What do here
	}

	this->nfds = 0;
	try { this->pollfds = new pollfd[MAX_CLIENTS]; }
	catch(const std::exception& e)
	{
		std::cerr << "Webserv: Failed to create poll for server." << std::endl;
		std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE); // TODO: What do here
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

//////////////////////////////////////////

/**
 * @brief 
 * 
 */
void ft::Server::pollListen(void)
{
	std::cout << GREEN << "Accepting Conneciton" << RESET << std::endl;

	// Accept a connection and set it to nonblocking mode.
	int32_t clientSocket = ft::accept(this->serverFD, &this->address);
	ft::fcntl(clientSocket, F_SETFL, O_NONBLOCK);

	// Find available file descriptor
	for (nfds_t i = 0; i < this->nfds; i++)
	{
		pollfd* poll = &this->pollfds[i];

		// Already used
		if (poll->fd != -1) continue;

		poll->fd = clientSocket;
		poll->events = POLLIN;
		poll->revents = NONE;

		// TODO: Set time out
		// TODO: Get IPV4 Address

		return;
	}

	// TODO: Else ?
}
/**
 * @brief To receive 
 * 
 * @param poll 
 */
void ft::Server::pollInEvent(pollfd* poll)
{
	std::cout << BLACK << "Incoming Request" << RESET << std::endl;

	ssize_t brecv;							// Bytes received
	const size_t BUFF_SIZE = 4096;			// Buffer size
	static char buffer[BUFF_SIZE] = {0};	// The buffer

	// Receive the incoming message
	if ((brecv = recv(poll->fd, buffer, BUFF_SIZE, NONE)) <= 0)
	{
		if (brecv == NONE)
			std::cerr << BLACK << "Connection closed by client" << RESET << std::endl;
		else
			std::cerr << RED << "Receive function has failed!" << RESET << std::endl;

		close(poll->fd);
		poll->fd = -1;
		return;
	}

	// Parse the incoming request
	try
	{
		// TODO: Parse HTTP request
	}
	catch (const ft::PayloadTooLarge& e)
	{
		this->respondWithStatus(poll, 413);
		return;
	}
	catch (const ft::BadRequest& e)
	{
		this->respondWithStatus(poll, 400);
		return;
	}
	catch (const std::bad_alloc& e)
	{
		std::cerr << RED << "Failed to allocate memory" << RESET << std::endl;
		this->respondWithStatus(poll, 507);
		return;
	}
	catch (const std::exception& e)
	{
		std::cerr << RED << "Something went wrong while parsing request" << RESET << std::endl;
		this->respondWithStatus(poll, 500);
		return;
	}
	
	poll->events = POLLOUT;
}

/**
 * @brief 
 * 
 * @param poll 
 */
void ft::Server::pollOutEvent(pollfd* poll)
{
	// TODO: Prevent timeout!
	// TODO: Use response class to send response.

	std::cout << "\033[30;1m" << "Sent Response" << "\033[0m" << std::endl;
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
			this->pollListen();
		// Pollfd is ready for reading.
		else if (poll->revents & POLLIN)
			this->pollInEvent(poll);
		// Pollfd is ready for writing.
		else if (poll->revents & POLLOUT)
			this->pollOutEvent(poll);
	}
}
