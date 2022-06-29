/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   GlobalConfig.hpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/01 13:54:52 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/27 21:07:25 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef GLOBALCONFIG_HPP
# define GLOBALCONFIG_HPP
# include "Common.hpp"
# include "Exceptions.hpp"
# include "ServerSection.hpp"
FT_BEGIN

/**
 * A config files containskey & values along with several
 * entries with their own unique single subsection.
 */
class GlobalConfig
{
public: // Ctor ~ Dtor
	GlobalConfig() = default;
	~GlobalConfig() = default;

public: // Functions
	/**
	 * Reads the given file and parses it. Throws exceptions on failure
	 *
	 */
	void readFile(const std::string& filePath);

	/**
	 * Inits all servers
	 *
	 */
	void runServers(void);

private:
	/**
	 * Check if config is configured properly. Throws exception if not
	 *
	 */
	void verifyConfig() const;

public: // Attributes

	// Global configuration at the start.
	ft::Section globalSection = ft::Section(getcwd(NULL, 0), "global");

	// Server sections.
	std::vector<ft::ServerSection> serverSections; // TODO: Server class should hold a reference to an section as its 'config'
};

FT_END
#endif
