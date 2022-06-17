/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   GlobalConfig.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/01 14:59:11 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/17 04:22:20 by fbes          ########   odam.nl         */
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
	if (line.front() != '[' || line.back() != ']')
		throw ft::ConfigParserSyntaxException();
	output = line;
	output.erase(0, 1); // remove [
	output.erase(output.size() - 1); // remove ]
	ft::trim(output); // remove whitespace around the leftover string
	if (output.find(' ') != std::string::npos) // do not allow strings in section names (after trimming)
		throw ft::ConfigParserSyntaxException();
	if (output.find('.', 1) != std::string::npos) // do not allow dots in section names (except for first character)
		throw ft::ConfigParserSyntaxException();
}

void ft::GlobalConfig::readFile(const std::string& filePath)
{
	std::ifstream fstream(filePath);
	if (!fstream.good())
		throw ft::FileNotFoundExecption();
	std::string line;
	std::pair<std::string, std::string> output;
	ft::Section* currentSection = &this->globalSection;

	while (std::getline(fstream, line))
	{
		ft::trim(line);
		if (isComment(line) || line.length() == 0) // skip comments and empty lines
			continue;

		if (isSectionDef(line))
		{
			std::string sectionName;

			getSectionName(line, sectionName);
			if (isSubSectionDef(sectionName)) { // is subsection (.location)
				if (currentSection->getName() == "global")
					throw ft::InvalidSubSectionPosition();
				if (sectionName != ".location") // only handle .location as subsection
					throw ft::UnknownSectionTypeException();
				ft::Section location(sectionName); // create new location subsection
				ft::ServerSection& currentServerSection = this->serverSections.back();
				currentServerSection.locations.push_back(location); // add subsection to server
				currentSection = &currentServerSection.locations.back(); // change current section to the newly generated location
			}
			else { // is main section (server)
				if (sectionName != "server")
					throw ft::UnknownSectionTypeException();
				ft::ServerSection server(sectionName, this->globalSection);
				this->serverSections.push_back(server); // add new server to list of servers in globalconfig
				currentSection = &this->serverSections.back(); // change current section to the newly generated server
			}
			continue; // continue with next line
		}

		ft::slice(line, '=', output);
		ft::trim(output.first);
		ft::trim(output.second);
		currentSection->setValue(output.first, output.second);
	}

	verifyConfig();
}

void ft::GlobalConfig::verifyConfig() const
{
	if (this->serverSections.size() == 0)
		throw ft::NoServersException();
	for (const ft::ServerSection& server : this->serverSections)
	{
		if (server.getAmountOfFields() == 0)
			throw ft::EmptySectionException();
		for (const ft::Section& location : server.locations)
			if (location.getAmountOfFields() == 0)
				throw ft::EmptySectionException();
	}
}

//////////////////////////////////////////
