/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Response.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/27 11:07:35 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/28 11:46:25 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

//////////////////////////////////////////

void ft::Response::writeHead(int32_t status)
{
	// this->data += ft::format("HTTP/1.1 %u %s\n", status, ft::getStatusCodes().at(status).c_str());
    this->data += "HTTP/1.1 " + std::to_string(status) + ft::getStatusCodes().at(status);
    this->headers["Server"] = "Breadserv";
}

void ft::Response::writeHeaders(void)
{
	for (const auto [key, value] : this->headers)
		this->data += key + " : " + value + '\n';
	this->data += "\n";
}

void deleteMethod(const std::string& filePath)
{
    if (FILE EXISTS )
    {
        REMOVE
        SEND 202
    }
    else
    {
        SEND 404
    }
}

void postMethod(const std::string& filePath)
{
   // TODO: ???
}

void getMethod(void)
{
    // TODO: ???
}

ft::Response::Status send(int32_t socket)
{
    // TODO: What ?

    off_t len = 0;
    if (sendfile(this->fileFd, socket, this->fileOffset, &len, NULL, 0) < 0)
        std::cerr << RED << "Sendfile function for response failed" << RESET << std::endl;
    this->fileOffset += len;

    return (this->fileOffset >= this->fileSize ? ft::Response::Status::DONE : ft::Response::Status::NOT_DONE);
}

//////////////////////////////////////////
