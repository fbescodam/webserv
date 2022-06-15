/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/01 11:51:45 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/15 15:03:01 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

//////////////////////////////////////////

ft::Response ft::CGI::runCGI(const ft::Request& request, const std::string& path)
{
	int32_t fds[2];
	ft::Response outResponse;

	if (!ft::fileExists(path))
		return ft::Response::getError(404);
	try
	{
		ft::pipe(fds);

		pid_t pid;
		if ((pid = ft::fork()) == 0)
		{
			// Child
			ft::dup2(fds[READ], STDIN_FILENO);
			ft::dup2(fds[WRITE], STDOUT_FILENO);
			ft::dup2(fds[WRITE], STDERR_FILENO);

			// TODO: Feed correct argv and envp values
			// Execve will abandon proccess or throw
			ft::execve(path.c_str(), nullptr, nullptr);
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		outResponse = ft::Response::getError(500);
	}
	
	// Read content of 
	outResponse.data += "Bruh";
	
	close(fds[READ]);
	close(fds[WRITE]);

	// TODO: Write child output to response.
	// TODO: Check if outResponse was set, if not return 200.

	return (outResponse);
}

//////////////////////////////////////////
