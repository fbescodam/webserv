/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/02 12:34:20 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/22 12:12:42 by lde-la-h      ########   odam.nl         */
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
		ft::listen(this->serverFD, MAX_CLIENTS);
		ft::fcntl(this->serverFD, F_SETFL, O_NONBLOCK);
	}
	catch(const std::exception& e)
	{
		std::cerr << "Webserv: Failed to create socket for server" << std::endl;
		ft::exceptionExit(e, EXIT_FAILURE);
	}

	this->nfds = 0;
	this->numFds = 1;

	try { this->pollfds = new pollfd[MAX_CLIENTS]; }
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
		this->clientIpv4[clientSocket] = ft::inet_ntop(this->address);
		return;
	}

	if (this->numFds >= MAX_CLIENTS)
		return ; //TODO: this needs to work for all servers not just this one
	this->pollfds[numFds].fd = clientSocket;
	this->pollfds[numFds].events = POLLIN;
	this->pollfds[numFds].revents = 0; // initialize event to 0 just to be safe
	this->timeout[this->pollfds[numFds].fd] = std::time(0);
	this->numFds++;

	this->clientIpv4[clientSocket] = ft::inet_ntop(this->address);
}

void ft::Server::generateOutStatus(pollfd *poll, int code)
{
	if (this->responses.find(poll->fd) != this->responses.end())
		delete this->responses[poll->fd];
	this->responses[poll->fd] = new ft::Response(code, &(this->config));
	poll->events = POLLOUT;
}

void ft::Server::pollInEvent(pollfd* poll)
{
	// Fetch the raw data

	ssize_t brecv; //brecvast
	const size_t BUFF_SIZE = 4096;
	char buff[4096] = {0};

	ft::Request* temp = NULL;
	if (this->responses.find(poll->fd) != this->responses.end())
		this->responses.erase(poll->fd); //delete this->responses[poll->fd];

	//receive bytes and store them in our request buffer, organized per connection(poll->fd)
	try 
	{
		brecv = ft::receive(poll->fd, buff, BUFF_SIZE, 0);
		if (brecv == 0) throw std::exception();
	}
	catch (std::exception &e)
	{
		this->req_buf.erase(poll->fd);
		this->timeout.erase(poll->fd);
		close(poll->fd);
		poll->fd = -1;
		return;
	}

	if (brecv > -1)
		this->timeout[poll->fd] = std::time(nullptr);
	this->req_buf[poll->fd] += buff;

	try // Parse the incoming request
	{
		temp = new ft::Request(this->req_buf[poll->fd], this->clientIpv4[poll->fd]);
		if (!temp->parse(std::stoul(*this->config.getValue("limit_body_size"))))
		{			
			delete temp;
			this->generateOutStatus(poll, 100); //TODO: something goes wrong here with big bodys through post
			return;
		}
	}
	catch (const ft::PayloadTooLarge& e)
	{
		delete temp;
		this->generateOutStatus(poll, 413);
		return;
	}
	catch (const ft::BadRequest& e)
	{
		delete temp;
		this->generateOutStatus(poll, 400);
		return;
	}
	catch (const std::bad_alloc& e)
	{
		this->generateOutStatus(poll, 507);
		return;
	}
	catch (const std::exception& e)
	{
		delete temp;
		this->generateOutStatus(poll, 500);
		return;
	}

	//TODO: 50/50 chance https throws a badrequest atm, its done in parse

	// Construct response on store them in response buffer
	this->responses[poll->fd] = new ft::Response(temp, &(this->config));

	if (this->responses[poll->fd]->verify())
		this->responses[poll->fd]->generateResponse();

	this->req_buf.erase(poll->fd);

	// Set poll to check for pollout events, this means we can send() to the fd because the client is ready
	poll->events = POLLOUT;
}

void ft::Server::resolveConnection(pollfd *poll)
{
	int temp = poll->fd;

	if (this->responses[temp]->fields["Connection"] == "keep-alive")
		poll->events = POLLIN;
	else
	{
		close(poll->fd);
		poll->fd = -1;
	}
	delete this->responses[temp];
	this->responses.erase(temp);
}

void ft::Server::pollOutEvent(pollfd* poll)
{
	// Event is happening so prevent timeout
	if (this->timeout.find(poll->fd) != this->timeout.end())
		this->timeout[poll->fd] = std::time(0);

	ft::ResponseStatus ret;
	if ((ret = this->responses[poll->fd]->send(poll->fd)) == ft::DONE)
	{
		this->resolveConnection(poll);
		std::cout << "//=/ Sent Response /=//" << std::endl;
	}
}

void ft::Server::cleanSocket(pollfd *poll)
{
	std::cout << "Connection closed-time out" << std::endl;

	this->timeout.erase(poll->fd);
	this->generateOutStatus(poll, 408);
	poll->events = POLLIN | POLLOUT;
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
