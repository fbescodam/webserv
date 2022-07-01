/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Response.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 19:34:00 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/01 15:39:39 by pvan-dij      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

ft::Response::~Response()
{
	if (this->file)
		fclose(this->file);
	if (this->fileFd > -1)
		close(this->fileFd);
}

ft::Response::Response(ft::Request reqIn, ft::ServerSection *configIn)
{
	// TODO: This should properly construct a response based on the earlier received request
	// data = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
	// reqIn.display();

	this->req = reqIn;
	this->config = ft::Section(ft::basedir(reqIn.path), "response", *configIn);
	this->rootPath = *configIn->getValue("path");
	this->locations = configIn->locations;
	this->sentHeader = false;
	this->fileOffset = 0;
	this->file = 0;
	this->fileFd = -1;



	//TODO: date in response?
	if (this->req.keyExists("Connection") && *this->req.getValue("Connection") == "keep-alive")
	{
		this->fields["Connection"] = "keep-alive";
		this->fields["Keep-Alive"] = "timeout=" + std::to_string(S_TIMEOUT);
	}
	else
		this->fields["Connection"] = "close";

	if (this->applyConfig())
		return ;

	switch(this->req.method)
	{
		case (ft::Method::GET):
			this->parseGet(); break;
		case (ft::Method::POST):
			this->parsePost(); break;
		case (ft::Method::DELETE):
			this->parseDelete(); break;
		default:
			this->generateStatusPage(405); //this will never happen anymore, 405 is checked earlier in applyconfig
	}
}

//////////////////////////////////////////

bool ft::Response::checkMethods(std::list<std::string> methodList)
{
	bool methodAllowed = false;
	for (const std::string &val: methodList)
	{
		if (val == ft::enumStrings[static_cast<int>(this->req.method)])
			methodAllowed = true;
	}
	if (methodAllowed == false)
	{
		if (!this->getCustomStatusPage(405))
			this->writeFileFields();
		return (true);
	}
	return (false);
}

bool ft::Response::applyConfig()
{
	if (this->req.path.back() == '/')
	{
		if (!this->config.keyExists("index"))
			this->req.path += "index.html"; // TODO: should only do this if dir_listing is disabled, otherwise generate a list of files and directories in path
		else
			this->req.path += *this->config.getValue("index");
	}

	for (const auto &val: this->locations)
	{
		if (val.appliesForPath(this->req.path))
			this->config.importFields(val.exportFields());
	}
	this->config.print("fijwf   ");

	// check if method is accepted for request
	std::list<std::string> methodList;
	if (this->config.getValueAsList("methods", methodList))
	{
		if (this->checkMethods(methodList))
			return (true);
	}
	else
	{
		if (this->checkMethods({"GET"}))
			return (true);
	}

	// check if access is allowed
	if (this->config.keyExists("access") && *this->config.getValue("access") == "no")
	{
		if (!this->getCustomStatusPage(403))
			this->writeFileFields();
		return (true);
	}

	// check if redirection is set
	std::list<std::string> redirInfo;
	if (this->config.getValueAsList("redir", redirInfo))
	{
		int code = std::atoi(redirInfo.front().c_str());
		this->fields["Location"] = redirInfo.back();
		this->generateStatusPage(code);
		return (true);
	}
	
	// all good, continue parsing the request (return false)
	return (false);

}

void ft::Response::generateStatusPage(int code)
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

void ft::Response::generateStatusPage(int code, std::string content)
{
	this->status = code;
	const std::string& statusText = ft::getStatusCodes().at(code);

	// Build header and fields
	this->writeHeader();
	this->fields["Content-Length"] = std::to_string(content.length());
	this->fields["Content-Type"] = "text/html";
	this->writeFields();
	this->writeEnd();

	// Build content
	this->data += content;
}

//returns true on status page, dont write fields after
bool ft::Response::getCustomStatusPage(int code)
{
	std::string errorPage = "error_" + std::to_string(code);

	if (!this->config.keyExists(errorPage))
		return (this->generateStatusPage(code), true); // no custom error page found, generate one on the fly
	this->filePath = this->rootPath + *this->config.getValue(errorPage);
	if (!ft::filesystem::fileExists(this->filePath))
		return (this->generateStatusPage(code), true); // custom error page not found (HA!), generate one on the fly
	this->file = fopen(this->filePath.data(), "r");
	this->status = code;
	return (false);
}

void ft::Response::writeFileFields(void)
{
	this->fileFd = fileno(this->file);
	this->fileSize = ft::filesystem::getFileSize(this->file);

	this->writeHeader();
	this->fields["Content-Length"] = std::to_string(this->fileSize);
	this->fields["Content-Type"] = ft::getContentType(this->filePath);
	this->writeFields();
	this->writeEnd();
}

void ft::Response::parseGet(void)
{
	this->filePath = *this->config.getValue("path") + this->req.path;
	// std::cout << requestedFile << std::endl;
	if (ft::filesystem::fileExists(this->filePath))
	{
		this->file = fopen(this->filePath.data(), "r");
		this->status = 200;
	}
	else
	{
		if (this->getCustomStatusPage(404))
			return ;
	}
	this->writeFileFields();
}

void ft::Response::parsePost(void)
{

}

//curl -X DELETE http://localhost:8080/delete/deletefile
void ft::Response::parseDelete(void)
{
	this->filePath = *this->config.getValue("path") + this->req.path;

	if (ft::filesystem::fileExists(this->filePath))
	{
		std::remove(this->filePath.c_str());
		this->generateStatusPage(200, "<!DOCTYPE html><html><body><h1>File deleted.</h1></body></html>");
	}
	else
		this->generateStatusPage(204);
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
		std::cout << "//-----rep header----//\n" << this->data;
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
	outResponse.generateStatusPage(code);
	return (outResponse);
}