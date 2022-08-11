/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Request.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/27 11:07:39 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/08/11 19:30:54 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

ft::Request::Request(const std::vector<ft::Server>& servers) noexcept : servers(servers)
{
	this->data = "";
	this->headerDone = this->headerParsed = false;
	this->contentLength = 0;
}

//////////////////////////////////////////

// Appends up until it managed to append enough data to build the header.
void ft::Request::appendBuffer(char *buffer, int32_t bread)
{
	static int32_t maxBodySize = this->servers[0].config.returnValueAsInt("limit_body_size");
	// Check if request is malformed.
	//else if (!isalpha(buffer.at(0)))
	//	throw ft::BadRequest(); // TODO: move to header parser, this is for invalid requests (HTTPS, for example)

	if (this->contentLength > maxBodySize || bread > maxBodySize)
		throw ft::PayloadTooLarge();

	std::cout << BLACK << "Appending to buffer (original size " << this->data.size() << ", appending " << bread << " bytes" << RESET << std::endl;
	this->data.append(buffer, bread);
	std::cout<< BLACK << "New buffer size: " << this->data.size() << RESET << std::endl;

	// std::cout << BLACK << "Data: " << this->data << RESET << std::endl;

	if (this->data.size() > maxBodySize)
		throw ft::PayloadTooLarge();

	// IF its a get request, we check for the \r\n\r\n
	// IF its a post request we compare against the content-length instead.

	// Check if we've received the full header
	if (!this->headerDone)
		this->headerDone = this->data.find("\r\n\r\n") != std::string::npos;
}

//////////////////////////////////////////

bool ft::Request::isHeaderDone(void) const
{
	return (this->headerDone);
}

//////////////////////////////////////////

bool ft::Request::isBodyDone(void) const
{
	if (!this->headerDone) // Do we even have the header.
		return (false);
	else if (this->method == ft::Exchange::Method::POST) // If we have the header we can check if the content length matches the data length.
	{
		int32_t bodySize = (int32_t) this->data.length();
		std::cout << BLACK << "content-length expected " << this->contentLength << ", current size " << bodySize <<RESET<<std::endl;
		return (this->contentLength <= bodySize);
	}
	return (true);
}

//////////////////////////////////////////

void ft::Request::parseBody()
{
	// TODO: why is this commented?
	//if (this->method != ft::Exchange::Method::POST)
	//	return;
	return;
}

//////////////////////////////////////////

// Extracts the header and parses it into the request object
void ft::Request:: parseHeader(ft::Connection& conn)
{
	size_t pos;
	if ((pos = this->data.find("\r\n\r\n")) == std::string::npos)
	{
		std::cout << RED << "Error: request header end not found" << RESET << std::endl;
		throw std::exception(); // there is no body! run!!
	}

	// We have gotten the data up until the body, we can now parse the header
	// BuffA = Header | BuffB = Body
	std::pair<std::string, std::string> splitBuff(this->data.substr(0, pos), this->data.substr(pos));
	std::istringstream iss(splitBuff.first);
	this->header_data = splitBuff.first;

	// Parse the status line
	std::string line;
	std::getline(iss, line);
	this->parseStatusLine(line);

	// Parse the headers
	std::pair<std::string, std::string> header;
	while (std::getline(iss, line))
	{
		// std::cout << BLACK << "Parsing line " << line << RESET << std::endl;
		if (line.find(':') == std::string::npos)
			throw ft::BadRequest();

		ft::slice(line, ":", header);

		if (header.first.empty() || header.second.empty())
			throw ft::BadRequest();
		ft::tolower(header.first);
		ft::trim(header.first);
		ft::trim(header.second);
		this->headers[header.first] = header.second;
	}

	// Check which server this request belongs to
	const std::string* host;
	if ((host = this->getHeaderValue("host")))
	{
		// Fist: Name | Second: Port
		std::pair<std::string, std::string> output;
		ft::slice(*host, ":", output);

		// Check if name matches that of any of our servers.
		// Ignore port and just pick first match, and if no match is found pick first server.
		for (auto& server : this->servers)
		{
			auto host = server.config.getValue("server_name");
			auto port = server.config.getValue("listen");
			if (port && host)
			{
				// TODO: Check port, can be optional
				if (*host == output.first)
				{
					conn.server = const_cast<ft::Server*>(&server);
					break;
				}
				// Not the right server, keep going...
			}
			else throw std::exception(); // Not set ?
		}
		if (!conn.server) // TODO: take into account the port
			conn.server = const_cast<ft::Server*>(&this->servers.front());
	}
	else
	{
		std::cout << RED << "Host not set in header" << RESET << std::endl;
		throw ft::BadRequest(); // for HTTP/1.1 Host NEEDS to be present!!!
	}

	if (this->headers.count("content-length"))
		this->contentLength = std::stoi(this->headers["content-length"]);

	// Empty the data
	this->data.clear();
	this->headerParsed = true;

	// Now split the body from the data and parse that next
	if (splitBuff.second.size() > 0)
	{
		splitBuff.second.erase(0, splitBuff.second.find_first_not_of(WHITESPACE)); // trim whitespace from left
		this->data = splitBuff.second;
		this->parseBody();
	}
}

//////////////////////////////////////////

void ft::Request::parseStatusLine(const std::string& line)
{
	std::vector<std::string> values;
	ft::split(line, ' ', values);

	if (values.size() != 3) throw ft::BadRequest();

	if (values[0] == "GET")
		this->method = ft::Exchange::Method::GET;
	else if (values[0] == "POST")
		this->method = ft::Exchange::Method::POST;
	else if (values[0] == "DELETE")
		this->method = ft::Exchange::Method::DELETE;
	else
		throw ft::NotImplemented();

	this->path = values[1];
	this->version = values[2];
	ft::trim(this->path);
	ft::trim(this->version);

	if (this->path[0] != '/')
		throw ft::BadRequest(); // Path should always start with a /
	if (this->version != "HTTP/1.1")
		throw ft::BadRequest(); // TODO: change to 505 HTTP version not supported
}
