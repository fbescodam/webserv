/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Connection.hpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/27 11:22:23 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/31 17:18:23 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef CONNECTION_HPP
# define CONNECTION_HPP
# include <time.h>

namespace ft {

class Server;
class Request;
class Response;

// A connection to a client for example.
struct Connection
{
	pollfd*			poll;			// The pollfd used for this connection
	ft::Server*		server;			// The server to which this connection belongs.
	time_t			lastActivity;	// The timestamp pointing to the last time there was activity on this connection.
	std::string		ipv4;			// Client IP.
	ft::Request*	request;		// A request from this client to parse
	ft::Response*	response;		// A response that needs to be sent to this client
};

}
#endif
