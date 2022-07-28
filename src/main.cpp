/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 17:39:03 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/28 17:09:17 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <list>
#include <array>
#include "Utils.hpp"
#include "Poller.hpp"
#include "Server.hpp"
#include "GlobalConfig.hpp"

// curl --verbose http://localhost:8080/
// --header for custom headers ala "header:123"

/**
 * @brief Verify arguments.
 *
 * @param[in] argc Argument count.
 */
static void verifyArguments(const int32_t argc)
{
	// Check arguments
	if (argc != 2)
	{
		std::cerr << "\nWebserv: Invalid arguments\n" << std::endl;
		std::cerr << "Usage: ./webserv <Configuration Filepath>\n" << std::endl;
		exit(EXIT_FAILURE);
	}
}

/**
 * @brief
 *
 * @param[in] configPath path to a config file
 * @param[in] config reference to the server configuration
 * @param[in] servers reference to the vector list of servers
 */
static void setupServers(const std::string& configPath, ft::GlobalConfig& config, std::vector<ft::Server>& servers, ft::Poller& poller)
{
	// Handle Interrupt signals
	signal(SIGINT, [](int32_t)
	{
		std::cout << "\n\nWebserv: Signal catched, shutting down..." << std::endl;
		exit(EXIT_SUCCESS);
	});

	std::cout << BLACK << "Webserv: Reading config file" << RESET << std::endl;
	try { config.readFile(configPath); }
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	std::cout << BLACK << "Webserv: Creating servers" << RESET << std::endl;
	for (ft::ServerSection& serverSection : config.serverSections)
	{
		ft::Server server(serverSection);
		if (!server.init())
			exit(EXIT_FAILURE);
		servers.push_back(server);
	}

	//poller.setServerAmount(servers.size());
}

/**
 * Program entry point.
 *
 * @param[in] argc argument count
 * @param[in] argv argument values
 * @return Either EXIT_SUCCESS or EXIT_FAILURE.
 */
int32_t main(int32_t argc, const char* argv[])
{
	ft::GlobalConfig						config;	 			// Contains the entire configuration for the whole application/
	std::vector<ft::Server>					servers; 			// A vector list that contains all of the servers.
	ft::Poller								poller(servers);	// Poller to handle incoming and outgoing communication.

	verifyArguments(argc);

	std::cout << BLACK << "Webserv: Starting" << RESET << std::endl;
	setupServers(argv[1], config, servers, poller);

	std::cout << BLACK << "Webserv: Running" << RESET << std::endl;
	do { poller.pollAll(); usleep(420); } while (true);

	std::cout << "Webserv: Shutting down" << std::endl;
	return (EXIT_SUCCESS);
}
