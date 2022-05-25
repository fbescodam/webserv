/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Request.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 19:34:12 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/05/25 22:16:37 by pvan-dij      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

ft::Request::Request(const char* Buffer)
{
    std::string Item;
    std::istringstream Iss(Buffer);

    std::getline(Iss, Item, ' ');
	SetMethod(Item);
    std::getline(Iss, this->Path, ' ');
    std::getline(Iss, this->Version);

	std::pair<std::string, std::string> Output;
	while (std::getline(Iss, Item)) // TODO: Make this better
	{
    	Item.erase(remove_if(Item.begin(),Item.end(), [](char c){return !(c>=32);}), Item.end());  
		if (!Item.empty())
		{
			ft::slice(Item, ':', Output);
			Fields[Output.first] = Output.second;
		}
	}
}

//////////////////////////////////////////

void ft::Request::Display(void) const
{
	std::cout << static_cast<int>(Method) << "-";
	std::cout << Path << "-";
	std::cout << Version << std::endl;

	for (const auto [Key, Value] : Fields)
		std::cout << Key << ":" << Value << std::endl;
}

void ft::Request::SetMethod(const std::string& SMethod)
{
	if (SMethod == "GET")
		Method = ft::ExchangeMethod::GET;
	else if (SMethod == "POST")
		Method = ft::ExchangeMethod::POST;
	else if (SMethod == "DELETE")
		Method = ft::ExchangeMethod::DELETE;
}

//////////////////////////////////////////
