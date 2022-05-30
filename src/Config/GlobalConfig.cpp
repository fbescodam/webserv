/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   GlobalConfig.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 19:44:03 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/05/30 19:36:45 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Common.hpp"
#include "GlobalConfig.hpp"
#include "Server.hpp"

static std::string* readConfig(const std::string& filePath)
{
	std::ifstream stream;
	std::string* contents = new std::string("");
	char buffer[BUFFER_SIZE + 1];

	stream.open(filePath, std::ifstream::in);
	while (true)
	{
		try
		{
			bzero(buffer, BUFFER_SIZE + 1);
			stream.read(buffer, BUFFER_SIZE);
			contents->append(buffer);
			if (stream.eof())
				break;
		}
		catch ( ... )
		{
			delete contents;
			return (nullptr);
		}
	}
	return (nullptr);
}

ft::GlobalConfig::GlobalConfig(const std::string& filePath)
{
	// TODO: STRINGSTREAM
	std::string* aaaaaa = readConfig(filePath);
}

ft::GlobalConfig::~GlobalConfig()
{
	// delete all virtual servers
	for (const auto* server : this->_servers)
		delete (server);
}

/**
 * @brief Create a virtual server and return a pointer to it
 *
 * @return const ft::Server*
 */
const ft::Server* ft::GlobalConfig::createServer()
{
	ft::Server* server = new ft::Server(this->_config);
	this->_servers.insert(this->_servers.end(), server);
	return (server);
}
