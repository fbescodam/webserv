/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Config.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 19:43:14 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/05/23 19:51:07 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef CONFIG_HPP
# define CONFIG_HPP
# include "Common.hpp"
FT_BEGIN

// A config singleton object that reads and stores the server information.
class Config
{
protected: // Ctor ~ Dtor
	Config() = default;

public:
	Config(Config &other) = delete;
	void operator=(const Config&) = delete;

public: // Functions

	/**
	 * @brief Tries to 
	 * 
	 * @param Filepath 
	 * @return Config 
	 */
	static bool ReadConfig(const std::string& Filepath);

	//
	static Config* GetInstance(void);

public: // Attributes
private:
	static Config* _Instance;
};

Config* Config::_Instance = nullptr;

FT_END
#endif
