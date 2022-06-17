/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Section.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/01 15:39:35 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/17 03:47:43 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Section.hpp"

ft::Section::Section(const std::string& name)
{
	this->name = name;
}

//////////////////////////////////////////

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
	// TODO: check if key is valid
	this->fields[key] = value;
	// std::cout << "[DEBUG] key " << key << " added to section " << this->name << " with value " << value << std::endl;
}

const std::string& ft::Section::getName() const
{
	return (this->name);
}

uint32_t ft::Section::getAmountOfFields() const
{
	return (this->fields.size());
}

void ft::Section::print(std::string prefix) const
{
	std::cout << "[DEBUG] " << prefix << ">>>> SECTION NAME " << this->name << " <<<<" << std::endl;
	for (auto it = this->fields.begin(); it != this->fields.end(); it++) {
		std::cout << "[DEBUG] " << prefix <<  it->first << " = " << it->second << std::endl;
	}
}

//////////////////////////////////////////
