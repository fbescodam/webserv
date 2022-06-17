/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Response.hpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 19:13:27 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/17 05:59:09 by pvan-dij      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef RESPONSE_HPP
# define RESPONSE_HPP
# include "Exchange.hpp"
# include "Request.hpp"
# include "ServerSection.hpp"

FT_BEGIN

enum ResponseStatus {DONE, NOT_DONE};

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
class Response final : public Exchange
{
public: // Ctor ~ Dtor
	Response() = default;
	Response(uint32_t inStatus) {status = inStatus;}
	Response(ft::Request req, ft::ServerSection *configIn);
	
	~Response();

public: // Functions

	/**
	 * Packages the response object and sends it over the socket.
	 * On failure will throw.
	 *
	 * @param socket The socket over which to send the response.
	 */
	ft::ResponseStatus send(int32_t socket);

	/**
	 * Constructs an error response based on the status code.
	 *
	 * @param code The status code.
	 * @return The appropriate response based
	 */
	static ft::Response getError(uint32_t code);

private:

	// Write the header, meaning version, status into data.
	void writeHeader(void);

	// Write the fields into data.
	void writeFields(void);

	// Writes the ending of a header.
	void writeEnd(void);

	//open requested file and send it to the socket, should be last
	void parseGet(void);
	void parsePost(void);
	void parseDelete(void);

public: // Attributes

	// The response status code and meaning.
	uint32_t status;

	//the request were basing the response on
	ft::Request req;

	//root filepath
	std::string root;

	//server config
	ft::ServerSection *config;

	//file fd
	FILE *file;
	int32_t fileFd;
	int32_t fileSize;
	off_t fileOffset;
	bool sentHeader;
};

FT_END
#endif
