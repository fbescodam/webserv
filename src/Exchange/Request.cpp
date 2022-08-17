/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Request.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/27 11:07:39 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/08/17 12:18:13 by lde-la-h      ########   odam.nl         */
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

// @see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Host
void ft::Request::checkHostHeader(ft::Connection& conn)
{
	const std::string* host;
	if (!(host = this->getHeaderValue("host")))
    {
        std::cout << RED << "Host header not set" << RESET << std::endl;
        throw ft::BadRequest();
    }
    
    // <host>:<port>
	std::pair<std::string, std::string> output;
	ft::slice(*host, ":", output);

    // Check which server this host matches to
	for (auto& server : this->servers)
    {
        const std::string* servHost = server.config.getValue("server_name");
		const std::string* servPort = server.config.getValue("listen");

        // Check if we messed up
        if (!servHost || !servPort)
            throw std::exception();

        // Check that name matches, if it does then check if port is set, if it is, chck that it matches.
        if (*servHost != output.first || (!output.second.empty() && *servPort != output.second))
            continue;
        
        conn.server = const_cast<ft::Server*>(&server);
        break;
    }

    // If not found then we simply leave at the default server set by poll.
    // Since there we default to the first server found on the requested port.
}

static bool isValid(const std::string& str)
{
    return std::any_of(str.begin(), str.end(), [](char c) { 
        return (isprint(static_cast<unsigned char>(c))); 
    });
};

//////////////////////////////////////////

// Extracts the header and parses it into the request object
void ft::Request::parseHeader(ft::Connection& conn)
{
    // Check if we have found the separator, we haven't its probably a bad request.
	size_t pos;
	if ((pos = this->data.find("\r\n\r\n")) == std::string::npos)
	{
		std::cout << RED << "Error: Request separator missing!" << RESET << std::endl;
		throw ft::BadRequest();
	}

	// We have gotten the data up until the body, we can now parse the header
	std::pair<std::string, std::string> splitBuff(this->data.substr(0, pos), this->data.substr(pos));
	std::istringstream ss(splitBuff.first);
	this->header_data = splitBuff.first;

    // Check the status line
    this->parseStatusLine(ss);

	std::string line;
    std::pair<std::string, std::string> header;
	while (std::getline(ss, line))
    {
        // Find separator of the header entry.
        if (line.find(':') == std::string::npos)
			throw ft::BadRequest();

        // Slice and trim for spaces.
        ft::slice(line, ":", header);
        ft::trim(header.first);
		ft::trim(header.second);

        // If either field is empty, or contains printable (like bytes or null) data
		if (header.first.empty() || header.second.empty() || !isValid(header.first) || !isValid(header.second))
			throw ft::BadRequest();

		ft::tolower(header.first);
		this->headers[header.first] = header.second;
    }

    // Check if the host header is set correctly.
    // HTTP/1.1 ALWAYS has this set!
    this->checkHostHeader(conn);

    // Erase header data and leave behind only the body.
	if (this->headers.count("content-length"))
		this->contentLength = std::stoi(this->headers["content-length"]);

	// Empty the data
	this->data.clear();
	this->headerParsed = true;

	// Now split the body from the data and parse that next
	if (splitBuff.second.size() > 0)
	{
        // Front trim from left to remove separator.
		splitBuff.second.erase(0, splitBuff.second.find_first_not_of(WHITESPACE));
		this->data = splitBuff.second;
	}
}

//////////////////////////////////////////

void ft::Request::parseStatusLine(std::istringstream& ss)
{
	std::string line;
	std::getline(ss, line);

    // Split on spaces for METHOD, VERSION and PATH
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
