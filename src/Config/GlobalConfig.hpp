/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   GlobalConfig.hpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 19:43:14 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/05/30 19:28:05 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef GlobalConfig_HPP
# define GlobalConfig_HPP
# include "Common.hpp"
# include "Config.hpp"
# include "../Server/Server.hpp"
# include <vector>
# define BUFFER_SIZE 128
FT_BEGIN

// A GlobalConfig singleton object that reads and stores the server information.
class GlobalConfig
{
public: // Ctor ~ Dtor
	GlobalConfig(const std::string& filePath);
	GlobalConfig(GlobalConfig &other) = delete;
	~GlobalConfig();
	void operator=(const GlobalConfig&) = delete;

public: // Functions
	const ft::Server& getServer(const std::string& host, const uint16_t port) const;
private:
	const ft::Server* createServer();

public: // Attributes
private:
	Config _config;
	std::vector<Server*> _servers;
};

FT_END
#endif
