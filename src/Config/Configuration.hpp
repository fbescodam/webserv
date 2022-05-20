/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Configuration.hpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: pvan-dij <pvan-dij@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/20 15:53:12 by pvan-dij      #+#    #+#                 */
/*   Updated: 2022/05/20 16:19:42 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP
# include "Common.hpp"
# include <map>
# include <array>

namespace ft
{
	class Option
	{
	public: // Ctor ~ Dtor
		Option(const std::string& Name, const std::string& Value) :
		_Name(Name), _Value(Value)
		{
			_AllowedIn.fill(false);
		}

	public: // Types
		enum class ConfigTypes : uint8_t
		{
			SERVER,
			DIR,
			FILE
		};

	private: // Attributes
		std::string _Name;
		std::string _Value;
		std::array<bool, 3> _AllowedIn;
	};

	class Configuration
	{
	public: // Functions
		virtual bool ReadConfig(const std::string& Path) = 0;

	private: // Attributes
		// std::vector<std::map<std::string, std::string>> _Config;
	};

	class +
	 : public Configuration
	{
		
	};

	class SubConfiguration : public Configuration
	{
		
	};
}

#endif
