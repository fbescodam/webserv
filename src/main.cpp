/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 17:39:03 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/05/27 14:21:14 by pvan-dij      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server/Server.hpp"

#define CLIENT_BODY_SIZE 30000

void ft_exit(int lol)
{
	std::cout << "Bye Bye!" << std::endl;
	exit(EXIT_SUCCESS);
}

int main(int argc, char const *argv[])
{
	signal(SIGINT, ft_exit);

	ft::Server server;

	server.init();
	server.run();

	return (EXIT_SUCCESS);
}

// curl --verbose http://localhost:8080/
// --header for custom headers ala "header:123"