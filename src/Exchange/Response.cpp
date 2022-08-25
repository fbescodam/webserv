/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Response.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/27 11:07:35 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/08/25 17:26:52 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

ft::Response::Response(const ft::Connection& conn) : conn(conn), config(this->conn.server->config)
{
	LOG("Set sendRes to nullptr (in constructor)");
	this->sendRes = nullptr;
	this->file = nullptr;
	this->offset = 0;
	this->headers["server"] = "Breadserv";
	this->isDirListing = false;
	if (conn.request->headers["connection"] == "keep-alive")
	{
		this->headers["connection"] = "keep-alive";
		this->headers["keep-alive"] = "timeout=" + std::to_string(CONN_TIMEOUT);
	}
}

//////////////////////////////////////////

ft::Response::~Response()
{
	fclose(this->file);
}

//////////////////////////////////////////

void ft::Response::importFieldsForPath(std::string& rootPath)
{
	this->pathConfig = ft::Section(this->config.getcwd(), "response", this->config);
	std::string* overridePath = nullptr;
	LOG("rootPath: " << rootPath);
	for (const ft::Section &location: this->config.locations)
	{
		if (location.appliesForPath(rootPath))
		{
			LOG("Location " << location.getAppliedPath() << " applies for path " << rootPath << "!");
			this->pathConfig.importFields(location.exportFields());
			overridePath = const_cast<std::string*>(location.getValue("path"));
			if (overridePath)
			{
				LOG("rootPath before override: " << rootPath);
				rootPath.erase(0, location.getAppliedPath().length());
				LOG("rootPath after erasure: " << rootPath);
				if (location.getAppliedPath().back() != '/' && rootPath.front() != '/')
					rootPath.erase(0, rootPath.find_first_of('/') + 1);
				LOG("rootPath after erasure of starting /: " << rootPath);

				rootPath = *overridePath + rootPath;
				LOG("rootPath after override: " << rootPath);
			}
		}
	}
	LOG("rootPath after import: " << rootPath);
}

//////////////////////////////////////////


void ft::Response::writeStatusLine(int32_t status)
{
	this->data += "HTTP/1.1 " + std::to_string(status) + " " + ft::getStatusCodes().at(status) + "\r\n";
}

//////////////////////////////////////////

void ft::Response::writeHeaders(void)
{
	for (const auto& [key, value] : this->headers)
		this->data += key + ": " + value + "\r\n";
	this->data += "\r\n";
}

//////////////////////////////////////////

void ft::Response::generateResponse(ft::Connection& conn, int32_t status)
{
	conn.response = new ft::Response(conn);
	conn.response->headers["connection"] = status == 100 ? "keep-alive" : "close";
	conn.response->generateStatus(status);
}

//////////////////////////////////////////

void ft::Response::generateStatus(int32_t status)
{
	// Find custom page
	std::string errorPage = "error_" + std::to_string(status);
	if (this->config.keyExists(errorPage))
	{

		LOG("A custom error page exists for status " << status);

		const std::string* path = this->config.getValue("path");
		const std::string* page = this->config.getValue(errorPage);
		if (!path || !page)
			goto generic;
		std::string filePath(this->config.getcwd() + *path + *page);
		if ((this->file = fopen(filePath.c_str(), "r")) == nullptr)
			goto generic;

		this->writeStatusLine(status);
		this->headers["content-length"] = std::to_string(ft::filesystem::getFileSize(this->file));
		this->headers["content-type"] = ft::getContentType(filePath);
		this->writeHeaders();
		this->fileSize = ft::filesystem::getFileSize(this->file);
		LOG("Set sendRes to sendHeaders");
		this->sendRes = &ft::Response::sendHeaders;
		return;
	}

generic: // Send generic built-in page.
	LOG("Generating status page for " << status);
	std::string statusText(ft::getStatusCodes().at(status));
	this->generateStatus(status, "<!DOCTYPE html><html><head><title>" + statusText + "</title></head><body><h1>" + std::to_string(status) + " " + statusText + "</h1></body></html>");
}

//////////////////////////////////////////

void ft::Response::generateStatus(int32_t status, const std::string& content)
{
	// Build header and fields
	this->writeStatusLine(status);
	this->headers["content-length"] = std::to_string(content.length());
	this->headers["content-type"] = "text/html";
	this->writeHeaders();

	// Build content
	this->data += content;
	LOG("Set sendRes to sendDynamic");
	this->sendRes = &ft::Response::sendDynamic;
}

//////////////////////////////////////////

void ft::Response::deleteMethod(const std::string& filePath)
{
	if (!ft::filesystem::fileExists(filePath))
		return (this->generateStatus(404));

	if (std::remove(filePath.c_str()) != 0)
		return (this->generateStatus(500, "<h1>Failed to delete file</h1>"));
	this->generateStatus(200, "<h1>File deleted</h1>");
}

