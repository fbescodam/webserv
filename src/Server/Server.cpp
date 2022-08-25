/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/27 11:08:42 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/08/09 16:29:440 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

ft::Server::Server(ft::ServerSection& inConfig) : config(inConfig) {}

//////////////////////////////////////////

// Direclty creates a whole new response.
void ft::Server::respondWithStatus(ft::Connection& conn, int32_t statusCode)
{
	// Yeet out whatever response we had before.
	delete conn.response;

	conn.response = new ft::Response(conn);
	conn.response->generateStatus(statusCode);
}

void ft::Server::setSocket(const ft::Socket* socket)
{
	LOG("Socket for server " << *this->config.getValue("server_name") << " set to fd " << socket->fd);
	this->socket = const_cast<ft::Socket*>(socket);
}

const ft::Socket* ft::Server::getSocket(void) const
{
	return (this->socket);
}

bool checkMethods(const std::list<std::string> &methods, ft::Exchange::Method reqMethod)
{
	const std::string stringedMethods[4] = {
		"GET",
		"POST",
		"DELETE",
		"MAX"
	};
	std::string reqMethodString = stringedMethods[static_cast<int32_t>(reqMethod)];

	for (const std::string& val: methods)
		if (val == reqMethodString)
			return (true);
	return (false);
}

// My god this shit is unreadable ...
void ft::Server::handleRequest(ft::Connection& conn)
{
	std::string rootPath; // where the request is rooted
	std::string filePath; // full path to the file requested (including server path)

	LOG("Now handling the request");

	try
	{
		conn.response = new ft::Response(conn);
		LOG("Requested path: " << conn.request->path);
		rootPath = conn.request->path.substr(0, conn.request->path.find_first_of('?'));
		// Note: no need to split on #, the anchor/fragment identifier is not sent as part of the request
		conn.response->importFieldsForPath(rootPath);
	}
	catch (const std::exception& e)
	{
		ERR("Exception occurred on request handling: " << e.what());
		try { this->respondWithStatus(conn, 500); }
		catch (const std::exception& e) { exit(EXIT_FAILURE); }
	}
	conn.response->pathConfig.print("pathConfig\t");

	if (!conn.response->pathConfig.returnValueAsBoolean("access"))
		return (this->respondWithStatus(conn, 403));

	// Get actual path used for IO
	const std::string* path = this->config.getValue("path");
	if (!path)
	{
		ERR("No path set in request config!");
		return (this->respondWithStatus(conn, 500));
	}

	filePath = this->config.getcwd() + *path + rootPath; // Get the FULL PATH to the file, starting at the root /
	LOG("Relative filePath: " << filePath);

	// Check for redirect
	std::list<std::string> redirInfo;
	LOG("Check redir");
	if (conn.response->pathConfig.getValueAsList("redir", redirInfo))
	{
		std::cout << GREEN << "Redir found, redirecting" << RESET << std::endl;
		conn.response->headers["Location"] = redirInfo.back();
		conn.response->generateStatus(std::stoi(redirInfo.front()));
		return;
	}
	LOG("No redir found");

	// Get the absolute path to perform security checks
	if (!ft::filesystem::getAbsolutePath(filePath.c_str(), filePath))
		return (this->respondWithStatus(conn, 404)); // Likely a 404 error
	LOG("Absolute filePath: " << filePath);

	if (filePath.find(this->config.getcwd() + *path) != 0)
	{
		ERR("[WARNING] Requested path is outside of the root defined in the server config!");
		return (this->respondWithStatus(conn, 403));
	}

	// Modify path for directory reading if path is a directory
	if (ft::filesystem::isDir(filePath) && filePath.back() != '/')
		filePath += "/";

	// Run directory checks
	if (filePath.back() == '/')
	{
		std::string indexFile = "index.html"; // default
		if (conn.response->pathConfig.keyExists("index"))
			indexFile = *conn.response->pathConfig.getValue("index");

		LOG("Requested path is a directory, checking if index file exists at " << filePath << indexFile);
		if (ft::filesystem::fileExists(filePath + indexFile))
			filePath = filePath + indexFile;
		else if (!conn.response->pathConfig.returnValueAsBoolean("dir_listing"))
		{
			LOG("Index file does not exist and dir_listing is not enabled, responding with 404");
			return (this->respondWithStatus(conn, 404));
		}
		else if (!ft::filesystem::isDir(filePath))
		{
			LOG("Index file does not exist, dir_listing is enabled, but directory does not exist, responding with 404");
			return (this->respondWithStatus(conn, 404));
		}
		else
			conn.response->isDirListing = true;
		LOG("Index file exists or dir_listing is enabled, new filePath is " << filePath);
	}

	// Gets the allowed methods for path
	std::list<std::string> methodList;
	static std::list<std::string> methodGet = {{"GET"}}; // Default allowed methods, if methods is not set in config
	bool methodRules = conn.response->pathConfig.getValueAsList("methods", methodList);
	if (!checkMethods(methodRules ? methodList : methodGet, conn.request->method))
		return (this->respondWithStatus(conn, 405));

	// If something fails within the GET, POST or DELETE methods
	// they set the appropriate content, worst case they kill the app.
	switch (conn.request->method)
	{
		case ft::Exchange::Method::GET:
		case ft::Exchange::Method::POST:
			return (conn.response->respond(filePath));
		case ft::Exchange::Method::DELETE:
			return (conn.response->deleteMethod(filePath));
		default: break; // Do nothing
	}
}

//////////////////////////////////////////
