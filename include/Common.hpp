/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Common.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 17:39:22 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/13 18:50:08 by lde-la-h      ########   odam.nl         */
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
# include <sstream>
# include <iterator>
# include <utility>
# include "Exceptions.hpp"

# define NONE 0
# define WRITE 1
# define READ 0
# define WHITESPACE " \t\n\t\f\v"

FT_BEGIN

// Common Functions
//////////////////////////////////////////

/**
 * @brief 
 * 
 * @return char** 
 */
char** ft::getEnviron(void);

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

FT_END
#endif
