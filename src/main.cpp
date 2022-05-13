/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/13 11:38:46 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/05/13 14:20:25 by lde-la-h      ########   odam.nl         */
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

    int server_fd, new_socket; long valread;
    IntSockAddr_t address;
    int addrlen = sizeof(address);
    
    // Only this line has been changed. Everything is same.
    const char* hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";

    try
    {
        server_fd = ft::Socket(ft::Domain::DM_IPV4, ft::Protocol::PR_TCP, 0);
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);

        std::memset(address.sin_zero, '\0', sizeof(address.sin_zero));
        
        ft::Bind(server_fd, &address, sizeof(address));
        ft::Listen(server_fd, 10);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    while(1)
    {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        
        char buffer[30000] = {0};
        valread = read( new_socket , buffer, 30000);
        printf("%s\n",buffer );
        write(new_socket , hello , strlen(hello));
        printf("------------------Hello message sent-------------------");
        close(new_socket);
    }
}
