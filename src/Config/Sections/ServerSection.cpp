/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ServerSection.cpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: fbes <fbes@student.codam.nl>                 +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/16 21:23:40 by fbes          #+#    #+#                 */
/*   Updated: 2022/06/17 04:19:56 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ServerSection.hpp"

ft::ServerSection::ServerSection(const std::string& name, ft::Section& inherit) : ft::Section(name, inherit)
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

//////////////////////////////////////////