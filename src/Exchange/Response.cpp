/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Response.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/27 11:07:35 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/08/11 17:02:40 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

ft::Response::Response(const ft::Connection& conn) : conn(conn), config(this->conn.server->config)
{
	std::cout << BLACK << "Set sendRes to nullptr (in constructor)" << RESET << std::endl;
	this->sendRes = nullptr;
	this->file = nullptr;
	this->offset = 0;
	this->headers["server"] = "Breadserv";
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

	std::cout << BLACK << "rootPath: " << rootPath << RESET << std::endl;
	for (const auto &location: this->config.locations)
	{
		if (location.appliesForPath(rootPath))
		{
			std::cout << BLACK << "Location " << location.getAppliedPath() << " applies for path " << rootPath << "!" << RESET << std::endl;
			this->pathConfig.importFields(location.exportFields());
			overridePath = const_cast<std::string*>(location.getValue("path"));
			if (overridePath)
			{
				std::cout << BLACK << "rootPath before override: " << rootPath << RESET << std::endl;
				rootPath.erase(0, location.getAppliedPath().length());
				std::cout << BLACK << "rootPath after erasure: " << rootPath << RESET << std::endl;
				if (location.getAppliedPath().back() != '/' && rootPath.front() != '/')
					rootPath.erase(0, rootPath.find_first_of('/') + 1);
				std::cout << BLACK << "rootPath after erasure of starting /: " << rootPath << RESET << std::endl;
				rootPath = *overridePath + rootPath;
				std::cout << BLACK << "rootPath after override: " << rootPath << RESET << std::endl;
			}
		}
	}
	std::cout << BLACK << "rootPath after import: " << rootPath << RESET << std::endl;
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
	if (status == 100)
		conn.response->headers["connection"] = "keep-alive";
	conn.response->generateStatus(status);
}

//////////////////////////////////////////

