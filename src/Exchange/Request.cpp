/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Request.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 19:34:12 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/05/25 16:47:28 by pvan-dij      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

//////////////////////////////////////////

void ft::Request::print(void)
{
	std::cout << method << "-";
	std::cout << path << "-";
	std::cout << version << std::endl;

	for (std::map<std::string, std::string>::iterator it = fields.begin(); it != fields.end(); ++it)
	{
		std::cout << it->first << " : " << it->second << std::endl;
	}
}

void ft::Request::set_method(std::string s_method)
{
	if (s_method == "GET")
		method = GET;
	if (s_method == "POST")
		method = POST;
	if (s_method == "DELETE")
		method = DELETE;
}

ft::Request::Request(char *buffer)
{
    std::istringstream iss(buffer);
    std::string item;
	std::string temp;

    std::getline(iss, item, ' ');
	set_method(item);
    std::getline(iss, path, ' ');
    std::getline(iss, version);
	while (iss >> temp)
	{
		iss >>item;
		temp.pop_back();
		fields[temp] = item;
	}
}

//////////////////////////////////////////
