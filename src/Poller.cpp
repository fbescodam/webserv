/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Poller.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/28 15:48:13 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/31 14:42:31 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Poller.hpp"

const ft::Poller::Socket* ft::Poller::createSocket(const uint16_t port)
{
	try
	{
		ft::Poller::Socket* socket = new ft::Poller::Socket();
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
}

//////////////////////////////////////////

ft::Poller::Poller(std::vector<ft::Server>& servers, const ft::GlobalConfig& globalConfig) : servers(servers), globalConfig(globalConfig)
{
	std::list<uint16_t>		ports;
	uint16_t				port;

	// First few pollfds are reserved for the server sockets, initialize them here
	// Only one server socket per defined port in the config
	this->reservedSocketAmount = 0;
	for (size_t i = 0; i < this->servers.size(); i++)
	{
		port = (uint16_t) this->servers[i].config.returnValueAsInt("listen");
		if (std::find(ports.begin(), ports.end(), port) == ports.end()) // port has no socket yet
		{
			const Socket* socket = this->createSocket(port);

			// Link socket to server
			servers[i].setSocket(this->sockets.back());


		}
		else
			servers[i].setSocket(this->getSocketByPort(port)); // Socket already exists, link it to the server
	}
}

//////////////////////////////////////////

void ft::Poller::pollAll(void)
{
	try { ft::poll(this->pollfds.data(), this->pollfds.size(), -1); }
	catch (GenericErrnoException& e)
	{
		std::cerr << RED << "Webserv: " << e.what() << RESET << std::endl;
		exit(EXIT_FAILURE);
	}

	// Check the result for all the pollers
	for (size_t i = 0; i < this->connections.size(); i++)
	{
		pollfd* fd = this->connections[i].poll;
		if (!fd)
			continue; // No pollfd for this connection, assume it is not in use
		Connection& conn = this->connections[i];

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
	for (size_t i = 0; i < MAX_CLIENTS; i++)
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
			if (this->pollfds[i].fd != -1) continue;

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
				if (this->connections[i].poll) continue;

				// Populate the connection struct
				this->connections[i].poll = fd;
				this->connections[i].lastActivity = std::time(nullptr);
				this->connections[i].server = nullptr; // don't know yet
				this->connections[i].ipv4 = ft::inet_ntop(server.getAddress());

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

void ft::Poller::pollInEvent(Connection& conn)
{
	ssize_t		brecv;		// Bytes received

	bzero(this->buffer, BUFF_SIZE); // Clear the buffer
	brecv = recv(conn.poll->fd, this->buffer, BUFF_SIZE, NONE);
	if (brecv <= 0)
	{
		if (brecv < 0)
			std::cerr << RED << "Receive function has failed!" << RESET << std::endl;
		else
			std::cout << "Connection closed by peer" << std::endl;
		this->closeConnection(conn);
		return;
	}
	conn.lastActivity = std::time(nullptr);
}

//////////////////////////////////////////

void ft::Poller::pollOutEvent(Connection& conn)
{
	conn.lastActivity = std::time(nullptr);
}

//////////////////////////////////////////

void ft::Poller::closeConnection(Connection& conn)
{
	if (conn.poll) // Double-check if a pollfd exists in this connection. If not, you actually want to call resetConnection(conn).
	{
		close(conn.poll->fd);
		conn.poll->fd = -1;
	}
	this->resetConnection(conn);
}

//////////////////////////////////////////

void ft::Poller::resetConnection(Connection& conn)
{
	conn.lastActivity = NONE;
	conn.server = nullptr;
	conn.ipv4 = nullptr;
}

//////////////////////////////////////////

const ft::Poller::Socket* ft::Poller::getSocketByPort(const uint16_t port) const
{
	for(Socket *socket : this->sockets)
	{
		if (socket->addr.port == port)
			return (socket);
	}
}
