/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Section.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/01 15:39:35 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/13 18:26:06 by fbes          ########   odam.nl         */
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

void ft::Section::setValue(const std::string& key, const std::string& value)
{
	// NOTE: According to docs if key does not exist, it will be added.
	this->fields[key] = value;
}

const std::string& ft::Section::getName()
{
	return (this->name);
}

//////////////////////////////////////////
