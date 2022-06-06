/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/02 12:34:20 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/06 10:09:13 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

ft::Server::Server(ft::ServerEntry& inConfig) : config(inConfig) { }

//////////////////////////////////////////

// TODO: Actually read the config, for now hardcode to test
void ft::Server::init(void)
{
	try
	{
		address = ft::SocketAddress(AF_INET, htons(8080), INADDR_ANY); // needs config values
		serverFD = ft::socket(IPV4, TCP, NONE);
		ft::setSocketOption(serverFD, SOL_SOCKET, SO_REUSEADDR, true, sizeof(int32_t)); // make kernel release socket after exit
		ft::bind(serverFD, &address);
		ft::listen(serverFD, 128);
		ft::fcntl(serverFD, F_SETFL, O_NONBLOCK);

		nfds = 0;
		maxClients = 5; // this should be gotten from config
		numFds = 1;
		pollfds = new pollfd[maxClients];
		pollfds->fd = serverFD;
		pollfds->events = POLLIN;
	}
	catch(const std::exception& e)
	{
		ft::exceptionExit(e, EXIT_FAILURE);
	}
}

void ft::Server::pollListen(void)
{
	std::cout << "\n//=/ ...Accepting connection... /=//\n" << std::endl;
	clientSocket = ft::accept(serverFD, &address);

	if (numFds == maxClients)
	{
		const char* error = "HTTP/1.1 503 Service Unavailabe\nContent-Type: text/plain\nContent-Length: 12\n\n503 error";
		char buffer[30000] = {0};
		ft::receive(pollfds->fd, buffer, 30000, 0);
		ft::send(clientSocket, error, strlen(error), 0); // send Response
		close(clientSocket); // End of Exchange
	}
	else
	{
		for (int j = 0; j < numFds; j++)
		{
			if ((pollfds + j)->fd == -1)
			{
				(pollfds + j)->fd = clientSocket;
				(pollfds + j)->events = POLLIN;
				(pollfds + j)->revents = 0;
				goto label;
			}
		}
		numFds++;
		(pollfds + numFds - 1)->fd = clientSocket;
		(pollfds + numFds - 1)->events = POLLIN;
		(pollfds + numFds - 1)->revents = 0;
	label:;
	}
}

void ft::Server::pollInEvent(int32_t i)
{
	char buffer[30000] = {0};
	ssize_t bytesrec = ft::receive((pollfds + i)->fd, buffer, 30000, 0);
	if (bytesrec == 0)
	{
		std::cout << "Connection closed\n";
		close((pollfds + i)->fd); // End of Exchange
		(pollfds + i)->fd = -1;
	}
	else
	{
		ft::Request req(buffer);
		//requests[(pollfds + i)->fd] = req; this doesnt work and im retarded
		req.display();
		(pollfds + i)->events = POLLOUT;
		std::cout << "//=/ Sent Response /=//" << std::endl;
	}
}

void ft::Server::pollOutEvent(int i)
{
	std::cout << "sending shit" << std::endl;
	//requests[(pollfds + i)->fd].display();
	ft::send((pollfds + i)->fd, hello, strlen(hello), 0); // send Response
	(pollfds + i)->events = POLLIN;
}

void ft::Server::run(void)
{
	nfds = numFds;
	ft::poll(pollfds, nfds, 0); //check our open fds for events

	for (int i = 0; i < numFds; i++)
	{
		if (!i && (pollfds[0].revents & POLLIN))
			this->pollListen();
		else
		{
			if ((pollfds + i)->revents & POLLIN) //pollfd is ready for reading
				this->pollInEvent(i);
			else if ((pollfds + i)->revents & POLLOUT) //pollfd is ready for writing
				this->pollOutEvent(i);
		}
	}
}

//////////////////////////////////////////
