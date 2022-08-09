/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Response.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/27 11:07:35 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/08/09 20:50:36 by W2Wizard      ########   odam.nl         */
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

static std::string basedir(const std::string& path)
{
	std::string base = path;
	base.erase(path.find_last_of('/'));
	return (base);
}

void ft::Response::importFieldsForPath()
{
	this->pathConfig = ft::Section(this->config.getcwd(), "response", this->config);
	std::string basePath = basedir(this->conn.request->path);

	for (const auto &val: this->config.locations)
		if (val.appliesForPath(basePath))
			this->pathConfig.importFields(val.exportFields());	
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
	conn.response->generateStatus(status);
}

//////////////////////////////////////////

void ft::Response::generateStatus(int32_t status)
{
	// Find custom page
	std::string errorPage = "error_" + std::to_string(status);
	if (this->config.keyExists(errorPage))
	{
		std::cout << RED << "here0"<<std::endl;

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
			ft::DirectoryFactory::buildContentFromDir(filePath, dirListing);

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
