/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Response.hpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/27 11:07:33 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/08/18 19:53:38 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef RESPONSE_HPP
# define RESPONSE_HPP
# include "Utils.hpp"
# include "Exchange.hpp"
# include "ServerSection.hpp"
# include "DirectoryFactory.hpp"
# include "Filesystem.hpp"
# include "CGI.hpp"

namespace ft {

struct Connection;

/**
 * A Response is from the outgoing server.
 *
 * E.g:
 * HTTP/1.1 503 Service Unavailabe\n
 * Content-Type: text/plain\n
 * Content-Length: 12\n
 * \n
 * 503 error
 */
class Response final : public ft::Exchange
{
public: // Ctor ~ Dtor
	Response(const ft::Connection& conn);
	~Response();

public: // Functions

	/**
	 * @brief Send the current response object onto the socket. It will call whatever job is left to run for sending.
	 * It will always point to sendDynamic, sendHeader or sendFile (private member functions).
	 *
	 * @param socket The client socket.
	 * @return ft::Response::Status The status if we are done or not.
	 */
	ft::Response::Status (ft::Response::*sendRes)(ft::fd_t socket); // Pointer to whatever Response needs to do next

	/**
	 * @brief Let the response generate a response page out of a given status code.
	 *
	 * @param status The status code.
	 * @param content The content of the page. Browser should be able handle even simple strings
	 */
	void generateStatus(int32_t status, const std::string& content);
	void generateStatus(int32_t status);
	static void generateResponse(ft::Connection& conn, int32_t status);

	void deleteMethod(const std::string& filePath);
	void respond(const std::string& filePath);

	void importFieldsForPath(std::string& rootPath);

private:
	void notCGI(const std::string& filePath);

	/**
	 * @brief Use sendDynamic if a header and a body need to be sent in one go. This only occurs if there is no fd.
	 *
	 * @param socket
	 * @return ft::Response::Status
	 */
	ft::Response::Status sendDynamic(ft::fd_t socket);

	/**
	 * @brief Use sendHeader if we only want to send the headers using send, and then a file (fd) using sendfile.
	 *
	 * @param socket
	 * @return ft::Response::Status
	 */
	ft::Response::Status sendHeaders(ft::fd_t socket);

	/**
	 * @brief Use sendFile after using sendHeader. Now it's time to send the contents of a file using C's built-in sendfile method.
	 *
	 * @param socket
	 * @return ft::Response::Status
	 */
	ft::Response::Status sendFile(ft::fd_t socket);

	void writeStatusLine(int32_t status);
	void writeHeaders(void);

public: // Attributes
	ft::Section	pathConfig;
	bool		isDirListing;
private:
	// The current connection this response is targeting.
	const ft::Connection& conn;

	// Copy of the configuration of the current server.
	ft::ServerSection& config;

	// The file to send as a body, else nullptr.
	FILE*	file;
	size_t	fileSize;
	off_t	offset;
};
}
#endif
