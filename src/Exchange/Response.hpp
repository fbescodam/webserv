/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Response.hpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 19:13:27 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/08 17:54:27 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef RESPONSE_HPP
# define RESPONSE_HPP
# include "Exchange.hpp"
# include "Request.hpp"
# include "CommonNetwork.hpp"
# include "ServerSection.hpp"

FT_BEGIN

enum ResponseStatus {DONE, NOT_DONE};
static const char *enumStrings[] = {"GET", "POST", "DELETE", "ERR"};

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
	Response(int32_t statusIn, ft::ServerSection* configIn);
	Response(ft::Request requestIn, ft::ServerSection* configIn);
	
	~Response();

public: // Functions

	/**
	 * Packages the response object and sends it over the socket.
	 * On failure will throw.
	 *
	 * @param socket The socket over which to send the response.
	 */
	void verify(void);

	/**
	 * Packages the response object and sends it over the socket.
	 * On failure will throw.
	 *
	 * @param socket The socket over which to send the response.
	 */
	ft::ResponseStatus send(int32_t socket);

private:

	// Write the header, meaning version, status into data.
	void writeHeader(int32_t status);

	// Write the fields into data.
	void writeFields(void);

	//open requested file and send it to the socket, should be last
	void parseGet(void);
	//void parsePost(void);
	void parseDelete(void);

	/**
	 * @brief 
	 * 
	 * @param code 
	 */
	void generateStatusPage(int32_t code);

	/**
	 * @brief 
	 * 
	 * @param code 
	 * @param content 
	 */
	void generateStatusPage(int32_t code, std::string content);

public: // Attributes

	// Configuration applied to this response
	ft::Section config;

	// Locations of the section.
	std::vector<ft::Section> locations;

	//the request were basing the response on
	ft::Request request;

	FILE*	file;
	int32_t	fileFd;
};

FT_END
#endif
