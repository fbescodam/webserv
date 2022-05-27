/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 17:39:03 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/05/27 12:25:31 by pvan-dij      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Exchange/Request.hpp"
#include "Exchange/Response.hpp"
#include <poll.h>

#define CLIENT_BODY_SIZE 30000

void ft_exit(int lol)
{
	std::cout << "Bye Bye!" << std::endl;
	exit(EXIT_SUCCESS);
}

int main(int argc, char const *argv[])
{
	int32_t serverFD, clientSocket, ValRead;
	ft::SocketAddress Address(AF_INET, htons(8080), INADDR_ANY); // Internet Address on Port 8080 on Localhost

	signal(SIGINT, ft_exit);



	const char* hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";

	// NOTE: Maybe use signal to catch ctrl+c to properly exit ?
	// Setup a server class ?

	try // Create internet socket.
	{
        serverFD = ft::socket(IPV4, TCP, NONE);

		// Should make the kernel release socket resources
		ft::setSocketOption(serverFD, SOL_SOCKET, SO_REUSEADDR, true, sizeof(int32_t));
		ft::bind(serverFD, &Address);
		ft::listen(serverFD, 128);
		fcntl(serverFD, F_SETFL, O_NONBLOCK); //macos sucks so have to set serverfd to be nonblocking
	}
	catch(const ft::Exception& e)
	{
		ft::exceptionExit(e, EXIT_FAILURE);
	}
	

    nfds_t  nfds = 0;
	int maxClients = 5; //maximum allowed connections
	int numFds = 1;
	struct pollfd *pollfds;
    pollfds = (pollfd *)malloc(maxClients * sizeof(struct pollfd));
	pollfds->fd = serverFD;
	pollfds->events = POLLIN;

	std::map<int, std::string> requests;
	//////////////////////////////////

    while(true)
    {
		nfds = numFds;
		ft::poll(pollfds, nfds, 0); //check our open fds for events

		for (int i = 0; i < numFds; i++)
		{
			if (!i)
			{
				if (pollfds[0].revents & POLLIN)
				{
					std::cout << "\n//=/ ...Accepting connection... /=//\n" << std::endl;
					try
					{
						clientSocket = ft::accept(serverFD, &Address);

						if (numFds == maxClients)
						{
							const char* error = "HTTP/1.1 503 Service Unavailabe\nContent-Type: text/plain\nContent-Length: 12\n\n503 error";
							char buffer[30000] = {0};
							recv((pollfds + i)->fd, buffer, 30000, 0);
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
			}
			else
			{
				if ((pollfds + i)->revents & POLLIN) //pollfd is ready for reading operation
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
							ft::Request request(buffer);
							request.display();
							(pollfds + i)->events = POLLOUT;
							// ft::send((pollfds + i)->fd, hello, strlen(hello), 0); // send Response
							std::cout << "//=/ Sent Response /=//" << std::endl;
						}
					}
					catch(const ft::Exception& e)
					{
						ft::exceptionExit(e, EXIT_FAILURE);
					}						
				}
				else if ((pollfds + i)->revents & POLLOUT) //pollfd is ready for writing
				{
					std::cout << "sending shit" << std::endl;
					ft::send((pollfds + i)->fd, hello, strlen(hello), 0); // send Response
					(pollfds + i)->events = POLLIN;
				}
			}
	
		}

    }
	return (EXIT_SUCCESS);
}

// curl --verbose http://localhost:8080/
// --header for custom headers ala "header:123"