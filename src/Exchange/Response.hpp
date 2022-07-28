/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Response.hpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/27 11:07:33 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/28 11:43:31 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef RESPONSE_HPP
# define RESPONSE_HPP
# include "Utils.hpp"
# include "Exchange.hpp"
# include "ServerSection.hpp"
# include "DirectoryFactory.hpp"

namespace ft {

/**
 * A Response is from the outgoing server.
 *
 * E.g:
 * HTTP/1.1 503 Service Unavailabe\n
 * Content-Type: text/plain\n
 * Content-Length: 12\n
 * \n
 * 503 error
 */
class Response final : public ft::Exchange
{
public: // Ctor ~ Dtor
	Response() = default;
	~Response();

public: // Types
    enum class Status : uint8_t
    {
        DONE,
        NOT_DONE,  
    };

public: // Functions

    /**
     * @brief Send the current response object onto the socket.
     * 
     * @param socket The client socket.
     * @return ft::Response::Status The status if we are done or not.
     */
    ft::Response::Status send(int32_t socket);

private:

	void writeHead(int32_t status);
	void writeHeaders(void);

    void deleteMethod(const std::string& filePath);
	void postMethod(const std::string& filePath);
    void getMethod(void);

};
}
#endif
