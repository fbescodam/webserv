/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Request.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/27 11:07:36 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/31 13:34:57 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef REQUEST_HPP
# define REQUEST_HPP
# include <sstream>
# include "Utils.hpp"
# include "Exchange.hpp"
# include "Exceptions.hpp"
# include "ServerSection.hpp"

namespace ft {

/**
 * Request
 * A Request is from the incoming client.
 *
 * GET /reference/string/to_string/ HTTP/1.1\n
 * Content-Type: text/plain\n
 * Content-Length: 12\n
 * \r\n\r\n
 * body
 */
class Request final : public ft::Exchange
{
public: // Ctor ~ Dtor
    Request(const std::string& buffer);

public: // Functions

    ft::Exchange::Status appendBuffer(const std::string& buffer);
    void ft::Request::parseHeader();

private:
    void parseStatusLine(const std::string& buffer);

public: // Attributes
    ft::Exchange::Method method;
    std::string path;
    std::string version;
    
private:
    std::string buffer;
};

}
#endif
