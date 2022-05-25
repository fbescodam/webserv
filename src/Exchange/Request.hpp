/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Request.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 19:13:27 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/05/25 22:16:41 by pvan-dij      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef REQUEST_HPP
# define REQUEST_HPP
# include "Exchange.hpp"
FT_BEGIN

// A Request is from the incoming client.
class Request : public Exchange
{
public: // Ctor ~ Dtor
	Request(const char* Buffer);

public: // Methods
	void Display(void) const;

private:
	void SetMethod(const std::string& SMethod);

public: // Attributes
	ft::ExchangeMethod Method;
	std::string Path;
	std::string Version;
	std::map<std::string, std::string> Fields;
};

FT_END
#endif
