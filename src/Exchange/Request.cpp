/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Request.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/27 11:07:39 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/31 17:11:34 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

ft::Request::Request(void) {
	this->buffer = "";
}

//////////////////////////////////////////

// Appends up until it managed to append enough data to build the header.
ft::Exchange::Status ft::Request::appendBuffer(const std::string& buffer)
{
	// Check if request is malformed.
	if (buffer.size() < 1)
		throw ft::BadRequest();
	else if (!isalpha(buffer.at(0)))
		throw ft::BadRequest();

	this->data += buffer;

	// No body, ask to continue.
	return (this->buffer.find("\r\n\r\n") != std::string::npos ? ft::Exchange::Status::DONE : ft::Exchange::Status::NOT_DONE);
}

//////////////////////////////////////////

void ft::Request::parseBody()
{
	if (this->method != ft::Exchange::Method::POST)
		return;
	if (this->data.size() > 100000)
		throw ft::BadRequest();
}

//////////////////////////////////////////

// Extracts the headeran d parses it into the request object
void ft::Request::parseHeader()
{
	size_t pos;
	if ((pos = this->buffer.find("\r\n\r\n")) == std::string::npos)
		throw std::exception(); // there is no body! run!!

	// We have gotten the data up until the body, we can now parse the header
	// BuffA = Header | BuffB = Body
	std::pair<std::string, std::string> splitBuff(this->data.substr(0, pos), this->data.substr(pos));
	std::istringstream iss(splitBuff.first);

	// Parse the status line
	std::string line;
	std::getline(iss, line);
	this->parseStatusLine(line);

	// Parse the headers
	std::pair<std::string, std::string> header;
	while (std::getline(iss, line))
	{
		if (line.find(':') == std::string::npos)
			throw ft::BadRequest();

		ft::slice(line, ":", header);

		if (header.first.empty() || header.second.empty())
			throw ft::BadRequest();
		this->headers[header.first] = header.second;
	}

	// Check which server this request belongs to
	const std::string* host = this->getHeaderValue("Host");
	if (host)
	{

		//TODO: request belongs to the first server with this name, OR IF IT DOES NOT EXIST, the first server on this port
	}
	else
	{
		// TODO: request belongs to first server on this port
	}

	// Empty the data
	this->data.empty();

	// Now split the body from the data and parse that next
	if (splitBuff.second.size() > 0)
	{
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
}
