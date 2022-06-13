/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Common.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/25 19:34:04 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/13 16:29:22 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Common.hpp"
#include "Exceptions.hpp"

//////////////////////////////////////////

char** ft::getEnviron(void)
{
    extern char **environ;

    return (environ);
}

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
	output.first = string.substr(0, start);
	output.second = string.substr(start + 1);
}

//////////////////////////////////////////
