/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   GlobalConfig.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/01 14:59:11 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/14 21:11:04 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <sys/stat.h>
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

static bool isSubSectionDef(const std::string& line)
{
	size_t firstCharIndex = line.find_first_not_of(WHITESPACE, 1);
	return (line[firstCharIndex] == '.');
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
	if (name.find(' ') != std::string::npos) // no spaces in section names (after trimming)
		return (false);
	if (name.find('.', 1) != std::string::npos) // no dots in section names (except for first character, which defines a subsection)
		return (false);
	return (true);
}

static bool isValidSectionPath(const std::string& path)
{
	if (path.front() != '/') // path has to start with a slash (which is the root of the server)
		return (false);
	if (path.length() == 1) // path cannot just be a slash
		return (false);
	return (true);
}

static void getSectionName(const uint32_t lineNum, const std::string& line, std::string& name)
{
	std::string temp = line;

	if (!isValidSectionDef(temp))
		throw ft::ConfigParserSyntaxException(lineNum);
	initParsedSection(temp);
	name = temp; // copy over to output
	if (name != "server")
		throw ft::UnknownSectionTypeException(lineNum);
	if (!isValidSectionNameSyntax(name))
		throw ft::ConfigParserSyntaxException(lineNum);
}

static void getSubSectionName(const uint32_t lineNum, const std::string& line, std::string& name, std::string& path)
{
	std::string temp = line;

	if (!isValidSectionDef(temp))
		throw ft::ConfigParserSyntaxException(lineNum);
	initParsedSection(temp);

	std::stringstream stream(temp); // create a stringstream out of the temp string
	std::string word;
	uint8_t count = 0;

	while (count < 4 && stream >> word) // loop over max 4 words, which is already a syntax error
	{
		if (count == 0)
			name = word;
		count++;
	}
	if (count > 2) // only allow the definition of the subsection (.location) and the path it is defined for (without any whitespace)
		throw ft::ConfigParserSyntaxException(lineNum);
	if (!isValidSectionNameSyntax(name))
		throw ft::ConfigParserSyntaxException(lineNum);
	if (name != ".location") // only handle .location as subsection
		throw ft::UnknownSectionTypeException(lineNum);
	if (count == 1) // no path defined for subsection
		throw ft::NoSubSectionLocation(lineNum);
	path = word; // copy the last word over as the subsection path
	if (!isValidSectionPath(path))
		throw ft::ConfigParserSyntaxException(lineNum);
}

//////////////////////////////////////////

void ft::GlobalConfig::readFile(const std::string& filePath)
{
	struct stat s;
	if (stat(filePath.c_str(), &s) == 0)
		if (s.st_mode & S_IFDIR)
			throw ft::IOException(); // it's a fucking directory you twat
	else
		throw ft::IOException(); // generic file read error
	std::ifstream fstream(filePath);
	if (!fstream.good())
		throw ft::IOException();
	std::string line;
	uint32_t lineNum = 0;
	std::pair<std::string, std::string> output;
	std::vector<std::string> definedPaths;
	ft::Section* currentSection = &this->globalSection;

	while (std::getline(fstream, line))
	{
		lineNum++;
		ft::trim(line); // trim the whole line (remove whitespace at beginning and end)
		if (isComment(line) || line.length() == 0) // skip comments and empty lines
			continue;

		if (isSectionDef(line))
		{
			std::string sectionName;
			std::string appliesToPath;

			if (currentSection->getName() != "global")
			{
				if (currentSection->getAmountOfFields() == 0)
					throw ft::EmptySectionException(lineNum);
				if (currentSection->getName() == "server")
					if (!currentSection->keyExists("path") || !currentSection->keyExists("server_names") || !currentSection->keyExists("listen"))
						throw ft::MissingFieldException(lineNum);
			}

			if (isSubSectionDef(line)) // is subsection (.location)
			{
				getSubSectionName(lineNum, line, sectionName, appliesToPath);
				if (currentSection->getName() == "global")
					throw ft::InvalidSubSectionPosition(lineNum);
				for (const auto definedPath : definedPaths)
					if (definedPath == appliesToPath)
						throw ft::DuplicateSubSectionLocation(lineNum);
				ft::ServerSection& currentServerSection = this->serverSections.back();
				ft::Section location(*currentServerSection.getValue("path"), sectionName, appliesToPath); // create new location subsection
				currentServerSection.locations.push_back(location); // add subsection to server
				definedPaths.push_back(appliesToPath); // add path to defined locations for this server
				currentSection = &currentServerSection.locations.back(); // change current section to the newly generated location
			}
			else // is main section (server)
			{
				getSectionName(lineNum, line, sectionName);
				ft::ServerSection server(this->globalSection.getcwd(), sectionName, this->globalSection);
				this->serverSections.push_back(server); // add new server to list of servers in globalconfig
				currentSection = &this->serverSections.back(); // change current section to the newly generated server
				definedPaths.clear(); // clear the defined locations list
			}
			continue; // continue with next line
		}

		ft::slice(line, '=', output);
		ft::trim(output.first);
		ft::trim(output.second);
		currentSection->verifyKeyValue(lineNum, output.first, output.second);
		currentSection->setValue(output.first, output.second);
	}

	verifyConfig(lineNum + 1);
}

void ft::GlobalConfig::verifyConfig(const uint32_t& lineNum) const
{
	if (this->serverSections.size() == 0)
		throw ft::NoServersException();
	for (const ft::ServerSection& server : this->serverSections)
	{
		if (server.getAmountOfFields() == 0)
			throw ft::EmptySectionException(lineNum);
		if (!server.keyExists("path") || !server.keyExists("server_names") || !server.keyExists("listen"))
			throw ft::MissingFieldException(lineNum);
		for (const ft::Section& location : server.locations)
			if (location.getAmountOfFields() == 0)
				throw ft::EmptySectionException(lineNum);
	}
}

//////////////////////////////////////////
