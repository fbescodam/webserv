/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: fbes <fbes@student.codam.nl>                 +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/31 16:27:42 by fbes          #+#    #+#                 */
/*   Updated: 2022/08/12 15:58:33 by pvan-dij      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

static std::vector<const char*> c_arr(const std::vector<std::string> &v)
{
	std::vector<const char*> c_arr;

	for (const std::string &val: v)
		c_arr.push_back(val.c_str());
	c_arr.push_back(NULL);
	return (c_arr);
}

bool ft::CGI::runCGI(const ft::Connection& conn, const std::string& path, std::string& out, const std::string& cgiBin)
{
	std::vector<std::string> argv = {cgiBin, path};
	std::vector<std::string> envp;
	envp.push_back("GATEWAY_INTERFACE=CGI/1.1");
	envp.push_back("REMOTE_ADDR=" + conn.ipv4);
	envp.push_back("REQUEST_METHOD=POST");
	envp.push_back("SCRIPT_NAME=" + path);
	envp.push_back("SERVER_NAME=localhost");
	envp.push_back("SERVER_PROTOCOL=HTTP/1.1");
	envp.push_back("PATH_INFO=~/work/webserv/examples/www/post/fileupload.sh"); // TODO: Remove abs path
	envp.push_back("CONTENT_LENGTH=" + conn.request->headers["content-length"]);

	std::cout << RED << "Entering cgi"<<RESET<<std::endl;

	int32_t fds[2];
	int32_t body_pipe[2];
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

		::execve(cgiBin.c_str(), (char *const *)c_arr(argv).data(), (char *const *)c_arr(envp).data());
		close(body_pipe[READ]);
		exit(EXIT_FAILURE);
	}
	else // Parent
	{
		std::string fullData = conn.request->header_data + "\r\n\r\n" + conn.request->data;
		// std::cout <<fullData;

		if (!conn.request->data.empty())
			size_t bread = write(body_pipe[WRITE],fullData.data(),fullData.size());

		close(body_pipe[WRITE]);
		close(body_pipe[READ]);
		close(fds[WRITE]);

		wait(NULL);
		char buff[4096] = {0};
		while (read(fds[READ], buff, sizeof(buff)) > 0)
		{
			out += buff;
			bzero(buff, sizeof(buff));
		}
		close(fds[READ]);
	}

	std::cout << RED << "Exiting cgi"<<RESET<<std::endl;
	std::cout << out<<std::endl;
	std::cout << out.size()<<std::endl;

	return (true);
}
