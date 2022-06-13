/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Response.hpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 19:13:27 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/13 18:14:06 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef RESPONSE_HPP
# define RESPONSE_HPP
# include "Exchange.hpp"
FT_BEGIN

/** As one giant shit strings
 * CODE - STATUS
 * <HEADER>
 * \n\n 
 * <BODY>
 */

// A Response is from the outgoing server.
class Response final : public Exchange
{
public: // Ctor ~ Dtor

public: // Functions

	/**
	 * Sends the response over HTTP to the socket.
	 * 
	 * @param socket The socket to send the response to.
	 * @param buffer The body of the response.
	 * @return if sending was successful.
	 */
	void send(int32_t socket, const std::string& buffer);

	static ft::Response getError(int32_t code);

private:
	void writeHeader(std::vector<uint8_t>& buffer);

public: // Attributes

};

FT_END
#endif
