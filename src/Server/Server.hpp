#pragma once

#ifndef SERVER_HPP
# define SERVER_HPP

#include "../Exchange/Request.hpp"
#include "../Exchange/Response.hpp"
#include "../Config/Config.hpp"
#include <poll.h>


FT_BEGIN


class Server
{
public:
	Server(Config& config);

	void init(void);
	void run(void);

	void newSocket(void);
	void pollInEvent(int i);
	void pollOutEvent(int i);

	Config _config;

	//listening socket
	int32_t serverFD, clientSocket, ValRead;
	ft::SocketAddress Address;

	//running part
	nfds_t  nfds;
	int maxClients;
	int numFds;
	struct pollfd *pollfds;

	std::map<int, ft::Request> requests; //idk if this is the best solution yet

	//tempory standard sendback message
	const char* hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
};

FT_END

#endif
