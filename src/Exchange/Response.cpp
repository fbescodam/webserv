/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Response.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 19:34:00 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/17 06:56:16 by pvan-dij      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "Request.hpp"
#include "ServerSection.hpp"
#include "CommonUnix.hpp"

//////////////////////////////////////////

ft::Response::~Response()
{
	fclose(this->file);
	close(this->fileFd);
}

ft::Response::Response(Request reqIn, ft::ServerSection *configIn)
{
	// TODO: This should properly construct a response based on the earlier received request
	// data = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
	// reqIn.display();

	this->req = reqIn;
	this->config = configIn;
	this->sentHeader = false;
	this->fileOffset = 0;
	this->fileFd = -1;

	if (this->req.keyExists("Connection") && *this->req.getValue("Connection") == "keep-alive")
	{
		this->fields["Connection"] = "keep-alive";
		this->fields["Keep-Alive"] = "timeout=" + std::to_string(S_TIMEOUT);
	}
	else
		this->fields["Connection"] = "close";

	switch(this->req.method)
	{
		case (ft::Method::GET):
			this->parseGet(); break;
		case (ft::Method::POST):
			this->parsePost(); break;
		case (ft::Method::DELETE):
			this->parseDelete(); break;
		default:
			this->parseError(405);
	}
}

//////////////////////////////////////////

void ft::Response::parseError(int code)
{
	this->status = code;
	const std::string& statusText = ft::getStatusCodes().at(code);
	const std::string& content = "<!DOCTYPE html><html><head><title>"+statusText+"</title></head><body><h1>"+std::to_string(code)+" "+statusText+"</h1></body></html>";

	// Build header and fields
	this->writeHeader();
	this->fields["Content-Length"] = std::to_string(content.length());
	this->fields["Content-Type"] = "text/html";
	this->writeFields();
	this->writeEnd();

	// Build content
	this->data += content;
}

void ft::Response::parseGet(void)
{
	if (this->req.path == "/")
		this->req.path = "/index.html";

	if (ft::filesystem::fileExists(this->config->root + this->req.path))
	{
		this->file = fopen((this->config->root + this->req.path).data(), "r"); 
		this->status = 200;
	}
	else
	{
		if (!this->req.config->keyExists("error_404"))
			return (this->parseError(404)); // no custom 404 page found, generate one on the fly
		this->file = fopen(this->req.config->getValue("error_404")->data(), "r"); 
		this->status = 404;
	}
	this->fileFd = fileno(this->file);
	this->fileSize = ft::filesystem::getFileSize(this->file);

	this->writeHeader();
	this->fields["Content-Length"] = std::to_string(this->fileSize);
	this->writeFields();
	this->writeEnd();

	std::cout << "//-----rep header----//\n" << this->data;
}

void ft::Response::parsePost(void)
{

}

void ft::Response::parseDelete(void)
{
	
}

////

// Writes the header object
void ft::Response::writeHeader(void)
{
	// HTTP/1.1 503 Service Unavailabe\n
	this->data += ft::format("HTTP/1.1 %u %s\n", this->status, ft::getStatusCodes().at(this->status).c_str());
	this->fields["Server"] = "Breadserv";
}

void ft::Response::writeFields(void)
{
	for (const auto [key, value] : this->fields)
		this->data += key + " : " + value + '\n';
}

void ft::Response::writeEnd(void)
{
	this->data += "\n";
}

//https://www.youtube.com/watch?v=tYzMYcUty6s fucking mime types
ft::ResponseStatus ft::Response::send(int32_t socket)
{
	if (!this->sentHeader)
	{
		ft::send(socket, this->data.data(), this->data.length(), 0);	
		this->sentHeader = true;
		if (this->fileFd < 0)
			return ft::DONE;
		return ft::NOT_DONE;
	}
	off_t len;
	sendfile(this->fileFd, socket, this->fileOffset, &len, NULL, 0);
	this->fileOffset += len;

	if (this->fileOffset >= this->fileSize)
		return ft::ResponseStatus::DONE;
	return ft::ResponseStatus::NOT_DONE;
}

ft::Response ft::Response::getError(uint32_t code)
{
	ft::Response outResponse(code);
	outResponse.parseError(code);
	return (outResponse);
}