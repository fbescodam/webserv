/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Common.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/25 19:34:04 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/07 14:08:32 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Common.hpp"
#include "Exceptions.hpp"

//////////////////////////////////////////

[[noreturn]] void ft::exceptionExit(const std::exception& e, int32_t code = EXIT_FAILURE)
{
	std::cerr << "Webserv: " << e.what() << std::endl;
	exit(code);
}

void ft::split(const std::string& string, char *delim, std::vector<std::string>& output)
{
	output.clear();

	size_t start, end = 0;
	while ((start = string.find_first_not_of(delim, end)) != std::string::npos)
	{
		end = string.find(delim, start);
		output.push_back(string.substr(start, end - start));
	}
}

std::string ft::basedir(const std::string& path)
{
	std::string base = path;
	base.erase(path.find_last_of('/'));
	return (base);
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

// File system

bool ft::filesystem::fileExists(const std::string& path)
{
	std::ifstream fstream(path);

	return (fstream.good());
}

size_t ft::filesystem::getFileSize(std::ifstream &is) // path to file
{
    is.seekg(0, is.end);
    size_t length = is.tellg();
    is.seekg(0, is.beg);
    return length;
}

size_t ft::filesystem::getFileSize(FILE *f) // path to file
{
	fseek(f, 0, SEEK_END); // seek to end of file
	size_t size = ftell(f); // get current file pointer
	fseek(f, 0, SEEK_SET); // seek back to beginning of file
	return size;
}

//////////////////////////////////////////
