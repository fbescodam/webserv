/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Exchange.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/26 17:50:42 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/26 17:52:28 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Exchange.hpp"

//////////////////////////////////////////

const std::string* ft::Exchange::getHeaderValue(const std::string& key) const
{
	auto it = this->headers.find(key);

	return (it != this->headers.end() ? &it->second : nullptr);
}

//////////////////////////////////////////
