/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Response.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/27 11:07:35 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/28 13:01:59 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

//////////////////////////////////////////

void ft::Response::writeHead(int32_t status)
{
	// this->data += ft::format("HTTP/1.1 %u %s\n", status, ft::getStatusCodes().at(status).c_str());
	this->data += "HTTP/1.1 " + std::to_string(status) + ft::getStatusCodes().at(status);
	this->headers["Server"] = "Breadserv";
}

void ft::Response::writeHeaders(void)
{
	for (const auto [key, value] : this->headers)
		this->data += key + " : " + value + '\n';
	this->data += "\n";
}

void ft::Response::generateStatus(int32_t status, const std::string& content)
{
	// Build header and fields
	this->writeHead(code);
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
		if (std::remove(filePath.c_str() != 0))
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
