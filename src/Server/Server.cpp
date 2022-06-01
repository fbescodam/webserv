#include "Server.hpp"

ft::Server::Server(Config& config)
{
	this->_config = config;
}

void ft::Server::init(void)
{
	try
	{
		Address = ft::SocketAddress(AF_INET, htons(8080), INADDR_ANY); // needs config values
		serverFD = ft::socket(IPV4, TCP, NONE);
		ft::setSocketOption(serverFD, SOL_SOCKET, SO_REUSEADDR, true, sizeof(bool)); // make kernel release socket after exit
		ft::bind(serverFD, &Address);
		ft::listen(serverFD, 128);
		fcntl(serverFD, F_SETFL, O_NONBLOCK); // macos sucks so have to set serverfd to be nonblocking
	}
	catch(const ft::Exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	
	nfds = 0;
	maxClients = 5; // this should be gotten from config
	numFds = 1;
	pollfds = new pollfd[maxClients];
	pollfds->fd = serverFD;
	pollfds->events = POLLIN;

}

//TODO: proper error responses
void ft::Server::newSocket(void)
{
	std::cout << "\n//=/ ...Accepting connection... /=//\n" << std::endl;
	try
	{
		clientSocket = ft::accept(serverFD, &Address);

		if (numFds == maxClients)
		{
			const char* error = "HTTP/1.1 503 Service Unavailabe\nContent-Type: text/plain\nContent-Length: 12\n\n503 error";
			char buffer[30000] = {0};
			recv(pollfds->fd, buffer, 30000, 0);
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
		label: ;
		}
	}
	catch(const ft::Exception& e)
	{
		ft::exceptionExit(e, EXIT_FAILURE);
	}
}

//TODO: implement proper request parsing
void ft::Server::pollInEvent(int i)
{
	try
	{
		char buffer[30000] = {0};
		ssize_t bytesrec = recv((pollfds + i)->fd, buffer, 30000, 0);
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
	catch(const ft::Exception& e)
	{
		ft::exceptionExit(e, EXIT_FAILURE);
	}
}

//TODO: implement proper response
void ft::Server::pollOutEvent(int i)
{
	std::cout << "sending shit" << std::endl;
	//requests[(pollfds + i)->fd].display();
	ft::send((pollfds + i)->fd, hello, strlen(hello), 0); // send Response
	(pollfds + i)->events = POLLIN;
}

void ft::Server::run(void)
{
    while(true)
    {
		nfds = numFds;
		ft::poll(pollfds, nfds, 0); //check our open fds for events

		for (int i = 0; i < numFds; i++)
		{
			if (!i)
			{
				if (pollfds[0].revents & POLLIN)
					this->newSocket();
			}
			else
			{
				if ((pollfds + i)->revents & POLLIN) //pollfd is ready for reading
					this->pollInEvent(i);
				else if ((pollfds + i)->revents & POLLOUT) //pollfd is ready for writing
					this->pollOutEvent(i);
			}
		}
    }
}
