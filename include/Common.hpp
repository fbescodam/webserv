/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Common.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 17:39:22 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/21 19:06:27 by pvan-dij      ########   odam.nl         */
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
# include <list>
# include <string>
# include <memory>
# include <unistd.h>
# include <map>
# include <chrono>
# include <sstream>
# include <iterator>
# include <utility>

# define NONE 0
# define WRITE 1
# define READ 0
# define WHITESPACE " \t\r\n\t\f\v"
# define S_TIMEOUT 5
# define MS_TIMEOUT S_TIMEOUT*1000
# define MAX_CLIENTS 60

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
void split(const std::string& string, char *Delim, std::vector<std::string>& output);
void split(const std::string& string, std::string delim, std::vector<std::string>& output);


/**
 * Slices a string into two parts at the first occurence of the delimiter.
 *
 * @param string The string to split.
 * @param delim The delimiter.
 * @param output The output.
 */
void slice(const std::string& string, char delim, std::pair<std::string, std::string>& output);
void slice(const std::string& string, std::string delim, std::pair<std::string, std::string>& output);


/**
 * Remove whitespace characters from the beginning and end of a string.
 *
 * @param string The string to trim. Warning: gets modified
 */
void trim(std::string& string);

/**
 * Get the directory path to a file (remove filename from a path)
 *
 * @param path a full path to a file
 * @return the path to the directory the file is in
 */
std::string basedir(const std::string& path);

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

// File system-ish functions (because MacOS 10.14)
//////////////////////////////////////////

namespace filesystem {

/**
 * @brief 
 * 
 * @param path 
 * @return true 
 * @return false 
 */
bool fileExists(const std::string& path);

/**
 * @brief Get the File Size object
 * 
 * @param is 
 * @return int 
 */
size_t getFileSize(std::ifstream &is);

/**
 * @brief Get the File Size object
 * 
 * @param f 
 * @return int 
 */
size_t getFileSize(FILE *f);

/**
 * Joins arguments together
 * 
 * @tparam Args 
 * @param format 
 * @param args 
 * @return std::string 
 */
template <typename Arg, typename... Args>
std::string join(Arg arg, Args... args)
{
	// Make sure we only use strings, crude prevention, too bad.
	static_assert(std::is_convertible<Arg, std::string>());

	/** 
	 * @see https://stackoverflow.com/questions/27375089/what-is-the-easiest-way-to-print-a-variadic-parameter-pack-using-stdostream 
	 * @see https://en.cppreference.com/w/cpp/language/fold
	 */
	std::stringstream stream;

	// Insert first argument
	stream << std::forward<Arg>(arg);

	// Fold args together (Crazy feature, just why ?)
	((stream << '/' << std::forward<Args>(args)), ...);

	return (stream.str());
}
}

FT_END
#endif