void ft::Response::generateStatus(int32_t status)
{
	// Find custom page
	std::string errorPage = "error_" + std::to_string(status);
	if (this->config.keyExists(errorPage))
	{
		std::cout << BLACK << "A custom error page exists for status " << status << RESET << std::endl;

		const std::string* path = this->config.getValue("path");
		const std::string* page = this->config.getValue(errorPage);
		if (!path || !page)
			goto generic;
		std::string filePath(*path + *page);
		if ((this->file = fopen(filePath.c_str(), "r")) == nullptr)
			goto generic;

		this->writeStatusLine(status);
		this->headers["content-length"] = std::to_string(ft::filesystem::getFileSize(this->file));
		this->headers["content-type"] = ft::getContentType(filePath);
		this->writeHeaders();
		this->fileSize = ft::filesystem::getFileSize(this->file);
		std::cout << BLACK << "Set sendRes to sendHeaders" << RESET << std::endl;
		this->sendRes = &ft::Response::sendHeaders;
		return;
	}

generic: // Send generic built-in page.
	std::cout << BLACK << "Generating status page for " << status << RESET << std::endl;
	std::string statusText(ft::getStatusCodes().at(status)); // TODO: Retrieve pig image.
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
	std::cout << BLACK << "Set sendRes to sendDynamic" << RESET << std::endl;
	this->sendRes = &ft::Response::sendDynamic;
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

// void ft::Response::fileUpload(std::string data)
// {
// 	std::cout << RED<<data<<std::endl;
// 	exit(0);
// }

void ft::Response::postMethod(const std::string& filePath)
{
	std::list<std::string> cgiBin;

	if (!ft::filesystem::fileExists(filePath))
		this->generateStatus(404);
	else if (this->config.getValueAsList("cgi_bin", cgiBin) && endsWith(filePath, cgiBin.front()))
	{
		std::string out;
		if (!ft::CGI::runCGI(this->conn, filePath, out, cgiBin.back()))
			return (this->generateStatus(500));

		this->data = out;
		std::cout << BLACK << "Set sendRes to sendDynamic" << RESET << std::endl;
		this->sendRes = &ft::Response::sendDynamic;
		return;
	}
	// else
	// {
	// 	this->fileUpload(conn.request->header_data + conn.request->data);
	// }

	// Nothing to post so we treat it as a get method.
	this->getMethod(filePath);
}

//////////////////////////////////////////

// Simply open the file and send it over.
void ft::Response::getMethod(const std::string& filePath)
{
	std::cout << BLACK << "Receiving GET method. Responding now." << RESET << std::endl;

	// Check if filepath ends with /, if so, dir listing.
	if (filePath.back() == '/')
	{
		try
		{
			std::string dirListing;
			ft::DirectoryFactory::buildContentFromDir(filePath, this->conn.request->path, dirListing);

			this->writeStatusLine(200);
			this->headers["content-length"] = std::to_string(dirListing.size());
			this->headers["content-type"] = "text/html";
			this->writeHeaders();
			this->data += dirListing;
			std::cout << BLACK << "Set sendRes to sendDynamic" << RESET << std::endl;
			this->sendRes = &ft::Response::sendDynamic;
			return;
		}
		catch(const std::exception& e)
		{
			std::cerr << RED << "Webserv: " << e.what() << RESET << std::endl;

			try { return (this->generateStatus(500)); }
			catch (const std::exception& e) { exit (EXIT_FAILURE); }
		}
	}

	try // No, its just a file
	{
		if (!ft::filesystem::fileExists(filePath))
			return (this->generateStatus(404));

		if ((this->file = fopen(filePath.c_str(), "r")) == nullptr)
			return (this->generateStatus(500));

		this->writeStatusLine(200);
		this->fileSize = ft::filesystem::getFileSize(this->file);
		this->headers["content-length"] = std::to_string(this->fileSize);
		this->headers["content-type"] = ft::getContentType(filePath);
		this->writeHeaders();
		std::cout << BLACK << "Set sendRes to sendHeaders" << RESET << std::endl;
		this->sendRes = &ft::Response::sendHeaders;
	}
	catch(const std::exception& e)
	{
		std::cerr << RED << "Webserv: " << e.what() << RESET << std::endl;

		// We should NEVER have to reach this point ...
		try { return (this->generateStatus(500)); }
		catch (const std::exception& e) { exit (EXIT_FAILURE); }
	}
}

//////////////////////////////////////////

ft::Response::Status ft::Response::sendDynamic(ft::fd_t socket)
{
	std::cout << BLACK << "Sending everything in one go (dynamically generated page)..." << RESET << std::endl;
	size_t bsent = ft::send(socket, this->data.data(), this->data.length(), NONE); // Send as much as possible
	std::cout <<RED<<"argh: "<<bsent<<RESET<<std::endl;
	this->data.erase(0, bsent); // Delete data that has been sent
	if (bsent < this->data.length()) // Not everything was sent, send more in the next poll
		return (ft::Response::Status::NOT_DONE);
	std::cout << BLACK << "Set sendRes to nullptr" << RESET << std::endl;
	this->sendRes = nullptr; // Everything was sent, nothing more to do
	return (ft::Response::Status::DONE);
}

//////////////////////////////////////////

ft::Response::Status ft::Response::sendHeaders(ft::fd_t socket)
{
	std::cout << BLACK << "Sending headers..." << RESET << std::endl;
	size_t bsent = ft::send(socket, this->data.data(), this->data.length(), NONE);
	this->data.erase(0, bsent); // Delete data that has been sent
	if (bsent < this->data.length()) // Not everything was sent, send more in the next poll
		return (ft::Response::Status::NOT_DONE);
	std::cout << BLACK << "Set sendRes to sendFile" << RESET << std::endl;
	this->sendRes = &ft::Response::sendFile; // Everything was sent, now continue with sending the file
	return (ft::Response::Status::NOT_DONE); // Actually not done yet! We now need to send the file
}

//////////////////////////////////////////

ft::Response::Status ft::Response::sendFile(ft::fd_t socket)
{
	off_t bsent = 0;

	if (this->file == nullptr)
		throw std::exception();

	std::cout << BLACK << "Sending file..." << RESET << std::endl;
	sendfile(fileno(this->file), socket, offset, &bsent, NULL, NONE);
	this->offset += bsent;

	if (this->offset < this->fileSize)
		return (ft::Response::Status::NOT_DONE);

	// Everything was sent, nothing more to do
	std::cout << BLACK << "Set sendRes to nullptr" << RESET << std::endl;
	this->sendRes = nullptr;
	return (ft::Response::Status::DONE);
}

//////////////////////////////////////////
