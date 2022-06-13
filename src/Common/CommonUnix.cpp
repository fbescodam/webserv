/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CommonUnix.cpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/13 17:05:34 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/13 17:55:09 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "CommonUnix.hpp"

//////////////////////////////////////////

void ft::pipe(int32_t fds[2])
{
	if (::pipe(fds) == -1)
		throw ft::GenericErrnoExecption();
}

int32_t ft::fork(void)
{
	pid_t pid = fork();

	if (pid == -1) 
		throw ft::GenericErrnoExecption();

	return (pid);
}

int32_t ft::execve(const char* file, char* const* argv, char* const* envp)
{
	int32_t val = execve(file, argv, envp);

	if (val == -1) 
		throw ft::GenericErrnoExecption();

	return (val);
}

bool ft::fileExists(const std::string& path)
{
	std::ifstream f(path.c_str());
	return f.good();
}

//////////////////////////////////////////
