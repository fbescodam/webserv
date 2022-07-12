/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Response.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 19:34:00 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/12 21:12:19 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"
#include "Response.hpp"
#include <sys/stat.h>

ft::Response::Response(int32_t statusIn, ft::ServerSection *configIn)
{
	this->config.importFields(configIn->exportFields());
	this->generateStatusPage(statusIn);
}


ft::Response::Response(ft::Request requestIn, ft::ServerSection* configIn)
{
	this->request = requestIn;
	this->config = ft::Section(ft::basedir(requestIn.path), "response", *configIn);
	this->locations = configIn->locations;
	this->file = NULL;
	this->fileFd = -1;
	this->sentHeader = false;
	this->fileOffset = 0;
}

ft::Response::~Response()
{
	
}

//////////////////////////////////////////

static bool checkMethods(const std::list<std::string>& methods, ft::Method requestMethod)
{
	std::string reqMethodString = ft::enumStrings[static_cast<int32_t>(requestMethod)];
	
	for (const std::string &val: methods)
		if (val == reqMethodString)
			return (true);
	return (false);
}

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <signal.h>

bool ft::Response::verify(void)
{

	// Loops over all locations and takes the rules from them and applies it to this config
	for (const auto &val: this->locations)
		if (val.appliesForPath(this->request.path))
			this->config.importFields(val.exportFields());

	struct stat stats;
	stat((*this->config.getValue("path") + this->request.path).c_str(), &stats);
	if (S_ISDIR(stats.st_mode) && this->request.path.back() != '/')
	{
		this->fields["Location"] = this->request.path + "/";
		this->generateStatusPage(308);
		return (false);
	}

	std::cout <<this->request.path<<std::endl;
	if (this->request.path.back() == '/')
	{
		if (!ft::filesystem::fileExists(this->request.path + *this->config.getValue("index")))
		{
			const std::string* dir = this->config.getValue("dir_listing");

			if (dir != nullptr && *dir == "no")
				this->request.path += "index.html";
		}
	}


	// Gets the allowed methods for path
	std::list<std::string> methodList;
	static std::list<std::string> methodGet = {{"GET"}};
	bool methodRules = this->config.getValueAsList("methods", methodList);
	
	// Checks if request method is in allowed methods
	if (!checkMethods(methodRules ? methodList : methodGet, this->request.method))
	{
		this->generateStatusPage(405); 
		return (false);
	}

	// Checks if access is allowed to request path
	const std::string* access = this->config.getValue("access");
	if (access != nullptr && *access == "no")
	{
		this->generateStatusPage(403); 
		return (false);
	}

	// Check for redirect
	std::list<std::string> redirInfo;
	if (this->config.getValueAsList("redir", redirInfo))
	{
		this->fields["Location"] = redirInfo.back();
		this->generateStatusPage(std::stoi(redirInfo.front()));
		return (false);
	}

	//no issues found
	return (true);
}

void ft::Response::generateStatusPage(int32_t code)
{	
	//try to find custom error page, if it doesnt exit make one on the fly
	std::string errorPage = "error_" + std::to_string(code);
	if (this->config.keyExists(errorPage))
	{
		std::string filePath(*this->config.getValue("path") + *this->config.getValue(errorPage));

		// Check if we can open the file, use FILE* because of content length.
		if ((this->file = fopen(filePath.data(), "r")))
		{
			this->fileFd = fileno(this->file);
			this->fileSize = ft::filesystem::getFileSize(this->file);
			this->writeHeader(code);
			this->fields["Content-Length"] = std::to_string(this->fileSize);
			this->fields["Content-Type"] = ft::getContentType(filePath);
			this->fields["Connection"] = "close"; //TODO: connection: keep-alive check somewhere
			this->writeFields();
			return;
		}
	}

	const std::string& statusText = ft::getStatusCodes().at(code);
	DIR *dir = opendir("./examples/www/imgs/pig1_files");
	srand(time(0));
	int rand = std::rand() % 100;
	dirent* ent;
	for (int i = rand; i > 0; i--)
		ent = readdir(dir);
	std::string name = "/imgs/pig1_files/" + std::string(ent->d_name);
	closedir(dir);
	ft::Response::generateStatusPage(code, "<!DOCTYPE html><html><head><title>"+statusText+"</title></head><body><h1>"+std::to_string(code)+" "+statusText+"</h1><img src=\""+name+"\"></body></html>");
}

