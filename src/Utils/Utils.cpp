/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Utils.cpp                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/27 10:41:28 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/27 10:56:53 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"

void ft::trim(std::string& string)
{
	string.erase(0, string.find_first_not_of(WHITESPACE)); // trim whitespace from left
	string.erase(string.find_last_not_of(WHITESPACE) + 1); // trim whitespace from right
}

void ft::slice(const std::string& string, std::string delim, std::pair<std::string, std::string>& output)
{
	size_t start, end = 0;
	start = string.find_first_of(delim);
	if (start == std::string::npos)
		throw ft::DelimiterNotFoundException();
	output.first = string.substr(0, start);
	output.second = string.substr(start + 1);
}
