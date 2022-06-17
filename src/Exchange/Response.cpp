/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Response.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 19:34:00 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/17 03:15:50 by pvan-dij      ########   odam.nl         */
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
}

ft::Response::Response(Request reqIn, ft::ServerSection *configIn)
{
	// TODO: This should properly construct a response based on the earlier received request
	// data = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
	reqIn.display();

	this->req = reqIn;
	this->config = configIn;
	this->sentHeader = false;
	this->fileOffset = 0;

	if (this->req.path == "/")
		this->req.path = "/index.html";

	if (ft::fileExists(this->config->root + this->req.path))
	{
		this->file = fopen((this->config->root + this->req.path).data(), "r"); 
		this->status = 200;
	}
	else
	{
		this->file = fopen("./examples/www/custom404.html", "r"); 
		this->status = 404;
	}
	this->fileFd = fileno(this->file);
	this->fileSize = ft::get_file_size(this->file);

	this->writeHeader();
	this->fields["Content-Length"] = std::to_string(this->fileSize);
	this->fields["Connection"] = "keep-alive";
	this->fields["Keep-Alive"] = "timeout=" + std::to_string(S_TIMEOUT);
	this->writeFields();
	this->writeEnd();

	std::cout << "//---------//\n" << this->data;
	

}

//////////////////////////////////////////

/** As one giant shit strings
 * CODE - STATUS
 * <HEADER>
 * \n\n
 * <BODY>
 *
 * HTTP/1.1 503 Service Unavailabe\n
 * Content-Type: text/plain\n
 * Content-Length: 12\n
 * \n
 * 503 error
 */

// Writes the header object
void ft::Response::writeHeader(void)
{
	// HTTP/1.1 503 Service Unavailabe\n
	data += ft::format("HTTP/1.1 %u %s\n", status, ft::getStatusCodes().at(status).c_str());
	fields["Server"] = "Breadserv";
}

void ft::Response::writeFields(void)
{
	for (const auto [key, value] : this->fields)
		data += key + " : " + value + '\n';
}

void ft::Response::writeEnd(void)
{
	data += "\n";
}

//https://www.youtube.com/watch?v=tYzMYcUty6s fucking mime types
ft::ResponseStatus ft::Response::send(int32_t socket)
{
	if (!this->sentHeader)
	{
		ft::send(socket, this->data.data(), this->data.length(), 0);	
		this->sentHeader = true;
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
	const std::string& content = ft::getStatusCodes().at(code);

	// Build header and fields
	outResponse.writeHeader();
	outResponse.fields["Content-Length"] = std::to_string(content.length());
	outResponse.fields["Content-Type"] = "text/plain";
	outResponse.fields["Connection"] = "close";
	outResponse.writeFields();
	outResponse.writeEnd();

	// Build content
	outResponse.data += content;
	return (outResponse);
}