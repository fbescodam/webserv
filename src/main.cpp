/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 17:39:03 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/07 14:47:20 by lde-la-h      ########   odam.nl         */
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
		std::cerr << "\nwebserv: Invalid arguments\n" << std::endl;
		std::cerr << "Usage: ./webserv <Configuration Filepath>\n" << std::endl;
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
	try { config.readFile(argv[1]); }
	catch (const std::exception& e)
	{
		ft::exceptionExit(e, EXIT_FAILURE);
	}

	// Debug printing
	for (ft::ServerSection& serverSection : config.serverSections)
		serverSection.printAll();

	static std::vector<ft::Server> servers;
	for (auto& serverSection : config.serverSections)
	{
		ft::Server server(serverSection);
		server.init();
		servers.push_back(server);
	}

	// Main loop
	while (true)
	for (auto it = servers.begin(); it != servers.end(); it)
	{
		try { (*it).run(); }
		catch(const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
			servers.erase(it);
			break;
		}
	}

	std::cout << "Webserv: Shutting down" << std::endl;
	return (EXIT_SUCCESS);
}

// curl --verbose http://localhost:8080/
// --header for custom headers ala "header:123"
