/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Common.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/25 19:34:04 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/14 13:26:52 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Common.hpp"

//////////////////////////////////////////

[[noreturn]] void ft::exceptionExit(const std::exception& e, int32_t code = EXIT_FAILURE)
{
	std::cerr << "Webserv: " << e.what() << std::endl;
	exit(code);
}

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
	if (start == std::string::npos)
		throw ft::DelimiterNotFoundException();
	output.first = string.substr(0, start);
	output.second = string.substr(start + 1);
}

void ft::trim(std::string& string)
{
	string.erase(0, string.find_first_not_of(WHITESPACE)); // trim whitespace from left
	string.erase(string.find_last_not_of(WHITESPACE) + 1); // trim whitespace from right
}

//////////////////////////////////////////
