/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 17:39:03 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/16 22:03:13 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server/Server.hpp"
#include "Config/GlobalConfig.hpp"

/**
 * Program entry point.
 *
 * @param argc Argument count.
 * @param argv Argument value.
 * @return Either EXIT_SUCCESS or EXIT_FAILURE
 */
int32_t main(int32_t argc, const char* argv[])
{
	// Check arguments
	if (argc != 2)
	{
		std::cerr << "\nWebserv: Invalid arguments\n" << std::endl;
		std::cerr << "Usage: ./Webserv <Configuration Filepath>\n" << std::endl;
		return (EXIT_FAILURE);
	}

	// Handle Interrupt signals
	signal(SIGINT, [](int32_t)
	{
		std::cout << "\n\nWebserv: Signal catched, shutting down" << std::endl;
		exit (EXIT_FAILURE); // We need to exit as we are in a lambda.
	});

	// Read config file ...
	ft::GlobalConfig config;
	try
	{
		config.readFile(argv[1]);
	}
	catch (const ft::DelimiterNotFoundException& e)
	{
		ft::exceptionExit(e, EXIT_FAILURE);
		return (false);
	}

	// Instead the config should have a method, start servers that inits and runs all the servers.
	// TODO: Should each server run in a for loop or instead in a thread.

	ft::Server server(config.serverSections[0]);
	server.init();

	// Main loop
	try
	{
		while (true)
			server.run();
	}
	catch(const std::exception& e)
	{
		// TODO: Don't actually die
		ft::exceptionExit(e, EXIT_FAILURE);
	}

	std::cout << "Webserv: Shutting down" << std::endl;
	return (EXIT_SUCCESS);
}

// curl --verbose http://localhost:8080/
// --header for custom headers ala "header:123"
