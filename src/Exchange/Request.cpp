/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Request.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 19:34:12 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/01 17:54:24 by pvan-dij      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

//all valid header field characters
// "!#$%&'*+-.^_`|~0123456789abdefghijklmnopqrstuvwxyzABCDEFGHJIKLMNOPQRSTUVWVXYZ";

//TODO: this needs to be thrown out in its entirety almost
ft::Request::Request(std::string buffer)
{
	std::string item;
	std::vector<std::string> splitBuffer;
	ft::split(buffer, "\n\n", splitBuffer);
	std::istringstream iss(splitBuffer[0]);


	std::getline(iss, item, ' ');
	setMethod(item);
	std::getline(iss, this->path, ' ');
	std::getline(iss, this->version);

	std::pair<std::string, std::string> output;
	while (std::getline(iss, item))
	{
		item.erase(remove_if(item.begin(), item.end(), [](char c){return !(c>=32);}), item.end());
		if (!item.empty())
		{
			ft::slice(item, ':', output);
			ft::trim(output.second);
			fields[output.first] = output.second;
		}
	}

	this->body = splitBuffer[1];
}

//////////////////////////////////////////

void ft::Request::display(void) const
{
	std::cout << static_cast<int>(method) << "-";
	std::cout << path << "-";
	std::cout << version << std::endl;

	for (const auto [key, value] : fields)
		std::cout << key << ":" << value << std::endl;
}

void ft::Request::setMethod(const std::string& inMethod)
{
	if (inMethod == "GET")
		method = ft::Method::GET;
	else if (inMethod == "POST")
		method = ft::Method::POST;
	else if (inMethod == "DELETE")
		method = ft::Method::DELETE;
	else
		method = ft::Method::ERR;
}

//////////////////////////////////////////
