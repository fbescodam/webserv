/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   GlobalConfig.hpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 19:43:14 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/05/30 16:49:48 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef GlobalConfig_HPP
# define GlobalConfig_HPP
# include "Common.hpp"
# include "Config.hpp"
# include "Server.hpp"
# include <list>
FT_BEGIN

// A GlobalConfig singleton object that reads and stores the server information.
class GlobalConfig
{
protected: // Ctor ~ Dtor
	GlobalConfig(const std::string& filepath);

public:
	GlobalConfig(GlobalConfig &other) = delete;
	void operator=(const GlobalConfig&) = delete;

public: // Functions
	Server getServer(const std::string& host, const uint16_t port);

public: // Attributes
private:
	Config _config;
	std::list<Server> _servers;
};

FT_END
#endif
