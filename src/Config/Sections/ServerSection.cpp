/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ServerSection.cpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: fbes <fbes@student.codam.nl>                 +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/16 21:23:40 by fbes          #+#    #+#                 */
/*   Updated: 2022/08/11 16:03:32 by fbes          ########   odam.nl         */
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
		location.print("\t");
	std::cout << std::endl;
}

const std::string* ft::ServerSection::getKeyFromLocationOrServer(const ft::Section& location, const std::string& key) const
{
	const std::string* ptr = location.getValue(key);

	return (ptr != nullptr ? ptr : this->getValue(key));
}

bool ft::ServerSection::keyExistsInLocationOrServer(const ft::Section& location, const std::string& key) const
{
	return (location.keyExists(key) ? true : this->keyExists(key));
}

//////////////////////////////////////////
