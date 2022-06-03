/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 17:39:03 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/05/31 14:04:44 by pvan-dij      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server/Server.hpp"
#include "Config/Config.hpp"

#define CLIENT_BODY_SIZE 30000

void ft_exit(int lol)
{
	std::cout << "Bye Bye!" << std::endl;
	exit(EXIT_SUCCESS);
}

int main(int argc, char const *argv[])
{
	signal(SIGINT, ft_exit);

	ft::Config config;
	ft::Server server(config);

	try
	{
		server.init();
		server.run();		
	}
	catch(const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}
	


	return (EXIT_SUCCESS);
}

// curl --verbose http://localhost:8080/
// --header for custom headers ala "header:123"
