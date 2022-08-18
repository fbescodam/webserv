/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 17:39:03 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/08/18 20:49:06 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <list>
#include <array>
#include <termios.h>
#include "Utils.hpp"
#include "Poller.hpp"
#include "Server.hpp"
#include "GlobalConfig.hpp"

// curl --verbose http://localhost:8080/
// --header for custom headers ala "header:123"

/**
 * @brief
 *
 * @param[in] configPath path to a config file
 * @param[in] config reference to the server configuration
 * @param[in] servers reference to the vector list of servers
 */
static void setupServers(const std::string& configPath, ft::GlobalConfig& config, std::vector<ft::Server>& servers)
{
	// Handle Interrupt signals
	signal(SIGINT, [](int32_t)
	{
		std::cout << "\n\nWebserv: Signal catched, shutting down..." << std::endl;
		exit(EXIT_SUCCESS);
	});
	
	LOG("Webserv: Reading config file");
	try { config.readFile(configPath); }
	catch (const std::exception& e)
	{
		std::cerr << RED << e.what() << RESET << std::endl;
		exit(EXIT_FAILURE);
	}
	LOG("Webserv: Creating servers");
	for (ft::ServerSection& serverSection : config.serverSections)
		servers.push_back(ft::Server(serverSection));
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
	ft::GlobalConfig		config;	 	// Contains the entire configuration for the whole application/
	std::vector<ft::Server>	servers; 	// A vector list that contains all of the servers.

	if (argc != 2)
	{
		std::cerr << "\nWebserv: Invalid arguments\n" << std::endl;
		std::cerr << "Usage: ./webserv <Configuration Filepath>\n" << std::endl;
		return (EXIT_FAILURE);
	}

	LOG("Webserv: Starting");
	setupServers(argv[1], config, servers);
	ft::Poller poller(servers, config);

	LOG("Webserv: Running");
	while (true)
	{
		poller.pollAll();
		usleep(420);
	}
}
