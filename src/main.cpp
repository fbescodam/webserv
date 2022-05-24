/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 17:39:03 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/05/24 12:31:27 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Common.hpp"
#include "CommonNetwork.hpp"
#include <arpa/inet.h>

#define CLIENT_BODY_SIZE 30000

int main(int argc, char const *argv[])
{
	int32_t ServerFD, NewSocket, ValRead;
	ft::SocketAddress Address(AF_INET, htons(8080), INADDR_ANY); // Internet Address on Port 8080 on Localhost

	const char* hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";

	try
	{
        ServerFD = ft::Socket(IPV4, TCP, NULL);
		ft::Bind(ServerFD, &Address, Address.GetSize());
		ft::Listen(ServerFD, 128);
	}
	catch(const ft::Exception& e)
	{
		ft::ExceptionExit(e);
	}
	
	//////////////////////////////////

    while(true)
    {

		// NOTE: Use send and recv functions instead of read and write!

        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        try
        {
			uint32_t size = Address.GetSize();
            NewSocket = ft::Accept(ServerFD, &Address, &size);

            char buffer[CLIENT_BODY_SIZE] = {0};
            ValRead = read(NewSocket, buffer, CLIENT_BODY_SIZE);
            printf("%s\n", buffer);

            write(NewSocket, hello, strlen(hello));
            printf("------------------Hello message sent-------------------");
            close(NewSocket);
        }
        catch(const ft::Exception& e)
        {
            ft::ExceptionExit(e);
        }
    }
	return (EXIT_SUCCESS);
}
