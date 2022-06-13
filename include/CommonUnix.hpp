/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CommonUnix.hpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/13 17:10:16 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/13 18:05:18 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef COMMONUNIX_HPP
# define COMMONUNIX_HPP
# include "Exceptions.hpp"
FT_BEGIN

// pipe wrapper.
void pipe(int32_t fds[2]);

// fork wrapper.
int32_t fork(void);

// Execve wrapper.
int32_t execve(const char* file, char* const* argv, char* const* envp);

/**
 * Check if a file exists.
 * 
 * @param path The file path to test.
 * @return True if file exists, else false. 
 */
bool fileExists(const std::string& path);

FT_END
#endif
