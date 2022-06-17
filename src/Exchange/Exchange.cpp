/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Exchange.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 19:15:11 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/17 06:49:40 by pvan-dij      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Exchange.hpp"

//////////////////////////////////////////

bool ft::Exchange::keyExists(const std::string& key) const
{
	return (this->fields.find(key) != this->fields.end());
}

const std::string* ft::Exchange::getValue(const std::string& key) const
{
	auto it = this->fields.find(key);

	return (it != this->fields.end() ? &it->second : nullptr);
}


//////////////////////////////////////////
