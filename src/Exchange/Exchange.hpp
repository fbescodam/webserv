/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Exchange.hpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 19:13:27 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/05/26 15:53:34 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef EXCHANGE_HPP
# define EXCHANGE_HPP
# include "CommonNetwork.hpp"
FT_BEGIN

// Supported exchange methods for the webserver.
enum class ExchangeMethod : uint8_t
{
	GET,
	POST,
	DELETE
};

// An exchange is both a response and a request via http
class Exchange
{
public: // Attributes
	std::map<std::string, std::string> fields;
};

FT_END
#endif
