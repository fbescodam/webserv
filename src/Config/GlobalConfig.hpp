/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   GlobalConfig.hpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/01 13:54:52 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/01 15:53:50 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef GLOBALCONFIG_HPP
# define GLOBALCONFIG_HPP
# include "Common.hpp"
# include "Entries/ServerEntry.hpp"
FT_BEGIN

/**
 * A config files containskey & values along with several
 * entries with their own unique single sub-entry.
 */
class GlobalConfig
{
public: // Ctor ~ Dtor
	GlobalConfig() = default;
	~GlobalConfig() = default;

public: // Functions

	/**
	 * Reads the given file and parses it.
	 * 
	 * @return True if the read & parsing was a success, else false.
	 */
	bool readFile(const std::string& filePath);

public: // Attributes

	// Global entry at the start.
	ft::Entry globalEntry;

	// Server entries.
	std::vector<ft::ServerEntry> serverEntries; // TODO: Server class should hold a reference to an entry as its 'config'
};

FT_END
#endif
