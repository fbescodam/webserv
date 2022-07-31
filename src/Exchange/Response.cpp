/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Response.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/27 11:07:35 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/31 13:51:08 by lde-la-h      ########   odam.nl         */
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
	// Find custom page
	std::string errorPage = "error_" + std::to_string(status);
	if (!this->config.keyExists(errorPage))
	{
		const std::string* path = this->config.getValue("path");
		const std::string* page = this->config.getValue(errorPage);
		if (!path || !page)
			goto generic;

		std::string filePath(*path + *page);
		if ((this->file = fopen(filePath.c_str(), "r")))
			goto generic;
		
		this->writeStatusLine(status);
		this->headers["Content-Length"] = std::to_string(ft::filesystem::getFileSize(this->file));
		this->headers["Content-Type"] = ft::getContentType(filePath);
		this->writeHeaders();
		return;
	}
	
generic: // Send generic built-in page.
	std::string statusText(ft::getStatusCodes().at(status)); // TODO: Retrieve pig image.
	this->generateStatus(status, "<!DOCTYPE html><html><head><title>" + statusText + "</title></head><body><h1>" + std::to_string(status) + " " + statusText + "</h1></body></html>");
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
		return;
	}
	this->generateStatus(404);
}

void ft::Response::postMethod(const std::string& filePath)
{
	// TODO: Only support single cgi ?
	std::list<std::string> cgiBin;
	
	if (!ft::filesystem::fileExists(filePath))
		this->generateStatus(404);
	else if (this->config.getValueAsList("cgi_bin", cgiBin))
	{
		std::string out;
		if (!ft::CGI::runCGI(this, filePath, out, cgiBin.back()))
			this->generateStatus(500); return;
		this->data = out;
		return;
	}
	this->generateStatus(400);
}

void ft::Response::getMethod(void)
{
	// TODO: ???
}

ft::Response::Status send(int32_t socket)
{
	// Send header first with send, then check if file is set to send body with send or sendfile
	// Send body via sendfile

	off_t len = 0;
	if (sendfile(this->fileFd, socket, this->fileOffset, &len, NULL, 0) < 0)
		std::cerr << RED << "Sendfile function for response failed" << RESET << std::endl;
	this->fileOffset += len;

	return (this->fileOffset >= this->fileSize ? ft::Response::Status::DONE : ft::Response::Status::NOT_DONE);
}

//////////////////////////////////////////
