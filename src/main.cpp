/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/13 11:38:46 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/05/20 16:09:32 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include "Common.hpp"
#include "Status.hpp"
#include "../HTTP/exchange.hpp"

//set in config file
#define CLIENT_BODY_SIZE 30000

/**
 * @brief Entry point of the application.
 * 
 * @param argc Argument count.
 * @param argv Argument values.
 * @return Exit status.
 */
int32_t main(int32_t argc, const char* argv[]) {
    int32_t ServerFD, NewSocket, Valread;
    IntSockAddr_t Address;
    uint32_t Addrlen = sizeof(Address);
    
    // HTTP Content
    const char* hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";

    // Setup
    try
    {
        ServerFD = ft::Socket(ft::Domain::DM_IPV4, ft::Protocol::PR_TCP);
        {
            Address.sin_family = AF_INET;
            Address.sin_addr.s_addr = INADDR_ANY;
            Address.sin_port = htons(PORT);
        }
        std::memset(Address.sin_zero, '\0', sizeof(Address.sin_zero));
        
        ft::Bind(ServerFD, &Address, Addrlen);
        ft::Listen(ServerFD, 10);
    }
    catch(const ft::Exception& e)
    {
        ft::ExceptionExit(e);
    }

    // Stay open
    while(true)
    {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        try
        {
            NewSocket = ft::Accept(ServerFD, &Address, &Addrlen);

            char buffer[CLIENT_BODY_SIZE] = {0};
            Valread = read(NewSocket, buffer, CLIENT_BODY_SIZE);
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
