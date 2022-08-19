/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Section.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/01 15:39:35 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/08/19 11:46:11 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Section.hpp"
#include "Utils.hpp"
#include <algorithm>

ft::Section::Section(const std::string& cwd, const std::string& name)
{
	this->cwd = cwd;
	this->name = name;
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
}

ft::Section::Section(const std::string& cwd, const std::string& name, ft::Section& inherit)
{
	this->cwd = cwd;
	this->name = name;
	this->fields = inherit.fields;
}

//////////////////////////////////////////

std::map<std::string, std::string> ft::Section::exportFields(void) const
{
	return (this->fields);
}

void ft::Section::importFields(std::map<std::string, std::string> fields)
{
	for (const auto &vals: fields)
	{
		LOG("Importing " << vals.first << ": " << vals.second);
		LOG("Currently " << vals.first << ": " << this->fields[vals.first]);
		this->fields[vals.first] = vals.second;
	}
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

int32_t ft::Section::returnValueAsInt(const std::string& key) const
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
#ifdef DEBUG
	std::cout << BLACK << prefix << ">>>> SECTION NAME " << this->name << " <<<<" << std::endl;

	if (!this->appliesToPath.empty())
		std::cout << prefix << ">>>> APPLIES TO PATH " << this->appliesToPath << " <<<<" << std::endl;

	for (auto& i : this->fields)
		std::cout << prefix << i.first << " = " << i.second << std::endl;

	std::cout << RESET << std::endl;;
#endif
}

const std::string& ft::Section::getcwd() const
{
	return (this->cwd);
}

//////////////////////////////////////////
