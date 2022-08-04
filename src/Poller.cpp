/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Poller.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/28 15:48:13 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/08/04 19:52:24 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Poller.hpp"

const ft::Socket* ft::Poller::createSocket(const uint16_t port)
{
	ft::Socket* socket = new ft::Socket();

	try
	{
		socket->fd = ft::socket(IPV4, TCP, NONE); // Create a new socket fd
		socket->addr = ft::SocketAddress(AF_INET, htons(port), INADDR_ANY); // Create a new address on the defined port

		ft::setSocketOption(socket->fd, SOL_SOCKET, SO_REUSEADDR, true, sizeof(int32_t)); // Make kernel release socket after exit
		ft::bind(socket->fd, &socket->addr); // Bind the socket to an address
		ft::listen(socket->fd, MAX_CLIENTS); // Allow the socket to listen, set the maximum amount of clients
		ft::fcntl(socket->fd, F_SETFL, O_NONBLOCK); // Set to non-blocking mode for use with poll

		this->sockets.push_back(socket); // Add (copy/move) the newly created socket to the list of sockets

		// Set up a new pollfd for this serversocket
		this->connections[this->reservedSocketAmount].poll = &this->pollfds[this->reservedSocketAmount];
		this->connections[this->reservedSocketAmount].poll->fd = socket->fd;
		this->connections[this->reservedSocketAmount].poll->events = POLLIN;
		this->reservedSocketAmount++;
	}
	catch(const std::exception& e)
	{
		std::cerr << RED << "Webserv: " << e.what() << RESET << std::endl;
		exit(EXIT_FAILURE);
	}

	return (socket);
}

//////////////////////////////////////////

ft::Poller::Poller(std::vector<ft::Server>& servers, const ft::GlobalConfig& globalConfig) : servers(servers), globalConfig(globalConfig)
{
	uint16_t				port;
	std::list<uint16_t>		ports;

	this->activeClients = 0;

	// Init connections TODO: Move to constructor
	for (ft::Connection& conn : this->connections)
	{
		conn.response = nullptr;
		conn.request = nullptr;
		conn.poll = nullptr;
		resetConnection(conn);
	}

	// Init pollfds
	for (pollfd& fd : this->pollfds)
	{
		fd.fd = -1;
		fd.events = POLLIN;
		fd.revents = NONE;
	}

	// First few pollfds are reserved for the server sockets, initialize them here
	// Only one server socket per defined port in the config
	this->reservedSocketAmount = 0;
	for (size_t i = 0; i < this->servers.size(); i++)
	{
		port = (uint16_t) this->servers[i].config.returnValueAsInt("listen");
		if (std::find(ports.begin(), ports.end(), port) == ports.end()) // port has no socket yet
		{
			const Socket* socket = this->createSocket(port);
			servers[i].setSocket(this->sockets.back()); // Link socket to server
		}
		else
			servers[i].setSocket(this->getSocketByPort(port)); // Socket already exists, link it to the server
	}
}

//////////////////////////////////////////

ft::Poller::~Poller()
{
	for (const ft::Socket* socket : this->sockets)
		delete socket;
	for (const ft::Connection& conn : this->connections)
	{
		delete conn.request;
		delete conn.response;
	}
}

//////////////////////////////////////////

void ft::Poller::pollAll(void)
{
	try { ft::poll(this->pollfds.data(), this->pollfds.size(), 100); }
	catch (GenericErrnoException& e)
	{
		std::cerr << RED << "Webserv: " << e.what() << RESET << std::endl;
		exit(EXIT_FAILURE);
	}

	// Check the result for all the pollers
	for (size_t i = 0; i < this->connections.size(); i++)
	{
		ft::Connection& conn = this->connections[i];
		pollfd* fd = conn.poll;
		if (!fd) continue; // No pollfd for this connection, assume it is not in use

		// First few polls are the listening thingies (one for each server), attaches new clients to a pollfd.
		// When a POLLIN event happens here, we need to specify which pollfd to use for this connection.
		if (i < this->servers.size() && (fd->revents & POLLIN))
			this->acceptIncoming(this->servers.at(i));
		else if (fd->revents & POLLIN) // Client connection ready for reading
			this->pollInEvent(conn);
		else if (fd->revents & POLLOUT) // Client connection ready for writing
			this->pollOutEvent(conn);
	}

	// Go over each connection and look for timeouts
	time_t now = std::time(nullptr);
	for (size_t i = this->reservedSocketAmount; i < MAX_CLIENTS; i++)
	{
		if (now - this->connections[i].lastActivity > CONN_TIMEOUT)
			this->closeConnection(this->connections.at(i));
	}
}

//////////////////////////////////////////

