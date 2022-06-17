/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ServerSection.hpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/01 13:54:52 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/13 17:30:53 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef SERVERSECTION_HPP
# define SERVERSECTION_HPP
# include "Common.hpp"
# include "Section.hpp"
FT_BEGIN

class ServerSection final : public ft::Section
{
public: // Ctor ~ Dtor
	ServerSection(const std::string& cwd, const std::string& name, ft::Section& inherit);
	~ServerSection() = default;

public: // Functions
	/**
	 * @brief used for debugging: print the entire config for this server + the locations
	 *
	 */
	void printAll() const;

	/**
	 * @brief get a key from either a specified location, or, if it does not exist there, from the server
	 * 
	 * @param location the location configuration section to search through first
	 * @param key the key of the configuration field to look for
	 * @return a pointer to the value of the configuration field, or NULL if it does not exist
	 */
	const std::string* getKeyFromLocationOrServer(const ft::Section& location, const std::string& key) const;

	/**
	 * @brief check if a key exists in either a location or its server
	 * 
	 * @param location the location configuration section to search through first
	 * @param key the key of the configuration field to look for
	 * @return true if the key exists in either one
	 */
	bool keyExistsInLocationOrServer(const ft::Section& location, const std::string& key) const;

public: // Attributes
	// Server locations
	std::vector<ft::Section> locations;
};

FT_END
#endif