/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   GlobalConfig.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/01 14:59:11 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/27 21:10:23 by fbes          ########   odam.nl         */
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

static bool isValidSectionDef(const std::string& line)
{
	return (line.front() == '[' && line.back() == ']'); // check if string starts with [ and ends with ]
}

static void initParsedSection(std::string& sectionDef)
{
	sectionDef.erase(0, 1); // remove [
	sectionDef.erase(sectionDef.size() - 1); // remove ]
	ft::trim(sectionDef); // remove whitespace around the leftover string
}

static bool isValidSectionNameSyntax(const std::string& name)
{
	if (name.find(' ') != std::string::npos) // do not allow strings in section names (after trimming)
		throw ft::ConfigParserSyntaxException();
	if (name.find('.', 1) != std::string::npos) // do not allow dots in section names (except for first character)
		throw ft::ConfigParserSyntaxException();
	return (true);
}

static void getSectionName(const std::string& line, std::string& name)
{
	std::string temp = line;

	if (!isValidSectionDef(temp))
		throw ft::ConfigParserSyntaxException();
	initParsedSection(temp);
	if (!isValidSectionNameSyntax(temp))
		throw ft::ConfigParserSyntaxException();
	name = temp; // copy over to output
}

static void getSubSectionName(const std::string& line, std::string& name, std::string& path)
{
	std::string temp = line;

	if (!isValidSectionDef(temp))
		throw ft::ConfigParserSyntaxException();
	initParsedSection(temp);

	std::stringstream stream(temp); // create a stringstream out of the temp string
	std::string word;
	uint8_t count = 0;

	while (count < 4 && stream >> word)
	{
		if (count == 0)
			name = word;
		count++;
	}
	if (!isValidSectionNameSyntax(name))
		throw ft::ConfigParserSyntaxException();
	if (count != 2) // only allow the definition of the subsection (.location) and the path it is defined for (without any whitespace)
		throw ft::ConfigParserSyntaxException();
	path = word; // copy the last word over as the subsection path, will not get here if it throws
}

//////////////////////////////////////////

void ft::GlobalConfig::readFile(const std::string& filePath)
{
	std::ifstream fstream(filePath);
	if (!fstream.good())
		throw ft::FileNotFoundException();
	std::string line;
	std::pair<std::string, std::string> output;
	ft::Section* currentSection = &this->globalSection;

	while (std::getline(fstream, line))
	{
		ft::trim(line); // trim the whole line (remove whitespace at beginning and end)
		if (isComment(line) || line.length() == 0) // skip comments and empty lines
			continue;

		if (isSectionDef(line))
		{
			std::string sectionName;
			std::string appliesToPath;

			if (isSubSectionDef(sectionName)) { // is subsection (.location)
				getSubSectionName(line, sectionName, appliesToPath);
				if (currentSection->getName() == "global")
					throw ft::InvalidSubSectionPosition();
				if (sectionName != ".location") // only handle .location as subsection
					throw ft::UnknownSectionTypeException();
				ft::ServerSection& currentServerSection = this->serverSections.back();
				ft::Section location(*currentServerSection.getValue("path"), sectionName, appliesToPath); // create new location subsection
				currentServerSection.locations.push_back(location); // add subsection to server
				currentSection = &currentServerSection.locations.back(); // change current section to the newly generated location
			}
			else { // is main section (server)
				getSectionName(line, sectionName);
				if (sectionName != "server")
					throw ft::UnknownSectionTypeException();
				ft::ServerSection server(this->globalSection.getcwd(), sectionName, this->globalSection);
				this->serverSections.push_back(server); // add new server to list of servers in globalconfig
				currentSection = &this->serverSections.back(); // change current section to the newly generated server
			}
			continue; // continue with next line
		}

		ft::slice(line, '=', output);
		ft::trim(output.first);
		ft::trim(output.second);
		currentSection->verifyKeyValue(output.first, output.second);
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
