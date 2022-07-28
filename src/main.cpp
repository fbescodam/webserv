/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 17:39:03 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/28 13:05:36 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>
#include <GlobalConfig.hpp>

/**
 * Program entry point.
 *
 * @param[in] argc Argument count.
 * @param[in] argv Argument value.
 * @return Either EXIT_SUCCESS or EXIT_FAILURE.
 */
int32_t main(int32_t argc, const char* argv[])
{
	// Check arguments
	if (argc != 2)
	{
		std::cerr << "\nWebserv: Invalid arguments\n" << std::endl;
		std::cerr << "Usage: ./webserv <Configuration Filepath>\n" << std::endl;
		return (EXIT_FAILURE);
	}

	// Handle Interrupt signals
	signal(SIGINT, [](int32_t)
	{
		std::cout << "\n\nWebserv: Signal catched, shutting down ..." << std::endl;
		exit (EXIT_FAILURE);
	});

	std::cout << BLACK << "Webserv: Starting..." << RESET << std::endl;
	std::cout << BLACK << "Webserv: Reading config file" << RESET << std::endl;

	static ft::GlobalConfig config;
	try { config.readFile(argv[1]); }
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return (EXIT_FAILURE);
	}
	
	std::cout << BLACK << "Webserv: Creating servers" << RESET << std::endl;

	static std::vector<ft::Server> servers;
	for (ft::ServerSection& serverSection : config.serverSections)
	{
		ft::Server server(serverSection);
		servers.push_back((server.init(), server));
	}

	std::cout << BLACK << "Webserv: Running" << RESET << std::endl;
	while (true)
	{
		usleep(500); // NOTE: To reduce CPU Usage
		for (auto it = servers.begin(); it != servers.end(); it++)
		{
			try { it->run(); }
			catch(const std::exception& e)
			{
				// NOTE: If we reach this place we utterly failed!
				std::cerr << RED <<  "Webserv: Server " << it->config.getName() << " has crashed!" << std::endl;
				std::cerr << e.what() << RESET << std::endl;
				servers.erase(it);
			}	
		}
	}

	std::cout << "Webserv: Shutting down" << std::endl;
	return (EXIT_SUCCESS);
}

// curl --verbose http://localhost:8080/
// --header for custom headers ala "header:123"
