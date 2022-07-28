/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Section.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/01 15:39:35 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/28 14:53:21 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Section.hpp"

ft::Section::Section(const std::string& cwd, const std::string& name)
{
	this->cwd = cwd;
	this->name = name;
	std::cout << cwd << std::endl;
}

ft::Section::Section(const char* cwd, const std::string& name)
{
	this->cwd = cwd;
	this->name = name;
	free((void*) cwd);
}

ft::Section::Section(const std::string& cwd, const std::string& name, const std::string& appliesToPath)
{
	this->cwd = cwd;
	this->name = name;
	this->appliesToPath = appliesToPath;
	std::cout << cwd << std::endl;
}

ft::Section::Section(const std::string& cwd, const std::string& name, ft::Section& inherit)
{
	this->cwd = cwd;
	this->name = name;
	this->fields = inherit.fields;
	std::cout << cwd << std::endl;
}

//////////////////////////////////////////

std::map<std::string, std::string> ft::Section::exportFields(void) const
{
	return (this->fields);
}

void ft::Section::importFields(std::map<std::string, std::string> fields)
{
	for (const auto &vals: fields)
		this->fields[vals.first] = vals.second;
}

bool ft::Section::keyExists(const std::string& key) const
{
	return (this->fields.find(key) != this->fields.end());
}

const std::string* ft::Section::getValue(const std::string& key) const
{
	auto it = this->fields.find(key);

	return (it != this->fields.end() ? &it->second : nullptr);
}

bool ft::Section::getValueAsInt(const std::string& key, int& output) const
{
	auto it = this->fields.find(key);

	if (it == this->fields.end())
		return (false);
	output = std::stoi(it->second);
	return (true);
}

int ft::Section::returnValueAsInt(const std::string& key) const
{
	auto it = this->fields.find(key);

	if (it == this->fields.end())
		return (-1);
	return (std::stoi(it->second));
}

bool ft::Section::returnValueAsBoolean(const std::string& key) const
{
	auto it = this->fields.find(key);

	if (it == this->fields.end())
		return (true); // intended
	return (it->second == "yes");
}

bool ft::Section::getValueAsList(const std::string& key, std::list<std::string>& list) const
{
	auto it = this->fields.find(key);

	list.clear();
	if (it == this->fields.end())
		return (false);

	std::stringstream stream(it->second);
	std::string item;
	while (stream >> item)
		list.push_back(item);
	return (true);
}

void ft::Section::setValue(const std::string& key, const std::string& value)
{
	// NOTE: According to docs if key does not exist, it will be added.
	this->fields[key] = value;
}

const std::string& ft::Section::getName() const
{
	return (this->name);
}

const std::string& ft::Section::getAppliedPath() const
{
	return (this->appliesToPath);
}

bool ft::Section::appliesForPath(const std::string& requestedPath) const
{
	if (this->appliesToPath == requestedPath || requestedPath.rfind(this->appliesToPath, 0) == 0)
		return (true);
	return (false);
}

uint32_t ft::Section::getAmountOfFields() const
{
	return (this->fields.size());
}

void ft::Section::print(std::string prefix) const
{
	std::cout << "[DEBUG] " << prefix << ">>>> SECTION NAME " << this->name << " <<<<" << std::endl;

	if (!this->appliesToPath.empty())
		std::cout << "[DEBUG] " << prefix << ">>>> APPLIES TO PATH " << this->appliesToPath << " <<<<" << std::endl;

	for (auto& i : this->fields)
		std::cout << "[DEBUG] " << prefix <<  i.first << " = " << i.second << std::endl;
}

const std::string& ft::Section::getcwd() const
{
	return (this->cwd);
}

// TODO: change to map with string keys and function pointer values instead of this ugly switch case
void ft::Section::verifyKeyValue(uint32_t lineNum, std::string& key, std::string& value) const
{
	// all possible keys
	const std::string possibleKeys[] = {
		"limit_body_size",
		"listen",
		"server_name",
		"index",
		"access",
		"dir_listing",
		"error_404", // TODO: add more custom error pages
		"methods",
		"path",
		"redir",
		"error_403",
		"cgi_bin"
	};

	int8_t index = -1;
	for (int8_t i = 0; i < (int8_t)possibleKeys->size(); i++)
	{
		if (possibleKeys[i] == key)
		{
			index = i;
			break;
		}
	}
	size_t space;
	int statusCode;

	if (index == -1)
		throw ft::UnknownFieldKeyException(lineNum);

	long bignum;
	switch (index)
	{
		// expecting integer in range > 0 < INT_MAX
		case 0: // limit_body_size
			bignum = std::stol(value);
			if (bignum <= 0 || bignum > INT_MAX)
				throw ft::InvalidFieldValueException(lineNum);
			break;

		// expecting integer in range > 0 < USHRT_MAX
		case 1: // listen
			bignum = std::stol(value);
			if (bignum <= 0 || bignum > USHRT_MAX)
				throw ft::InvalidFieldValueException(lineNum);
			break;

		// expecting a non-empty string
		case 2: // server_name
		case 3: // index
		case 6: // error_404
		case 7: // methods
		case 10: //error_403
		case 8: // path
			if (value.size() == 0)
				throw ft::InvalidFieldValueException(lineNum);
			break;

		// expecting a boolean in the form of "yes" or "no"
		case 4: // access
		case 5: // dir_listing
			if (value != "yes" && value != "no")
				throw ft::InvalidFieldValueException(lineNum);
			break;

		// expecting two strings, of which 1st is either "temp" or "perm" or a number (status code) and 2nd is a path
		case 9: // redir
			space = value.find(' ');
			if (space == std::string::npos)
				throw ft::InvalidFieldValueException(lineNum);
			statusCode = std::stoi(value.substr(0, space));
			if (statusCode != 301 && statusCode != 302 && statusCode != 307 && statusCode != 308)
				throw ft::InvalidFieldValueException(lineNum);
			// TODO check max 2 fields
			break;
		case 11: // cgi_bin
			space = value.find(' ');
			if (space == std::string::npos)
				throw ft::InvalidFieldValueException(lineNum);
			// TODO check max 2 fields
			break;
	}
}

//////////////////////////////////////////
