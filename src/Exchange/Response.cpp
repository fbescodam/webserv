/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Response.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/27 11:07:35 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/31 17:44:20 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

ft::Response::Response(const ft::Connection& conn) : conn(conn) { } // TODO: Response should take a status code and config as well
// TODO: Second constructor for request, config and conn

//////////////////////////////////////////

void ft::Response::writeStatusLine(int32_t status)
{
	this->data += "HTTP/1.1 " + std::to_string(status) + ft::getStatusCodes().at(status);
	this->headers["Server"] = "Breadserv";
}

//////////////////////////////////////////

void ft::Response::writeHeaders(void)
{
	for (const auto [key, value] : this->headers)
		this->data += key + " : " + value + '\n';
	this->data += "\n";
}

//////////////////////////////////////////

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
		if ((this->file = fopen(filePath.c_str(), "r")) == nullptr)
			goto generic;

		this->writeStatusLine(status);
		this->headers["Content-Length"] = std::to_string(ft::filesystem::getFileSize(this->file));
		this->headers["Content-Type"] = ft::getContentType(filePath);
		this->writeHeaders();
		this->fileSize = ft::filesystem::getFileSize(this->file);
		this->sendRes = &ft::Response::sendHeaders;
		return;
	}

generic: // Send generic built-in page.
	std::string statusText(ft::getStatusCodes().at(status)); // TODO: Retrieve pig image.
	this->generateStatus(status, "<!DOCTYPE html><html><head><title>" + statusText + "</title></head><body><h1>" + std::to_string(status) + " " + statusText + "</h1></body></html>");
}

//////////////////////////////////////////

void ft::Response::generateStatus(int32_t status, const std::string& content)
{
	// Build header and fields
	this->writeStatusLine(status);
	this->headers["Content-Length"] = std::to_string(content.length());
	this->headers["Content-Type"] = "text/html";
	this->writeHeaders();

	// Build content
	this->data += content;
	this->sendRes = &ft::Response::sendStatic;
}

//////////////////////////////////////////

void ft::Response::deleteMethod(const std::string& filePath)
{
	if (!ft::filesystem::fileExists(filePath))
		return (this->generateStatus(404));

	if (std::remove(filePath.c_str()) != 0)
		return (this->generateStatus(500, "File deletion failed!"));
	this->generateStatus(200, "File deleted");
}

//////////////////////////////////////////

void ft::Response::postMethod(const std::string& filePath)
{
	std::list<std::string> cgiBin;

	if (!ft::filesystem::fileExists(filePath))
		this->generateStatus(404);
	else if (this->config.getValueAsList("cgi_bin", cgiBin) && endsWith(filePath, cgiBin.front()))
	{
		std::string out;
		if (!ft::CGI::runCGI(this->conn, filePath, out, cgiBin.back()))
		{
			this->generateStatus(500);
			return;
		}
		this->data = out;
		this->sendRes = &ft::Response::sendStatic;
		return;
	}

	// Nothing to post so we treat it as a get method.
	this->getMethod(filePath);
}

//////////////////////////////////////////

// Simply open the file and send it over.
void ft::Response::getMethod(const std::string& filePath)
{
	if (!ft::filesystem::fileExists(filePath))
		return (this->generateStatus(404));

	if ((this->file = fopen(filePath.c_str(), "r")) == nullptr)
		return (this->generateStatus(500));

	this->fileSize = ft::filesystem::getFileSize(this->file);
	this->sendRes = &ft::Response::sendHeaders;
}

//////////////////////////////////////////

ft::Response::Status ft::Response::sendStatic(int32_t socket)
{
	size_t bsent = ft::send(socket, this->data.data(), this->data.length(), NONE); // Send as much as possible
	if (bsent < this->data.length()) // Not everything was sent, send more in the next poll
	{
		this->data.erase(0, bsent);
		return (ft::Response::Status::NOT_DONE);
	}
	this->sendRes = nullptr; // Everything was sent, nothing more to do
	return (ft::Response::Status::DONE);
}

//////////////////////////////////////////

ft::Response::Status ft::Response::sendHeaders(int32_t socket)
{
	size_t bsent = ft::send(socket, this->data.data(), this->data.length(), NONE);
	if (bsent < this->data.length()) // Not everything was sent, send more in the next poll
	{
		this->data.erase(0, bsent);
		return (ft::Response::Status::NOT_DONE);
	}
	this->sendRes = &ft::Response::sendFile; // Everything was sent, now continue with sending the file
	return (ft::Response::Status::NOT_DONE); // Actually not done yet! We now need to send the file
}

//////////////////////////////////////////

ft::Response::Status ft::Response::sendFile(int32_t socket)
{
	off_t bsent = 0;
	static off_t offset = 0;

	if (this->file == nullptr)
		throw std::exception();

	sendfile(fileno(this->file), socket, offset, &bsent, NULL, NONE);
	offset += bsent;

	if (offset >= this->fileSize)
	{
		this->sendRes = nullptr; // Everything was sent, nothing more to do
		return (ft::Response::Status::DONE);
	}
	return (ft::Response::Status::NOT_DONE);
}

//////////////////////////////////////////
