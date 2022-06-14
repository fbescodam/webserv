/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   GlobalConfig.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/01 14:59:11 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/13 18:45:01 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "GlobalConfig.hpp"

//////////////////////////////////////////

static bool isComment(const std::string& line)
{
	return (line[0] == ';' || line[0] == '#');
}

static bool isSectionDef(const std::string& line)
{
	return (line[0] == '[');
}

static bool isSubSectionDef(const std::string& sectionName)
{
	return (sectionName[0] == '.');
}

static void getSectionName(const std::string& line, std::string& output)
{
	output = line; // remove [ and ]
}

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


	std::ifstream fstream(filePath);
	if (!fstream.good())
		return (false);
	std::string line;
	std::pair<std::string, std::string> output;

	while (std::getline(fstream, line))
	{
		ft::trim(line);
		if (isComment(line) || line.length() == 0) // skip comments and empty lines
			continue;

		if (isSectionDef(line)) {
			std::string sectionName;

			getSectionName(line, sectionName);
			if (isSubSectionDef(sectionName)) {
				// TODO: define location
			}
			else {
				// TODO: define server
			}
		}

		try { // parse a field
			ft::slice(line, '=', output);
			ft::trim(output.first);
			ft::trim(output.second);
		}
		catch (const ft::DelimiterNotFoundException) {
			std::cout << "Error slicing shit on line " << line << std::endl;
			return (false);
		}
		std::cout << line << std::endl;
	}
	return (true);
}

//////////////////////////////////////////
