#pragma once

#ifndef SERVER_HPP
# define SERVER_HPP

#include "Request.hpp"
#include "Response.hpp"
#include "CommonNetwork.hpp"




class Server
{
public:
	Server(Config& config);

	void init(void);
	void run(void);

	void newSocket(void);
	void pollInEvent(int32_t i);
	void pollOutEvent(int32_t i);

	Config _config;

	//listening socket
	int32_t serverFD, clientSocket, ValRead;
	ft::SocketAddress Address;

	//running part
	int32_t maxClients;
	int32_t numFds;
	pollfd* pollfds;
	nfds_t nfds;

	std::map<int, ft::Request> requests; //idk if this is the best solution yet

	//tempory standard sendback message
	const char* hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
};


#endif