void ft::Response::generateStatusPage(int32_t code, std::string content)
{
	// Build header and fields
	this->writeHeader(code);
	this->fields["Content-Length"] = std::to_string(content.length());
	this->fields["Content-Type"] = "text/html";
	this->writeFields();
	
	// Build content
	this->data += content;
}

//curl -X DELETE http://localhost:8080/delete/deletefile
void ft::Response::deleteMethod(std::string filePath)
{
	if (ft::filesystem::fileExists(filePath))
	{
		std::remove(filePath.c_str());
		this->generateStatusPage(200, "<!DOCTYPE html><html><body><h1>File deleted.</h1></body></html>");
	}
	else
		this->generateStatusPage(404);
}

//after verify, make up the response
void ft::Response::generateResponse()
{
	std::string filePath = *this->config.getValue("path") + this->request.path;

	if (this->request.method == ft::Method::DELETE)
	{
		this->deleteMethod(filePath); 
		return;
	}

	//TODO: check if filepath is a cgi and handle a post accordingly

	//check if filepath ends with /, if so, dir listing
	if (filePath.back() == '/')
	{
		std::string dirListing;
		ft::DirectoryFactory::buildContentFromDir(filePath, dirListing);
		this->writeHeader(200);
		this->fields["Content-Length"] = std::to_string(dirListing.size());
		this->fields["Content-Type"] = "text/html";
		this->fields["Connection"] = "close"; //TODO: connection: keep-alive check somewhere
		this->writeFields();
		this->data += dirListing;
		return;
	}

	//if file exists write appropriate fields
	if (ft::filesystem::fileExists(filePath))
	{
		this->file = fopen(filePath.data(), "r");
		this->fileFd = fileno(this->file);
		this->fileSize = ft::filesystem::getFileSize(this->file);
		this->writeHeader(200);
		this->fields["Content-Length"] = std::to_string(this->fileSize);
		this->fields["Content-Type"] = ft::getContentType(filePath);
		this->fields["Connection"] = "close"; //TODO: connection: keep-alive check somewhere
		this->writeFields();
		return;
	}
	//file doesnt exist, make errorpage
	this->generateStatusPage(404);
}

// Writes the header object
void ft::Response::writeHeader(int32_t status)
{
	// HTTP/1.1 503 Service Unavailabe\n
	this->data += ft::format("HTTP/1.1 %u %s\n", status, ft::getStatusCodes().at(status).c_str());
	this->fields["Server"] = "Breadserv";
}

// Writes fields to data
void ft::Response::writeFields(void)
{
	for (const auto [key, value] : this->fields)
		this->data += key + " : " + value + '\n';
	this->data += "\n";
}

///

//TODO: does this need to be cleaner?
ft::ResponseStatus ft::Response::send(int32_t socket)
{
	//send the header part
	if (!this->sentHeader)
	{
		ft::send(socket, this->data.data(), this->data.length(), 0);
		this->sentHeader = true;
		if (this->fileFd < 0)
			return ft::DONE;
		return ft::NOT_DONE;
	}

	//send the file, if its done resolve the connectino
	off_t len;
	sendfile(this->fileFd, socket, this->fileOffset, &len, NULL, 0);
	this->fileOffset += len;

	if (this->fileOffset >= this->fileSize)
		return ft::ResponseStatus::DONE;
	return ft::ResponseStatus::NOT_DONE;
}

