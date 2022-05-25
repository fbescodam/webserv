/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Request.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 19:34:12 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/05/25 19:22:59 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

ft::Request::Request(const char* Buffer)
{
    std::istringstream Iss(Buffer);
    std::string Item;
	std::string Temp;

    std::getline(Iss, Item, ' ');
	SetMethod(Item);
    std::getline(Iss, this->Path, ' ');
    std::getline(Iss, this->Version);
	while (Iss >> Temp) // TODO: Make this better
	{
		Iss >> Item;
		Temp.pop_back();
		Fields[Temp] = Item;
	}
}

//////////////////////////////////////////

void ft::Request::Display(void) const
{
	std::cout << static_cast<uint8_t>(Method) << "-";
	std::cout << Path << "-";
	std::cout << Version << std::endl;

	for (const auto [Key, Value] : Fields)
		std::cout << Key << " : " << Value << std::endl;
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
