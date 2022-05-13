/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/13 11:38:46 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/05/13 14:54:21 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include "Socket.hpp"
#include "Common.hpp"

#define PORT 8080

enum class Banana
{
    Yellow
};

/**
 * @brief Entry point of the application.
 * 
 * @param argc Argument count.
 * @param argv Argument values.
 * @return Exit status.
 */
int32_t main(int32_t argc, const char* argv[])
{

    int32_t ServerFD, NewSocket, Valread;
    IntSockAddr_t Address;
    const size_t Addrlen = sizeof(Address);
    
    // HTTP Content
    const char* hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";

    // Setup
    try
    {
        ServerFD = ft::Socket(ft::Domain::DM_IPV6, ft::Protocol::PR_TCP, 0);
        {
            Address.sin_family = AF_INET;
            Address.sin_addr.s_addr = INADDR_ANY;
            Address.sin_port = htons(PORT);
        }
        std::memset(Address.sin_zero, '\0', sizeof(Address.sin_zero));
        
        ft::Bind(ServerFD, &Address, Addrlen);
        ft::Listen(ServerFD, 10);
    }
    catch(const std::exception& e)
    {
        std::cerr << "Webserv: " << e.what() << std::endl;
    }

    // Stay open
    while(true)
    {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        try
        {
            ft::Accept(ServerFD, &Address, &Addrlen);

            char buffer[30000] = {0};
            Valread = read(NewSocket, buffer, 30000);
            printf("%s\n", buffer);

            write(NewSocket, hello, strlen(hello));
            printf("------------------Hello message sent-------------------");
            close(NewSocket);
        }
        catch(const std::exception& e)
        {
            std::cerr << "Webserv: " << e.what() << std::endl;
        }
    }
    return (EXIT_SUCCESS);
}
