/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CommonNetwork.cpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/25 19:34:04 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/30 15:43:26 by pvan-dij      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "CommonNetwork.hpp"
#include "Exceptions.hpp"

//////////////////////////////////////////

// Socket creates an endpoint for communication and returns a descriptor.
int32_t ft::socket(int32_t Domain, int32_t Type, int32_t Protocol)
{
	int32_t fd = ::socket(Domain, Type, Protocol);
	if (fd < 0)
		throw ft::GenericErrnoException();
	return (fd);
}

// Assigns a name to an unnamed socket, requests that address be assigned to the socket.
void ft::bind(int32_t socketFD, ft::SocketAddress* address)
{
	socklen_t length = address->getSize();

	if (::bind(socketFD, reinterpret_cast<sockaddr*>(address), length) < 0)
		throw ft::GenericErrnoException();
}

// Defines the maximum length for the queue of pending connections.
void ft::listen(int32_t socketFD, int32_t BackLog = 128)
{
	if (::listen(socketFD, BackLog) < 0)
		throw ft::GenericErrnoException();
}

// Extracts the first connection request on the queue of pending connections
int32_t ft::accept(int32_t socketFD, ft::SocketAddress* address)
{
	socklen_t length = address->getSize();

	int32_t fd = ::accept(socketFD, reinterpret_cast<sockaddr*>(address), &length);
	if (fd < 0)
		throw ft::GenericErrnoException();
	return (fd);
}

// Examines a set of file descriptors to see if some of them are ready for I/O or if certain events have occurred on them.
int32_t ft::poll(struct pollfd fds[], size_t size, int32_t timeout)
{
	int32_t FDs = ::poll(fds, size, timeout);
	if (FDs < 0)
		throw ft::GenericErrnoException();
	return (FDs);
}

// Send a message to a socket.
ssize_t ft::send(int32_t socket, const void* buffer, size_t length, int32_t flags)
{
	ssize_t Value = ::send(socket, buffer, length, flags);
	if (Value == -1)
		throw ft::GenericErrnoException();
	return (Value);
}

// Receive a message from a socket.
ssize_t ft::receive(int32_t socket, void* buffer, size_t length, int32_t flags)
{
	ssize_t Value = ::recv(socket, buffer, length, flags);
	if (Value == -1)
		throw ft::GenericErrnoException();
	return (Value);
}

// Manipulate the options associated with a socket.
int32_t ft::setSocketOption(int32_t socket, int32_t level, int32_t optionName, bool optionValue, size_t optionLen)
{
	int32_t Value = setsockopt(socket, level, optionName, &optionValue, optionLen);
	if (Value == -1)
		throw ft::GenericErrnoException();
	return (Value);
}

std::string ft::inet_ntop(SocketAddress& address) 
{
	// 4 bytes, IPV4
	static char buf[sizeof(int32_t) * 4];
	sockaddr_in temp = address.getCStyle();

	if (::inet_ntop(AF_INET, &temp, buf, sizeof(buf)) == nullptr)
		return "";
	return std::string(buf);
}

