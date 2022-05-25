/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Request.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 19:13:27 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/05/25 16:46:45 by pvan-dij      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef REQUEST_HPP
# define REQUEST_HPP
# include "Exchange.hpp"
FT_BEGIN

// A Request is from the incoming client.
class Request : Exchange
{
public:
	Request(char *buffer);

	ft::method method;
	std::string path;
	std::string version;
	std::map<std::string, std::string> fields;

	void print(void);
private:
	void set_method(std::string s_method);
};

FT_END
#endif
