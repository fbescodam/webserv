/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: fbes <fbes@student.codam.nl>                 +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/31 16:27:42 by fbes          #+#    #+#                 */
/*   Updated: 2022/08/18 16:32:04 by pvan-dij      ########   odam.nl         */
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
	auto servPath = conn.server->config.getValue("path");
	auto uploadPath = conn.server->config.getValue("upload_dir");
	auto servName = conn.server->config.getValue("server_name");
	if (!servName || !servPath || !uploadPath) return (false);
	
    std::map<ft::Exchange::Method, std::string> methods = {
        {ft::Exchange::Method::GET, "GET"},
        {ft::Exchange::Method::POST, "POST"},
        {ft::Exchange::Method::DELETE, "DELETE"},
    };

	std::vector<std::string> argv = {cgiBin, path};
	std::vector<std::string> envp;
	try
	{
		size_t queryPos = conn.request->path.find_first_of('?');
		envp.push_back("QUERY_STRING=" + (queryPos != std::string::npos ? conn.request->path.substr(queryPos) : ""));
		envp.push_back("GATEWAY_INTERFACE=CGI/1.1");
		envp.push_back("SERVER_PROTOCOL=HTTP/1.1");
		envp.push_back("REMOTE_ADDR=" + conn.ipv4);
		envp.push_back("REQUEST_METHOD=" + methods[conn.request->method]);
		envp.push_back("SCRIPT_NAME=" + path.substr(path.find_last_of("/") + 1));
		envp.push_back("SERVER_NAME=" + *servName);
		envp.push_back("PATH_INFO=" + path);
		envp.push_back("CONTENT_LENGTH=" + conn.request->headers["content-length"]);
		envp.push_back("UPLOAD_DIR=" + conn.server->config.getcwd() + *servPath + *uploadPath);
		envp.push_back("UPLOAD_PATH=" + *uploadPath);
	}
	catch(...) { return (false); } // Allocation failure or something.

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
