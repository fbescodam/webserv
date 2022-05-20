/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Exchange.hpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/20 14:57:25 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/05/20 16:15:59 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef EXCHANGE_HPP
# define EXCHANGE_HPP
# include <map>
# include "Common.hpp"

namespace ft
{
	// HTTP Exchange methods
	enum class Methods : uint8_t
	{
		GET,
		POST,
		DELETE,
	};

	/**
	 * An exchange is either a response or a request over http
	 */
	class Exchange
	{
	public:
		Exchange() = default;

	private: // Attributes
		std::map<std::string, std::string> _Headers;
		std::vector<uint8_t> _Body;
	};

	// TODO: Docs 

	// Request
	class Request : public Exchange
	{
	public: // Attributes
		Request(char *raw_request)
		{
			std::string raw(raw_request);

			std::string method = raw.substr(0, raw.find(' '));

			if (method == "GET")
				Type = ft::Methods::GET;
			else if (method == "POST")
				Type = ft::Methods::POST;
			else if (method == "DELETE")
				Type = ft::Methods::DELETE;
			return ;
		}
		// Requested path on the server.
		std::string Path;

		// Type of request.
		ft::Methods Type;
	};

	// Response
	class Response : public Exchange
	{
	public: // Ctor ~ Dtor
		Response(const Request& InRequest);

	public: // Attributes
		uint16_t Status;
	};

}
#endif
