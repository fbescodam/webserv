/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Common.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/25 19:34:04 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/05/26 09:43:48 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Common.hpp"

void ft::split(const std::string& string, char delim, std::vector<std::string>& output)
{
	output.clear();
	output.reserve(std::count(string.begin(), string.end(), delim));

    size_t start, end = 0; 
    while ((start = string.find_first_not_of(delim, end)) != std::string::npos)
    {
        end = string.find(delim, start);
        output.push_back(string.substr(start, end - start));
    }
}

void ft::slice(const std::string& string, char delim, std::pair<std::string, std::string>& output)
{
    size_t start, end = 0;
	start = string.find_first_of(delim, end);
	output.first = string.substr(0, start);
	output.second = string.substr(start + 1);
}

