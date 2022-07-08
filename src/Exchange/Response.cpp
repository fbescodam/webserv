/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Response.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 19:34:00 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/08 17:54:12 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"
#include "Response.hpp"

ft::Response::Response(int32_t statusIn, ft::ServerSection *configIn)
{
	this->config.importFields(configIn->exportFields())
	this->generateStatusPage(statusIn);
}


ft::Response::Response(ft::Request requestIn, ft::ServerSection* configIn)
{
	this->request = requestIn;
	this->config = ft::Section(ft::basedir(requestIn.path), "response", *configIn);
	this->locations = configIn->locations;
	this->file = NULL;
	this->fileFd = -1;
}

//////////////////////////////////////////

static bool checkMethods(const std::list<std::string>& methods, ft::Method requestMethod)
{
	for (const std::string &val: methods)
		if (val == ft::enumStrings[static_cast<int32_t>(requestMethod)])
			return (true);
	return (false);
}

void ft::Response::verify(void)
{
	if (this->request.path.back() == '/')
	{
		if (this->config.keyExists("index"))
			this->request.path += *this->config.getValue("index");
		else
		{
			const std::string* dir = this->config.getValue("dir_listing");

			if (dir != nullptr && *dir == "no")
				this->request.path += "index.html";
		}
	}

	// Loops over all locations and takes the rules from them and applies it to this config
	for (const auto &val: this->locations)
		if (val.appliesForPath(this->request.path))
			this->config.importFields(val.exportFields());

	// Gets the allowed methods for path
	std::list<std::string> methodList;
	static std::list<std::string> methodGet = {{"GET"}};
	bool methodRules = this->config.getValueAsList("methods", methodList);
	
	// Checks if request method is in allowed methods
	if (checkMethods(methodRules ? methodList : methodGet, this->request.method))
		this->generateStatusPage(405); return;

	// Checks if access is allowed to request path
	const std::string* access = this->config.getValue("access");
	if (access != nullptr && *access == "no")
		this->generateStatusPage(403); return;

	// Check for redirect
	std::list<std::string> redirInfo;
	if (this->config.getValueAsList("redir", redirInfo))
	{
		this->fields["Location"] = redirInfo.back();
		this->generateStatusPage(std::stoi(redirInfo.front()));
		return;
	}

	//no issues found
	//TODO: actual response, and directory listing if path ends with "/"
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
			return;
		}
	}

	const std::string& statusText = ft::getStatusCodes().at(code);
	ft::Response::generateStatusPage(code, "<!DOCTYPE html><html><head><title>"+statusText+"</title></head><body><h1>"+std::to_string(code)+" "+statusText+"</h1></body></html>");
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

void ft::Response::parseGet(void)
{
	
}

//curl -X DELETE http://localhost:8080/delete/deletefile
void ft::Response::parseDelete(void)
{
	
}

////

// Writes the header object
void ft::Response::writeHeader(int32_t status)
{
	// HTTP/1.1 503 Service Unavailabe\n
	this->data += ft::format("HTTP/1.1 %u %s\n", status, ft::getStatusCodes().at(status).c_str());
	this->fields["Server"] = "Breadserv";
}

void ft::Response::writeFields(void)
{
	for (const auto [key, value] : this->fields)
		this->data += key + " : " + value + '\n';
	this->data += "\n";
}

///

//https://www.youtube.com/watch?v=tYzMYcUty6s fucking mime types
ft::ResponseStatus ft::Response::send(int32_t socket)
{

}

