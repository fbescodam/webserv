/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Response.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/27 11:07:35 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/28 21:58:15 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

//////////////////////////////////////////

void ft::Response::writeStatusLine(int32_t status)
{
	this->data += "HTTP/1.1 " + std::to_string(status) + ft::getStatusCodes().at(status);
	this->headers["Server"] = "Breadserv";
}

void ft::Response::writeHeaders(void)
{
	for (const auto [key, value] : this->headers)
		this->data += key + " : " + value + '\n';
	this->data += "\n";
}

void ft::Response::generateStatus(int32_t status)
{
	// Build header and fields
	this->writeStatusLine(status);

	// Try to find custom error page
	if (this->config.)
	{
		/* code */
	}

}

void ft::Response::generateStatus(int32_t status, const std::string& content)
{
	// Build header and fields
	this->writeStatusLine(status);
	this->headers["Content-Length"] = std::to_string(content.length());
	this->headers["Content-Type"] = "text/html";
	this->writeHeaders();

	// Build content
	this->data += content;
}

void ft::Response::deleteMethod(const std::string& filePath)
{
	if (ft::filesystem::fileExists(filePath))
	{
		if (std::remove(filePath.c_str()) != 0)
			this->generateStatus(500, "File deletion failed!");
		else
			this->generateStatus(200, "File deleted");
	}
	else
		this->generateStatus(404, "");
}

void ft::Response::postMethod(const std::string& filePath)
{
   // TODO: ???
}

void ft::Response::getMethod(void)
{
	// TODO: ???
}

ft::Response::Status send(int32_t socket)
{
	// TODO: What ?

	off_t len = 0;
	if (sendfile(this->fileFd, socket, this->fileOffset, &len, NULL, 0) < 0)
		std::cerr << RED << "Sendfile function for response failed" << RESET << std::endl;
	this->fileOffset += len;

	return (this->fileOffset >= this->fileSize ? ft::Response::Status::DONE : ft::Response::Status::NOT_DONE);
}

//////////////////////////////////////////
