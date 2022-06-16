/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Common.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 17:39:22 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/16 21:08:56 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

// Common includes, typedefs and other stuff that we constantly need.

#pragma once

#ifndef COMMON_HPP
# define COMMON_HPP
// To avoid ugly idents
# define FT_BEGIN namespace ft {
# define FT_END }
# include <iostream>
# include <fstream>
# include <vector>
# include <string>
# include <memory>
# include <unistd.h>
# include <map>
# include <chrono>
# include <sstream>
# include <iterator>
# include <utility>
# include "Exceptions.hpp"

# define NONE 0
# define WRITE 1
# define READ 0
# define WHITESPACE " \t\n\t\f\v"
# define S_TIMEOUT 60
# define MS_TIMEOUT 60*1000

FT_BEGIN

// Common Functions
//////////////////////////////////////////

/**
 * Splits a string and stores the result in Output.
 *
 * @param string The string to split.
 * @param delim The delimiter.
 * @param output The output.
 */
void split(const std::string& string, char Delim, std::vector<std::string>& output);

/**
 * Slices a string into two parts at the first occurence of the delimiter.
 *
 * @param string The string to split.
 * @param delim The delimiter.
 * @param output The output.
 */
void slice(const std::string& string, char delim, std::pair<std::string, std::string>& output);

/**
 * Remove whitespace characters from the beginning and end of a string.
 *
 * @param string The string to trim. Warning: gets modified
 */
void trim(std::string& string);

/**
 * Creates a new string from the format.
 *
 * Use normal printf args such as %d or %s.
 *
 * @tparam Args va_args
 * @param format The format string
 * @param args The arguments to pass to the string.
 * @return std::string
 */
template<typename... Args>
std::string format(const std::string& format, Args ... args)
{
	// Get size of the formated string
	int32_t size = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1;
	if (size <= 0)
		throw std::runtime_error("Formatting error!");

	// Make temp buff to put in formatted result.
	char* buff = new char[size];
	std::snprintf(buff, size, format.c_str(), args ...);

	// Pass to string
	std::string out(buff, buff + size - 1);
	delete[] buff;

	return (out);
}

FT_END
#endif
