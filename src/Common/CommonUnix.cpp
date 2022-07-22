/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CommonUnix.cpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/13 17:05:34 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/21 18:58:21 by pvan-dij      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "CommonUnix.hpp"

//////////////////////////////////////////

void ft::pipe(int32_t fds[2])
{
	if (::pipe(fds) == -1)
	{
		fds[0] = -1;
		fds[1] = -1;
		throw ft::GenericErrnoException();
	}
}

int32_t ft::fork(void)
{
	pid_t pid = ::fork();

	if (pid == -1)
		throw ft::GenericErrnoException();

	return (pid);
}

int32_t ft::execve(const std::string& file, char* const* argv, char* const* envp)
{
	int32_t val = ::execve(file.c_str(), argv, envp);

	if (val == -1)
		throw ft::GenericErrnoException();

	return (val);
}

int32_t ft::dup2(int32_t fdsA, int32_t fdsB)
{
	int32_t val = ::dup2(fdsA, fdsB);

	if (val == -1)
		throw ft::GenericErrnoException();

	return (val);
}

//////////////////////////////////////////
