/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/01 11:51:45 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/17 03:41:39 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

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

/**
 * @see https://www.ibm.com/docs/en/netcoolomnibus/8.1?topic=SSSHTQ_8.1.0/com.ibm.netcool_OMNIbus.doc_8.1.0/webtop/wip/reference/web_cust_envvariablesincgiscripts.html
 */
static std::vector<std::string> get_envp(const ft::Request& request, const std::string& path) 
{
	static std::unordered_map<ft::Method, std::string> const methods = 
	{ 
		{ ft::Method::GET, "GET" },
		{ ft::Method::POST, "POST" },
		{ ft::Method::DELETE, "DELETE" },
	};

	std::vector<std::string> envp = {
		"GATEWAY_INTERFACE=CGI/1.1", 
		"SERVER_PROTOCOL=HTTP/1.1",
		ft::format("REMOTE_ADDR=%d", -1),
		ft::format("REQUEST_METHOD=%s", methods.find(request.method)), // 
		ft::format("SCRIPT_NAME=%s", path.c_str()),
		ft::format("SERVER_NAME=%s", "TODO: Fill in!"),
		ft::format("SERVER_PORT=%s", "TODO: Fill in!"),
		ft::format("SERVER_PORT=%s", "TODO: Fill in!"),
		ft::format("QUERY_STRING=%s", "TODO: Fill in!"),

		// Do we need this ?
		ft::format("CONTENT_LENGTH=%s", "TODO: Fill in!"),
	};

	return (envp);
}

void ft::CGI::runCGI(const ft::Request& request, const std::string& path)
{
	int32_t fds[2];
	ft::Response outResponse;

	if (!ft::filesystem::fileExists(path))
	{
		ft::Response::getError(404).send(request.socket);
		return;
	}

	try
	{
		ft::pipe(fds);

		pid_t pid;
		if ((pid = ft::fork()) == 0)
		{
			// Redirect 
			ft::dup2(fds[READ], STDIN_FILENO);
			ft::dup2(request.socket, STDOUT_FILENO);
			ft::dup2(request.socket, STDERR_FILENO);

			// Convert Cpp => C style
			auto argv = const_cast<char* const*>(c_arr({"TODO: File path"}).data());
			auto envp = const_cast<char* const*>(c_arr(get_envp(request, path)).data());

			// Execve will abandon proccess or throw
			ft::execve(path.c_str(), argv, envp);

			// Close all our pipes
			close(fds[READ]);
			close(fds[WRITE]);
			close(STDOUT_FILENO);
			close(STDERR_FILENO);
			exit(EXIT_FAILURE);
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		ft::Response::getError(500).send(request.socket);
	}
	
	close(fds[READ]);
	close(fds[WRITE]);
}

//////////////////////////////////////////
