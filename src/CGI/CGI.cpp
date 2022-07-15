/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/01 11:51:45 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/15 17:06:39 by pvan-dij      ########   odam.nl         */
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

/**
 * @see https://www.ibm.com/docs/en/netcoolomnibus/8.1?topic=SSSHTQ_8.1.0/com.ibm.netcool_OMNIbus.doc_8.1.0/webtop/wip/reference/web_cust_envvariablesincgiscripts.html
 */
static std::vector<std::string> get_envp(const ft::Response& response, const std::string& path) 
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
		ft::format("REMOTE_ADDR=%s", response.request->ipv4.c_str()), //TODO: could be null ig
		ft::format("REQUEST_METHOD=%s", methods.find(response.request->method)),
		ft::format("SCRIPT_NAME=%s", path.c_str()),
		ft::format("SERVER_NAME=%s", response.config.getValue("server_names")->c_str()), // TODO: only get one server name (the first one)
		ft::format("SERVER_PORT=%s", response.config.getValue("listen")->c_str()), //TODO: get value can return NULL
		//ft::format("QUERY_STRING=%s", "TODO: Fill in!"), // 
		// ft::format("CONTENT_LENGTH=%s", request.body.length()),
	};

	return (envp);
}

void ft::CGI::runCGI(const ft::Response& response, const std::string& path, std::string &out)
{
	int32_t fds[2];
	ft::Response outResponse;

	char* const bruh[999] = {
		"/bin/sh",
		const_cast<char *>(path.c_str()),
		nullptr,
	};

	if (!ft::filesystem::fileExists(path))
	{
		// ft::Response::getError(404).send(response.socket);
		return;
	}

	try
	{
		ft::pipe(fds);

		pid_t pid;
		if ((pid = ft::fork()) == 0)
		{
			// Redirect 
			// ft::dup2(fds[READ], STDIN_FILENO);
			ft::dup2(fds[WRITE], STDOUT_FILENO);
			//ft::dup2(request.socket, STDERR_FILENO);

			// Convert Cpp => C style
			auto argv = const_cast<char* const*>(c_arr({"TODO: File path"}).data());
			auto envp = const_cast<char* const*>(c_arr(get_envp(response, path)).data());

			// Execve will abandon proccess or throw
			// ft::execve("examples/www/cgi_tester", NULL, NULL);
			ft::execve("/bin/sh", bruh, NULL);

			// Close all our pipes
			close(fds[READ]);
			close(fds[WRITE]);
			close(STDOUT_FILENO);
			close(STDERR_FILENO);
			exit(EXIT_FAILURE);
		}
		waitpid(pid, NULL, 0);
		close(fds[WRITE]);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		// ft::Response::getError(500).send(response.socket);
	}


	#define BUFFER_SIZE 4096
	char buf[BUFFER_SIZE + 1] = {0};
	while (read(fds[READ], buf, BUFFER_SIZE) > 0)
	{
		out += buf;
		bzero(buf, BUFFER_SIZE);
	}
	//TODO: use binary stream for potential images

	close(fds[READ]);
	close(fds[WRITE]);
}

//////////////////////////////////////////