//////////////////////////////////////////


void ft::Response::respond(const std::string& filePath)
{
	std::list<std::string> cgiBin;

	if (!ft::filesystem::fileExists(filePath))
		this->generateStatus(404);
	if (this->config.getValueAsList("cgi_bin", cgiBin) && endsWith(filePath, cgiBin.front()))
	{
		std::string out;
		if (!ft::CGI::runCGI(this->conn, filePath, out, cgiBin.back()))
		{
			ERR("CGI execution failed! Responding with internal server error (500).");
			return (this->generateStatus(500));
		}

		this->data = out;
		LOG("Set sendRes to sendDynamic");
		this->sendRes = &ft::Response::sendDynamic;
		return;
	}

	this->notCGI(filePath);
}

//////////////////////////////////////////

// Simply open the file and send it over.
void ft::Response::notCGI(const std::string& filePath)
{
	LOG("Receiving GET method. Responding now.");

	if (this->isDirListing) try
	{
		std::string dirListing;
		ft::DirectoryFactory::buildContentFromDir(filePath, this->conn.request->path, dirListing);

		this->writeStatusLine(200);
		this->headers["content-length"] = std::to_string(dirListing.size());
		this->headers["content-type"] = "text/html";
		this->writeHeaders();
		this->data += dirListing;
		LOG("Set sendRes to sendDynamic");
		this->sendRes = &ft::Response::sendDynamic;
		return;
	}
	catch(const std::exception& e)
	{
		LOG("Exception occurred in dirlisting: " << e.what());
		try { return (this->generateStatus(500)); }
		catch (const std::exception& e) { exit(EXIT_FAILURE); }
	}
	else try
	{
		if (!ft::filesystem::fileExists(filePath))
			return (this->generateStatus(404));

		if ((this->file = fopen(filePath.c_str(), "r")) == nullptr)
		{
			ERR("Unable to run fopen on file " << filePath);
			return (this->generateStatus(500));
		}

		this->writeStatusLine(200);
		this->fileSize = ft::filesystem::getFileSize(this->file);
		this->headers["content-length"] = std::to_string(this->fileSize);
		this->headers["content-type"] = ft::getContentType(filePath);
		this->writeHeaders();
		LOG("Set sendRes to sendHeaders");
		this->sendRes = &ft::Response::sendHeaders;
	}
	catch(const std::exception& e)
	{
		ERR("Webserv: " << e.what());
		// We should NEVER have to reach this point ...
		try { return (this->generateStatus(500)); }
		catch (const std::exception& e) { exit (EXIT_FAILURE); }
	}
}

//////////////////////////////////////////

ft::Response::Status ft::Response::sendDynamic(ft::fd_t socket)
{
	LOG("Sending everything in one go (dynamically generated page)...");
	size_t bsent = send(socket, this->data.data(), this->data.length(), NONE); // Send as much as possible
	LOG("Bytes sent: " << bsent);
	this->data.erase(0, bsent); // Delete data that has been sent
	if (bsent < this->data.length()) // Not everything was sent, send more in the next poll
		return (ft::Response::Status::NOT_DONE);
	LOG("Set sendRes to nullptr");
	this->sendRes = nullptr; // Everything was sent, nothing more to do
	return (ft::Response::Status::DONE);
}

//////////////////////////////////////////

ft::Response::Status ft::Response::sendHeaders(ft::fd_t socket)
{
	LOG("Sending headers...");
	size_t bsent = send(socket, this->data.data(), this->data.length(), NONE);
	this->data.erase(0, bsent); // Delete data that has been sent
	if (bsent < this->data.length()) // Not everything was sent, send more in the next poll
		return (ft::Response::Status::NOT_DONE);
	LOG("Set sendRes to sendFile");
	this->sendRes = &ft::Response::sendFile; // Everything was sent, now continue with sending the file
	return (ft::Response::Status::NOT_DONE); // Actually not done yet! We now need to send the file
}

//////////////////////////////////////////

ft::Response::Status ft::Response::sendFile(ft::fd_t socket)
{
	off_t bsent = 0;

	if (this->file == nullptr)
		throw std::exception();

	LOG("Sending file...");
	sendfile(fileno(this->file), socket, offset, &bsent, NULL, NONE);
	this->offset += bsent;

	if (this->offset < this->fileSize)
		return (ft::Response::Status::NOT_DONE);

	// Everything was sent, nothing more to do
	LOG("Set sendRes to nullptr");
	this->sendRes = nullptr;
	return (ft::Response::Status::DONE);
}

//////////////////////////////////////////
