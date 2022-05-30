/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Common.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 17:39:22 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/05/30 19:26:28 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

// Common includes, typedefs and other stuff that we constantly need.

#pragma once

#ifndef COMMON_HPP
# define COMMON_HPP
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

# define NONE 0

// To avoid ugly idents
# define FT_BEGIN namespace ft {
# define FT_END }

FT_BEGIN

// Common Functions
//////////////////////////////////////////

/**
 * Splits a string and stores the result in Output.
 *
 * @param String The string to split.
 * @param Delim The delimiter.
 * @param Output The output.
 */
void split(const std::string& String, char Delim, std::vector<std::string>& Output);

/**
 * Slices a string into two parts at the first occurence of the delimiter.
 *
 * @param String The string to split.
 * @param Delim The delimiter.
 * @param Output The output.
 */
void slice(const std::string& String, char Delim, std::pair<std::string, std::string>& Output);

FT_END
#endif