const std::map<uint16_t, std::string>& ft::getStatusCodes()
{
	static std::map<uint16_t, std::string> list = {
		// 1xx Informational
		{100, "Continue"},
		{101, "Switching Protocols"},
		{102, "Processing"},

		// 2xx Success
		{200, "OK"},
		{201, "Created"},
		{202, "Accepted"},
		{203, "Non-authoritative Information"},
		{204, "No Content"},
		{205, "Reset Content"},
		{206, "Partial Content"},
		{207, "Multi-Status"},
		{208, "Already Reported"},
		{226, "IM Used"},

		// 3xx Redirection
		{300, "Multiple Choices"},
		{301, "Moved Permanently"},
		{302, "Found"},
		{303, "See Other"},
		{304, "Not Modified"},
		{305, "Use Proxy"},
		{307, "Temporary Redirect"},
		{308, "Permanent Redirect"},

		// 4xx Client Error
		{400, "Bad Request"},
		{401, "Unauthorized"},
		{402, "Payment Required"},
		{403, "Forbidden"},
		{404, "Not Found"},
		{405, "Method Not Allowed"},
		{406, "Not Acceptable"},
		{407, "Proxy Authentication Required"},
		{408, "Request Timeout"},
		{409, "Conflict"},
		{410, "Gone"},
		{411, "Length Required"},
		{412, "Precondition Failed"},
		{413, "Payload Too Large"},
		{414, "Request-URI Too Long"},
		{415, "Unsupported Media Type"},
		{416, "Requested Range Not Satisfiable"},
		{417, "Expectation Failed"},
		{418, "I'm a teapot"},
		{421, "Misdirected Request"},
		{422, "Unprocessable Entity"},
		{423, "Locked"},
		{424, "Failed Dependency"},
		{426, "Upgrade Required"},
		{428, "Precondition Required"},
		{429, "Too Many Requests"},
		{431, "Request Header Fields Too Large"},
		{444, "Connection Closed Without Response"},
		{451, "Unavailable For Legal Reasons"},
		{499, "Client Closed Request"},

		//5xx Server Error
		{500, "Internal Server Error"},
		{501, "Not Implemented"},
		{502, "Bad Gateway"},
		{503, "Service Unavailable"},
		{504, "Gateway Timeout"},
		{505, "HTTP Version Not Supported"},
		{506, "Variant Also Negotiates"},
		{507, "Insufficient Storage"},
		{508, "Loop Detected"},
		{510, "Not Extended"},
		{511, "Network Authentication Required"},
		{599, "Network Connect Timeout Error"}
	};

	return (list);
}

std::string& ft::getContentType(std::string ext)
{
	static std::map<std::string, std::string> list = {
		{"html",	"text/html"},
		{"htm",		"text/html"},
		{"shtml",	"text/html"},
		{"css",		"text/css"},
		{"xml",		"text/xml"},
		{"mml",		"text/mathml"},
		{"txt",		"text/plain"},
		{"jad",		"text/vnd.sun.j2me.app-descriptor"},
		{"wml",		"text/vnd.wap.wml"},
		{"htc",  	"text/x-component"},
		
		{"js",		"application/javascript"},
		{"atom",	"application/atom+xml"},
		{"rss",		"application/rss+xml"},
		{"7z",		"application/x-7z-compressed"},

		{"gif",		"image/gif"},
		{"jpeg",	"image/jpeg"},
		{"jpg",		"image/jpeg"},
		{"avif",	"image/avif"},
		{"png",		"image/png"},
		{"svg",		"image/svg+xml"},
		{"svgz",	"image/svg+xml"},
		{"tif",		"image/tif"},
		{"tiff",	"image/tiff"},
		{"wbmp",	"image/wbmp"},
		{"webp",	"image/webp"},
		{"ico",		"image/ico"},
		{"jng",		"image/jng"},
		{"bmp",		"image/bmp"},
		
		{"json",	"application/json"},

		{"mid",		"audio/midi"},
		{"midi",	"audio/midi"},
		{"kar",		"audio/midi"},
		{"mp3",		"audio/mpeg"},
		{"ogg",		"audio/ogg"},
		{"m4a",		"audio/x-m4a"},
		{"ra",		"audio/x-realaudio"},

		{"mid",		"audio/midi"},
		{"midi",	"audio/midi"},
		{"kar",		"audio/midi"},
		{"mp3",		"audio/mpeg"},
		{"ogg",		"audio/ogg"},
		{"m4a",		"audio/x-m4a"},
		{"ra",		"audio/x-realaudio"},

		{"3gpp",	"video/3gpp"},
		{"3gp",		"video/3gpp"},
		{"mp4",		"video/mp4"},
		{"mpeg",	"video/mpeg"},
		{"mpg",		"video/mpeg"},
		{"mov",		"video/mov"},
		{"m4a",		"video/webm"},
	};

	// Find the damn thing
	auto reslt = list.find(ext.substr(ext.find_last_of(".") + 1));

	if (reslt == list.end())
		return (list["html"]); // Default
	return ((*reslt).second);
}

//////////////////////////////////////////
