/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/01 11:51:45 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/22 13:55:14 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"
#include "GlobalConfig.hpp"

//////////////////////////////////////////

static std::vector<const char*> c_arr(const std::vector<std::string> &v) 
{
	std::vector<const char*> c_arr;

	for (const std::string &val: v)
		c_arr.push_back(val.c_str());

	c_arr.push_back(NULL);
	return (c_arr);
}

bool ft::CGI::runCGI(const ft::Response& response, const std::string& path, std::string &out, const std::string &cgiBin)
{
	int32_t fds[2];
	int32_t body_pipe[2];

	std::vector<std::string> argv = {cgiBin, path};
	std::vector<std::string> envpp;
	envpp.push_back("GATEWAY_INTERFACE=CGI/1.1");
	envpp.push_back("REMOTE_ADDR=" + response.request->ipv4);
	envpp.push_back("REQUEST_METHOD=POST");
	envpp.push_back("SCRIPT_NAME=" + path);
	envpp.push_back("SERVER_NAME=localhost");
	envpp.push_back("SERVER_PROTOCOL=HTTP/1.1");
	envpp.push_back("PATH_INFO=~/work/webserv/examples/www/post/fileupload.sh");
	envpp.push_back("CONTENT_LENGTH=" + std::to_string(response.request->body.size()));

	// Create pipes
	try { ft::pipe(fds); ft::pipe(body_pipe); }
	catch(const std::exception& e)
	{
		close(fds[WRITE]);
		close(fds[READ]);
		close(body_pipe[READ]);
		close(body_pipe[WRITE]);

		std::cerr << e.what() << std::endl;
		return (false);
	}

	pid_t pid;
	try { pid = ft::fork(); }
	catch (std::exception &e)
	{
		close(fds[WRITE]);
		close(fds[READ]);

		close(body_pipe[READ]);
		close(body_pipe[WRITE]);

		std::cerr << e.what() << std::endl;
		return (false);
	}

	if (pid == 0) // Child
	{
		::dup2(body_pipe[READ], STDIN_FILENO);
		::dup2(fds[WRITE], STDOUT_FILENO);
		::dup2(fds[WRITE], STDERR_FILENO);
		
		close(body_pipe[WRITE]);
		close(fds[WRITE]);
		close(fds[READ]);

		::execve(cgiBin.c_str(), (char *const *)c_arr(argv).data(), (char *const *)c_arr(envpp).data());
		close(body_pipe[READ]);
		exit(EXIT_FAILURE);
	}
	else // Parent
	{
		if (!response.request->body.empty())
			write(body_pipe[WRITE], response.request->body.data(), response.request->body.size());

		close(body_pipe[WRITE]);
		close(body_pipe[READ]);
		close(fds[WRITE]);

		char buff[4096] = {0};
		while (read(fds[READ], buff, sizeof(buff)) > 0)
		{
			out += buff;
			bzero(buff, sizeof(buff));
		}
		close(fds[READ]);
		wait(NULL);
	}
	return (true);
}

//////////////////////////////////////////
