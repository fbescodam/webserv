/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Request.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 19:13:27 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/05/30 17:26:39 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef REQUEST_HPP
# define REQUEST_HPP
# include "Exchange.hpp"
FT_BEGIN

// A Request is from the incoming client.
class Request final : public Exchange
{
public: // Ctor ~ Dtor
	Request(const char* Buffer);

public: // Methods
	void display(void) const;

	// Get the newest request from the socket.
	static Request getFirst(int32_t Socket);

private:
	void setMethod(const std::string& method);

public: // Attributes
	ft::ExchangeMethod method;
	std::string path;
	std::string version;
};

FT_END
#endif
