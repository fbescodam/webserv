/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Filesystem.hpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/28 12:31:23 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/08/11 19:50:14 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef FILESYSTEM_HPP
# define FILESYSTEM_HPP
# include <iostream>
# include <fstream>

namespace ft {
namespace filesystem {

/**
 * @brief Check if a file exists.
 *
 * @param path
 * @return true
 */
bool fileExists(const std::string& path);

/**
 * @brief Check if a path is a directory.
 *
 * @param path
 * @return true
 */
bool isDir(const std::string& path);

/**
 * @brief Get the File Size.
 *
 * @param is
 * @return size_t
 */
size_t getFileSize(std::ifstream& f);

/**
 * @brief Get the File Size.
 *
 * @param f
 * @return size_t
 */
size_t getFileSize(FILE* f);

}
}
#endif
