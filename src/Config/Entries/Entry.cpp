/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Entry.cpp                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/01 15:39:35 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/01 15:47:36 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Entry.hpp"

//////////////////////////////////////////

bool ft::Entry::keyExists(const std::string& key) const
{
	return (this->fields.find(key) != this->fields.end());
}

const std::string* ft::Entry::getValue(const std::string& key) const
{
	auto it = this->fields.find(key);

	return (it != this->fields.end() ? &it->second : nullptr);
}

void ft::Entry::setValue(const std::string& key, const std::string& value)
{
	// NOTE: According to docs if key does not exist, it will be added.
	this->fields[key] = value;
}

//////////////////////////////////////////
