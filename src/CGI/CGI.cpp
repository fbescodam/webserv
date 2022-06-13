/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/01 11:51:45 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/13 18:48:35 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

//////////////////////////////////////////

ft::Response ft::CGI::runCGI(const ft::Request& request, const std::string& path)
{
	pid_t pid;
	int32_t fds[2];
	ft::Response outResponse;

	if (!ft::fileExists(path))
		return ft::Response::getError(404);
	try
	{
		ft::pipe(fds);
		if (pid = ft::fork() == 0) // Child
		{
			// TODO: Write wrapper
			if (dup2(fds[READ], STDIN_FILENO) == -1 || \
				dup2(fds[WRITE], STDIN_FILENO) == -1)
				throw ft::GenericErrnoExecption();
			ft::execve(path.c_str(), nullptr, ft::getEnviron());
		}
		else // Parent
		{
			// TODO: Wait for execve, what do here ?
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		outResponse = ft::Response::getError(500);
	}
	
	close(fds[READ]);
	close(fds[WRITE]);

	// TODO: Write child output to response.
	// TODO: Check if outResponse was set, if not return 200.

	return (outResponse);
}

//////////////////////////////////////////
