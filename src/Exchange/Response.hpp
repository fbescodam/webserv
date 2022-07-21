/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Response.hpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 19:13:27 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/21 17:37:32 by pvan-dij      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef RESPONSE_HPP
# define RESPONSE_HPP
# include "Exchange.hpp"
# include "Request.hpp"
# include "CommonNetwork.hpp"
# include "ServerSection.hpp"
# include "DirectoryFactory.hpp"
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
	Response(ft::Request *requestIn, ft::ServerSection* configIn);
	
	~Response();

public: // Functions

	/**
	 * Packages the response object and sends it over the socket.
	 * On failure will throw.
	 *
	 * @param socket The socket over which to send the response.
	 */
	bool verify(void);

	void generateResponse();

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

	//delete method
	void deleteMethod(std::string filePath);

	//post method
	void postMethod(std::string filePath);

	//yes
	void retrievePigPath(std::string &name);


public: // Attributes

	// Configuration applied to this response
	ft::Section config;

	// Locations of the section.
	std::vector<ft::Section> locations;

	//the request were basing the response on
	ft::Request *request;

	FILE*	file;
	int32_t	fileFd;

	//for send function
	bool sentHeader;
	size_t fileOffset;
	size_t fileSize;
};

FT_END
#endif