bool ft::Poller::acceptIncoming(const ft::Server& server)
{
	if (this->activeClients >= MAX_CLIENTS)
	{
		std::cout << RED << "Refusing incoming connection; max clients has been reached" << RESET << std::endl;
		return (false);
	}
	try
	{
		// Accept this connection.
		std::cout << GREEN << "Accepting incoming connection" << RESET << std::endl;
		const Socket *serverSocket = server.getSocket();
		// Assign new connection a clientSocket, which is connected to a server's socket
		ft::fd_t clientSocket = ft::accept(serverSocket->fd, const_cast<ft::SocketAddress*>(&serverSocket->addr));
		ft::fcntl(clientSocket, F_SETFL, O_NONBLOCK); // Set the clientSocket to non-blocking mode for use with poll

		// Find available pollfd
		pollfd* fd = nullptr;
		for (size_t i = this->servers.size() - 1; i < this->pollfds.size(); i++)
		{
			// Already in use, skip
			if (this->pollfds[i].fd != -1)
				continue;

			// Populate pollfd
			fd = &this->pollfds[i];
			fd->fd = clientSocket;
			fd->events = POLLIN;
			fd->revents = NONE;
			break;
		}

		// Double-check if a pollfd was actually found before assigning it a connection
		if (fd)
		{
			// Find available connection struct
			for (size_t i = this->servers.size() - 1; i < this->connections.size(); i++)
			{
				// Already in use, skip
				if (this->connections[i].poll)
					continue;

				// Populate the connection struct
				this->connections[i].poll = fd;
				this->connections[i].lastActivity = std::time(nullptr);
				this->connections[i].server = nullptr; // Don't know yet
				this->connections[i].ipv4 = ft::inet_ntop(*const_cast<ft::SocketAddress*>(&server.getSocket()->addr));

				// Increment the active connection count
				this->activeClients++;
				return (true);
			}
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << RED << e.what() << RESET << '\n';
		return (false);
	}

	// This should never happen
	std::cout << RED << "Refusing incoming connection; no available pollfd or connection struct found" << RESET << std::endl;
	return (false);
}

//////////////////////////////////////////

void ft::Poller::pollInEvent(ft::Connection& conn)
{
	ssize_t		brecv;		// Bytes received

	// Clear the current response, as a request is now being made.
	delete conn.response;
	conn.response = nullptr;
	if (!conn.request)
		conn.request = new Request(this->servers);

	bzero(this->buffer, BUFF_SIZE); // Clear the buffer
	brecv = recv(conn.poll->fd, this->buffer, BUFF_SIZE, NONE);
	if (brecv <= 0)
	{
		if (brecv < 0)
			std::cerr << RED << "Receive function has failed!" << RESET << std::endl;
		else
			std::cout << BLACK << "Connection closed by peer" << RESET << std::endl;
		this->closeConnection(conn);
		return;
	}
	if (conn.request->appendBuffer(this->buffer) == ft::Exchange::Status::DONE)
	{
		std::cout << BLACK << "[DEBUG] Buffer is complete!" << RESET << std::endl;

		// Once we have the full request, parse the header and check which server.
		// Pass connection to server for it to parse the request and generate an appropriate response.
		try
		{
			conn.request->parseHeader(conn);
			conn.server->handleRequest(conn);
		}
		catch (const ft::BadRequest& e)
		{
			conn.response = new ft::Response(conn);
			conn.response->generateStatus(400);
		}
		catch (const ft::PayloadTooLarge& e)
		{
			conn.response = new ft::Response(conn);
			conn.response->generateStatus(413);
		}
		catch (const ft::NotImplemented& e)
		{
			conn.response = new ft::Response(conn);
			conn.response->generateStatus(501);
		}
		catch (const std::exception& e)
		{
			std::cerr << RED << e.what() << RESET << std::endl;
			conn.response = new ft::Response(conn);
			conn.response->generateStatus(500);
		}
	}
	else
	{
		std::cout << BLACK << "[DEBUG] Buffer is incomplete." << RESET << std::endl;
		conn.response = new ft::Response(conn);
		conn.response->generateStatus(100);
	}
	std::cout << "Pollout now" << std::endl;
	conn.poll->events = POLLOUT;
	conn.lastActivity = std::time(nullptr);
}

//////////////////////////////////////////

void ft::Poller::pollOutEvent(ft::Connection& conn)
{
	conn.lastActivity = std::time(nullptr);

	// WTFFFFFF
	if (!conn.response)
		std::cout << RED << "Warning: response points to nothing in a PollOutEvent" << RESET << std::endl;
	if (!conn.response->sendRes || (conn.response->*(conn.response->sendRes))(conn.poll->fd) == ft::Response::Status::DONE)
		this->resolveConnection(conn);
	else
		std::cout << "Partially sent a response" << std::endl;
}

//////////////////////////////////////////

void ft::Poller::resolveConnection(ft::Connection& conn)
{
	std::cout << GREEN << "Connection resolved, response has been sent " << RESET << std::endl;
	if (!conn.response && conn.response->headers["Connection"] == "keep-alive")
	{
		this->deleteReqRes(conn); // Clear the request and response to be ready for the next request
		conn.poll->events = POLLIN;
		conn.poll->revents = NONE;
		std::cout << "Connection kept alive" << std::endl;
		return;
	}
	std::cout << "Connection closed" << std::endl;
	this->closeConnection(conn);
}

//////////////////////////////////////////

void ft::Poller::closeConnection(ft::Connection& conn)
{
	if (conn.poll) // Double-check if a pollfd exists in this connection. If not, you actually want to call resetConnection(conn).
	{
		close(conn.poll->fd);
		conn.poll->fd = -1;
		conn.poll->events = POLLIN | POLLOUT;
		conn.poll->revents = NONE;
	}
	this->resetConnection(conn);
}

//////////////////////////////////////////

void ft::Poller::deleteReqRes(ft::Connection& conn)
{
	delete conn.request;
	conn.request = nullptr;
	delete conn.response;
	conn.response = nullptr;
}

//////////////////////////////////////////

void ft::Poller::resetConnection(ft::Connection& conn)
{
	this->deleteReqRes(conn);
	conn.lastActivity = NONE;
	conn.server = nullptr;
	conn.ipv4 = "";
	if (conn.poll)
		conn.poll = nullptr;
}

//////////////////////////////////////////

const ft::Socket* ft::Poller::getSocketByPort(const uint16_t port) const
{
	for(ft::Socket* socket : this->sockets)
	{
		if (ntohs(socket->addr.port) == port)
			return (socket);
	}
	return (nullptr);
}
