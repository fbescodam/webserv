/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/01 11:51:45 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/19 21:01:09 by pvan-dij      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"
#include "GlobalConfig.hpp"

//////////////////////////////////////////

// TODO: Move these maybe

static std::vector<const char*> c_arr(const std::vector<std::string>& array) 
{
	std::vector<const char*> out(array.size() + 1);

	for (auto& elem : array)
		out.push_back(elem.c_str());
	out.push_back(nullptr);

	return (out);
}

void ft::CGI::runCGI(const ft::Response& response, const std::string& path, std::string &out, const std::string &cgiBin)
{
	int32_t fds[2];

	std::vector<std::string> envpp;
	envpp.push_back("GATEWAY_INTERFACE=CGI/1.1");
	envpp.push_back("REMOTE_ADDR=" + response.request->ipv4);
	envpp.push_back("REQUEST_METHOD=POST");
	envpp.push_back("SCRIPT_NAME=" + path);
	envpp.push_back("SERVER_NAME=localhost");
	envpp.push_back("SERVER_PROTOCOL=HTTP/1.1");
	envpp.push_back("CONTENT_LENGTH=" + std::to_string(response.request->body.size()));

	try
	{
		ft::pipe(fds);

		pid_t pid;
		if ((pid = ft::fork()) == 0)
		{
			ft::dup2(fds[READ], STDIN_FILENO);
			ft::dup2(fds[WRITE], STDOUT_FILENO);
			ft::dup2(fds[WRITE], STDERR_FILENO);

			write(STDIN_FILENO, response.request->body.c_str(), response.request->body.size());

			char* const* argv = const_cast<char* const*>(c_arr({cgiBin, path}).data());
			char* const* envp = const_cast<char* const*>(c_arr(envpp).data());

			ft::execve(cgiBin, argv, envp);
		}
		else // Parent
		{
			size_t bread;
			char buf[4096] = {0};
			while ((bread = read(fds[READ], buf, sizeof(buf))) > 0)
			{
				std::cerr << "lol\n";
				out += buf;
				bzero(buf, sizeof(buf));
				if (bread < sizeof(buf))
					close(fds[READ]);
			}
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}

//////////////////////////////////////////

/*

	int32_t fds[2];
	ft::Response outResponse;
	std::vector<std::string> envpp;
	envpp.push_back("GATEWAY_INTERFACE=CGI/1.1");
	envpp.push_back("REMOTE_ADDR=" + response.request->ipv4);
	envpp.push_back("REQUEST_METHOD=POST");
	envpp.push_back("SCRIPT_NAME=" + path);
	envpp.push_back("SERVER_NAME=localhost");
	envpp.push_back("SERVER_PROTOCOL=HTTP/1.1");
	envpp.push_back("CONTENT_LENGTH=" + std::to_string(response.request->body.size()));

	try
	{
		ft::pipe(fds);

		pid_t pid;
		if ((pid = ft::fork()) == 0)
		{
			// Redirect 
			ft::dup2(fds[READ], STDIN_FILENO);
			ft::dup2(fds[WRITE], STDOUT_FILENO);

			// Convert Cpp => C style
			//auto argv = const_cast<char* const*>(c_arr({cgiBin, path}).data());
			//auto envp = const_cast<char* const*>(c_arr(get_envp(response, path)).data());


			auto argvv = c_arr({cgiBin, path});
			char* const* argv = const_cast<char* const*>(argvv.data());

			auto envpv = c_arr(envpp);
			char* const* envp = const_cast<char* const*>(envpv.data());
			//char* const* envp

			// Execve will abandon proccess or throw
			ft::execve(cgiBin, argv, envp);
			// Close all our pipes
			close(fds[READ]);
			close(fds[WRITE]);
			close(STDOUT_FILENO);
			close(STDERR_FILENO);
			exit(EXIT_FAILURE);
		}
		ft::dup2(fds[WRITE], STDIN_FILENO);
		write(fds[WRITE], response.request->body.c_str(), response.request->body.size());
		close(fds[WRITE]);
		waitpid(-1, NULL, 0);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}


	ft::dup2(fds[READ], STDOUT_FILENO);
	#define BUFFER_SIZE 4096
	char buf[BUFFER_SIZE + 1] = {0};
	while (read(fds[READ], buf, BUFFER_SIZE) > 0)
	{
		out += buf;
		bzero(buf, BUFFER_SIZE);
	}
	//TODO: use binary stream for potential images
	close(fds[READ]);
*/