/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ServerSection.cpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: fbes <fbes@student.codam.nl>                 +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/16 21:23:40 by fbes          #+#    #+#                 */
/*   Updated: 2022/06/17 08:28:28 by pvan-dij      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ServerSection.hpp"

ft::ServerSection::ServerSection(const std::string& cwd, const std::string& name, ft::Section& inherit) : ft::Section(cwd, name, inherit)
{
	this->setValue("path", "examples/www"); // predefined path for in case it is missing
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

//////////////////////////////////////////