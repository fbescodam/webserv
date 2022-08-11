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
	const std::string shit[4] = {
		"GET",
		"POST",
		"DELETE",
		"MAX"
	};
	std::string reqMethodString = shit[static_cast<int32_t>(reqMethod)];

	for (const std::string& val: methods)
		if (val == reqMethodString)
			return (true);
	return (false);
}

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
	filePath = this->config.getcwd() + *(this->config.getValue("path")) + rootPath;
	std::cout << BLACK << "Relative filePath: " << filePath << RESET << std::endl;

	// Get the absolute path to perform security checks
	if (!ft::filesystem::getAbsolutePath(filePath.c_str(), filePath))
		return (this->respondWithStatus(conn, 404)); // Likely a 404 error
	std::cout << BLACK << "Absolute filePath: " << filePath << RESET << std::endl;

	if (filePath.find(this->config.getcwd() + *this->config.getValue("path")) != 0)
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
	static std::list<std::string> methodGet = {{"GET"}};
	bool methodRules = conn.response->pathConfig.getValueAsList("methods", methodList);

	// Checks if request method is in allowed methods
	if (!checkMethods(methodRules ? methodList : methodGet, conn.request->method))
		return (this->respondWithStatus(conn, 405));

	// Check for redirect
	std::list<std::string> redirInfo;
	if (conn.response->pathConfig.getValueAsList("redir", redirInfo))
	{
		conn.response->headers["Location"] = redirInfo.back();
		conn.response->generateStatus(std::stoi(redirInfo.front()));
		return ;
	}

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

/*
void ft::Server::pollInEvent(pollfd* poll)
{
	std::cout << BLACK << "Incoming Request" << RESET << std::endl;

	ssize_t brecv;							// Bytes received
	const size_t BUFF_SIZE = 4096;			// Buffer size
	static char buffer[BUFF_SIZE] = {0};	// The buffer

	// Receive the incoming message
	if ((brecv = recv(poll->fd, buffer, BUFF_SIZE, NONE)) <= 0)
	{
		if (brecv == NONE)
			std::cerr << BLACK << "Connection closed by client" << RESET << std::endl;
		else
			std::cerr << RED << "Receive function has failed!" << RESET << std::endl;

		close(poll->fd);
		poll->fd = -1;
		return;
	}

	// Parse the incoming request
	try
	{
		// TODO: Parse HTTP request
	}
	catch (const ft::PayloadTooLarge& e)
	{
		this->respondWithStatus(poll, 413);
		return;
	}
	catch (const ft::BadRequest& e)
	{
		this->respondWithStatus(poll, 400);
		return;
	}
	catch (const std::bad_alloc& e)
	{
		std::cerr << RED << "Failed to allocate memory" << RESET << std::endl;
		this->respondWithStatus(poll, 507);
		return;
	}
	catch (const std::exception& e)
	{
		std::cerr << RED << "Something went wrong while parsing request" << RESET << std::endl;
		this->respondWithStatus(poll, 500);
		return;
	}

	poll->events = POLLOUT;
}

void ft::Server::pollOutEvent(pollfd* poll)
{
	// TODO: Prevent timeout!
	// TODO: Use response class to send response.

	std::cout << "\033[30;1m" << "Sent Response" << "\033[0m" << std::endl;
}
*/

/*
void ft::Server::run(pollfd* fds, size_t size)
{
	// Check our open fds for events.
	ft::poll(fds, size, 0);

	for (int i = 0; i < size; i++)
	{
		pollfd* poll = &fds[i];

		// Create new connection
		if (!i && (fds[0].revents & POLLIN))
			this->pollListen();
		// Pollfd is ready for reading.
		else if (poll->revents & POLLIN)
			this->pollInEvent(poll);
		// Pollfd is ready for writing.
		else if (poll->revents & POLLOUT)
			this->pollOutEvent(poll);
	}
}
*/
