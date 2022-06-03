/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   GlobalConfig.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/01 14:59:11 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/02 14:33:58 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "GlobalConfig.hpp"

//////////////////////////////////////////

bool ft::GlobalConfig::readFile(const std::string& filePath)
{
	/** 
	 * 1. Any found entries should be added to global
	 * 
	 * 2. Until a new tag is found.
	 * 
	 * 3. That tag is added to the vector of entries
	 * any subtags get added to the parent tag.
	 * 
	 * 4. Repeat 2 until eof or error.
	 */


	// (Wtf is this func name)
	std::ifstream fstream(filePath);
	if (!fstream.good())
		return (false);
	
	// Empty into string stream instead.
	std::stringstream sstream;
	sstream << fstream.rdbuf();
	fstream.close();

	// TODO: @Fbes Do the parsing :P

	return (true);
}

void ft::GlobalConfig::runServers(void)
{
	for (auto& server : this->servers)
	{
		// TODO: @Fbes Your problem :D
	}	
}

//////////////////////////////////////////
