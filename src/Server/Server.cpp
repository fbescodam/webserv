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

	conn.response = new ft::Response(conn); // TODO try catch this maybe, if it fails, close the connection
	conn.response->generateStatus(statusCode);

	// TODO: Do something will poll ?
}

void ft::Server::setSocket(const ft::Socket* socket)
{
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

	std::cout << BLACK << "Now handling the request" << RESET << std::endl;

	try
	{
		conn.response = new ft::Response(conn);
		std::cout << BLACK << "Requested path: " << conn.request->path << RESET << std::endl;
		rootPath = conn.request->path.substr(0, conn.request->path.find_first_of('?'));
		// Note: no need to split on #, the anchor/fragment identifier is not sent as part of the request
		conn.response->importFieldsForPath(rootPath);
	}
	catch (const std::exception& e)
	{
		std::cerr << RED << e.what() << RESET << std::endl;

		try { this->respondWithStatus(conn, 500); }
		catch (const std::exception& e) { exit(EXIT_FAILURE); }
	}
	conn.response->pathConfig.print("pathConfig\t");

	if (!conn.response->pathConfig.returnValueAsBoolean("access"))
		return (this->respondWithStatus(conn, 403));

	// Get actual path used for IO
	const std::string* path = this->config.getValue("path");
	if (!path)
		return (this->respondWithStatus(conn, 500));

	filePath = this->config.getcwd() + *path + rootPath; // Get the FULL PATH to the file, starting at the root /
	std::cout << BLACK << "Relative filePath: " << filePath << RESET << std::endl;

	// Check for redirect
	std::list<std::string> redirInfo;
	std::cout << BLACK << "Check redir" << RESET << std::endl;
	if (conn.response->pathConfig.getValueAsList("redir", redirInfo))
	{
		std::cout << GREEN << "Redir found, redirecting" << RESET << std::endl;
		conn.response->headers["Location"] = redirInfo.back();
		conn.response->generateStatus(std::stoi(redirInfo.front()));
		return;
	}
	std::cout << BLACK << "No redir found" << RESET << std::endl;

	//TODO: doing this before redir caused issues because redir directory did not exist, moved it up for now
	// Get the absolute path to perform security checks
	if (!ft::filesystem::getAbsolutePath(filePath.c_str(), filePath))
		return (this->respondWithStatus(conn, 404)); // Likely a 404 error
	std::cout << BLACK << "Absolute filePath: " << filePath << RESET << std::endl;

	if (filePath.find(this->config.getcwd() + *path) != 0)
	{
		std::cout << RED << "[WARNING] Requested path is outside of the root defined in the server config!" << RESET << std::endl;
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

		std::cout << BLACK << "Requested path is a directory, checking if index file exists at " << filePath << indexFile << RESET << std::endl;
		if (ft::filesystem::fileExists(filePath + indexFile))
			filePath = filePath + indexFile;
		else if (!conn.response->pathConfig.returnValueAsBoolean("dir_listing"))
		{
			std::cout << BLACK << "Index file does not exist and dir_listing is not enabled, responding with 404" << RESET << std::endl;
			return (this->respondWithStatus(conn, 404));
		}
		else if (!ft::filesystem::isDir(filePath))
		{
			std::cout << BLACK << "Index file does not exist, dir_listing is enabled, but directory does not exist, responding with 404" << RESET << std::endl;
			return (this->respondWithStatus(conn, 404));
		}
		else
			conn.response->isDirListing = true;
		std::cout << BLACK << "Index file exists or dir_listing is enabled, new filePath is " << filePath << RESET << std::endl;
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
			return (conn.response->getMethod(filePath));
		case ft::Exchange::Method::POST:
			return (conn.response->postMethod(filePath));
		case ft::Exchange::Method::DELETE:
			return (conn.response->deleteMethod(filePath));
		default: break; // Do nothing
	}
}

//////////////////////////////////////////
