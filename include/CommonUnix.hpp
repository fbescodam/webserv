/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CommonUnix.hpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/13 17:10:16 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/17 03:42:58 by lde-la-h      ########   odam.nl         */
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
int32_t execve(const std::string& file, char* const* argv, char* const* envp);

// dup2 wrapper.
int32_t dup2(int32_t fdsA, int32_t fdsB);

FT_END
#endif
