/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Common.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/25 19:34:04 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/05/25 21:46:20 by pvan-dij      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Common.hpp"

void ft::split(const std::string& String, char Delim, std::vector<std::string>& Output)
{
	Output.clear();
	Output.reserve(std::count(String.begin(), String.end(), Delim));

    size_t Start, End = 0; 
    while ((Start = String.find_first_not_of(Delim, End)) != std::string::npos)
    {
        End = String.find(Delim, Start);
        Output.push_back(String.substr(Start, End - Start));
    }
}

void ft::slice(const std::string& String, char Delim, std::pair<std::string, std::string>& Output)
{
    size_t Start, End = 0;
	Start = String.find_first_of(Delim, End);
	Output.first = String.substr(0, Start);
	Output.second = String.substr(Start + 1);
}

