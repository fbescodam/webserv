/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Utils.hpp                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/27 10:40:21 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/27 10:47:21 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef UTILS_HPP
# define UTILS_HPP
# define WHITESPACE " \t\r\n\t\f\v"
# include <string>
# include "Exceptions.hpp"

namespace ft
{

// NOTE: Cant be bothered with templates just yet ...

/**
 * Remove whitespace characters from the beginning and end of a string.
 *
 * @param string The string to trim. Warning: gets modified
 */
void trim(std::string& string);

/**
 * Slices a string into two parts at the first occurence of the delimiter.
 *
 * @param string The string to split.
 * @param delim The delimiter.
 * @param output The output.
 */
void slice(const std::string& string, std::string delim, std::pair<std::string, std::string>& output);

}
#endif