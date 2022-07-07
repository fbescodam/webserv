/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ServerSection.cpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: fbes <fbes@student.codam.nl>                 +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/16 21:23:40 by fbes          #+#    #+#                 */
/*   Updated: 2022/07/07 18:32:53 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ServerSection.hpp"

ft::ServerSection::ServerSection(const std::string& cwd, const std::string& name, ft::Section& inherit) : ft::Section(cwd, name, inherit)
{

}

//////////////////////////////////////////

void ft::ServerSection::printAll() const
{
	this->print("");
	for (const ft::Section& location : this->locations)
		location.print("	");
	std::cout << "[DEBUG] " << std::endl;
}

const std::string* ft::ServerSection::getKeyFromLocationOrServer(const ft::Section& location, const std::string& key) const
{
	const std::string* ptr = location.getValue(key);
	if (ptr)
		return (ptr);
	return (this->getValue(key));
}

bool ft::ServerSection::keyExistsInLocationOrServer(const ft::Section& location, const std::string& key) const
{
	if (location.keyExists(key))
		return (true);
	return (this->keyExists(key));
}

//////////////////////////////////////////
