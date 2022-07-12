/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/02 12:34:20 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/12 20:55:41 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

ft::Server::Server(ft::ServerSection& inConfig) : config(inConfig) { }

//////////////////////////////////////////

void ft::Server::init(void)
{
	try
	{
		this->config.getValueAsList("server_names", serverNames);
		this->address = ft::SocketAddress(AF_INET, htons(this->config.returnValueAsInt("listen")), INADDR_ANY); // needs config values
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
	this->maxClients = 120; // this should be gotten from config
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
	ft::fcntl(clientSocket, F_SETFL, O_NONBLOCK);
	for (int i = 0; i < this->numFds; i++)
	{
		pollfd* poll = &this->pollfds[i];

		if (poll->fd != -1)
			continue;

		poll->fd = clientSocket;
		poll->events = POLLIN;
		poll->revents = 0;
		this->timeout[poll->fd] = std::time(0);
		return;
	}
	this->pollfds[numFds].fd = clientSocket;
	this->pollfds[numFds].events = POLLIN;
	this->pollfds[numFds].revents = 0;
	this->timeout[this->pollfds[numFds].fd] = std::time(0);
	
	this->numFds++;
}


/*
	ssize_t bytesrec;
	char buffer[BUFF_SIZE] = {0};
	this->timeout[poll->fd] = std::time(0);
	
	bytesrec = ft::receive(poll->fd, buffer, BUFF_SIZE, 0);
	
	this->req_buf[poll->fd] += buffer;
	if (bytesrec == BUFF_SIZE) //we assume there is more data waiting
		return ; //TODO: 100 continue?
	try
	{
		if (this->req_buf[poll->fd].size() == 0)
			throw ft::BadRequest();			
		ft::Request temp(this->req_buf[poll->fd]);
		this->responses[poll->fd] = new ft::Response(temp, &(this->config));
		this->responses[poll->fd]->verify();
		this->req_buf.erase(poll->fd);
		// this->requests[poll->fd]->display();
	}
	catch(const ft::BadRequest &e)
	{
		this->responses[poll->fd] = ft::Response::getErrorPointer(400);
		this->req_buf.erase(poll->fd);
		std::cout << e.what() << std::endl;
	}
	poll->events = POLLOUT;
*/

#define BUFF_SIZE 4096
void ft::Server::pollInEvent(pollfd* poll)
{
	ssize_t brecv; //brecvast
	char buff[BUFF_SIZE] = {0};
	this->timeout[poll->fd] = std::time(nullptr);

	//receive bytes and store them in our request buffer, organized per connection(poll->fd)
	brecv = ft::receive(poll->fd, buff, BUFF_SIZE, 0);
	this->req_buf[poll->fd] += buff;

	//assume more data is coming, send 100 continue
	if (brecv == BUFF_SIZE)
	{
		ft::Response cont(100, &(this->config));
		cont.send(poll->fd);
		return;
	}

	//construct response on store them in response buffer
	ft::Request temp(this->req_buf[poll->fd]);
	this->responses[poll->fd] = new ft::Response(temp, &(this->config));
	if (this->responses[poll->fd]->verify())
		this->responses[poll->fd]->generateResponse();
	this->req_buf.erase(poll->fd);

	//set poll to check for pollout events, this means we can send() to the fd because the client is ready
	poll->events = POLLOUT;
}

void ft::Server::resolveConnection(pollfd *poll)
{
	if (this->responses[poll->fd]->fields["Connection"] == "keep-alive")
	{
		delete this->responses[poll->fd];	
		poll->events = POLLIN;
	}
	else
	{
		int temp = poll->fd;
		close(poll->fd);
		poll->fd = -1;
		delete this->responses[temp];
	}
}

void ft::Server::pollOutEvent(pollfd* poll)
{
	ft::ResponseStatus ret = this->responses[poll->fd]->send(poll->fd);
	if (ret == ft::DONE)
	{
		this->resolveConnection(poll);
		std::cout << "//=/ Sent Response /=//" << std::endl;	
	}
	this->timeout[poll->fd] = std::time(0);
}

//TODO: 408 request timeout
void ft::Server::cleanSocket(pollfd *poll)
{
	std::cout << "Connection closed-time out" << std::endl;

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
	this->nfds = this->numFds;

	// Check our open fds for events
	ft::poll(this->pollfds, this->nfds, 0);
	for (int i = 0; i < this->numFds; i++)
	{
		pollfd* poll = &this->pollfds[i];

		if (poll->fd < 0)
			continue;

		// Create new connection
		if (!i && (this->pollfds[0].revents & POLLIN))
			this->pollListen();

		// Pollfd is ready for reading
		else if (poll->revents & POLLIN)
			this->pollInEvent(poll);

		// Pollfd is ready for writing
		else if (poll->revents & POLLOUT)
			this->pollOutEvent(poll);

		//connection timed out: 5 seconds
		if (i > 0 && this->checkTimeout(poll))
			this->cleanSocket(poll);
	}
}

//////////////////////////////////////////
