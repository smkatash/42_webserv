#include "Core.hpp"
#include "RequestParser.hpp"
#include "ResponseHandler.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <ctime>

Core::Core(Parser configs)
: configs_(configs)
{
	Server server;
	// if(server.serverInit())
	// 	serverClose();
	server.serverInit();
	server_ = server;

	// void eventWrite(Server server);
	// void eventRead(Server server);
	// this->servers_ = servers;
}

Core::~Core()
{
}

void Core::populateListeningMap(Socket socket)
{
	int socketDescriptor;
	
	socketDescriptor = socket.getSocketDescriptor();
	listeningSockets_.insert(std::pair<int, Socket>(socketDescriptor, socket));
}

void Core::populateMap(Socket socket)
{
	int sD;
	
	sD = socket.getSocketDescriptor();
	sockets_.insert(std::pair<int, Socket>(sD, socket));
}

bool Core::setNewConnection()
{
		// is a new connection;
	Socket newSocket(server_.getPort(), server_.getServerAddress(), server_.getServerSocketDescriptor());
	if(newSocket.socketInit() == false)
	{
		printf("error socket init");
		return (false);
	}
	if (server_.appendNewToSocketList(newSocket) == false)
	{
		printf("error append new socket");
		return (false);
	}
	populateMap(newSocket);
	return (true);
}

void	Core::run()
{
	int i;
	int tmpEventDescriptor;
	int numOfEvent;

	struct kevent currentEvent;
	for(i = 0; i < 10; i++)
		memset(&eventlist_[i], 0, sizeof(eventlist_[i]));
	printf("Server Listening\n");
	while(1)
	{
		i = 0;
		numOfEvent = kevent(kqFd, NULL, 0, eventlist_, MAX_EVENT, NULL);
		while( i <= numOfEvent)
		{
			currentEvent = eventlist_[i];
			tmpEventDescriptor = currentEvent.ident;
			// PRINT
			if (tmpEventDescriptor == server_.getServerSocketDescriptor()) //here we should check a vector of serverFd;
			{
				if(setNewConnection() == false)
				{
					printf("setNewConnectionError\n");
					exit(0);
				}
			}
			else
			{
				std::map<int, Socket>::iterator it = sockets_.find(tmpEventDescriptor);
				if(it != sockets_.end())
				{
					if(it->second.readHandler() >= 0)
					{
						RequestParser request(it->second.getData());

						std::cout << "Request:\n" << std::endl;
						std::cout << it->second.getData() << std::endl;

						ResponseHandler response(request.getRequest(), configs_.getConfigFile());

						response.handle();

						std::cout << "\n\nResponse:\n" << std::endl;
						std::cout << response.generate() << std::endl;

						it->second.writeHandler(response.generate());
						close(tmpEventDescriptor);
						sockets_.erase(tmpEventDescriptor);
					}
					else
					{
						printf("Failed:recv\n");
						// TODO: INTERNAL SERVER ERROR
						// the socket read buffer is empty;
						// bytes is < 0 we have to return error;
					}
				}
				// ssize_t  = recv(tmpEventDescriptor, buffer, BUFFER_SIZE, 0);

			}
		// should we erase the event from the queue or is done automatically? 
		// EV_SET(&event, tmpEventDescriptor, EVFILT_READ, EV_DELETE, 0, 0, NULL);
        // if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) 
		// {
        //   perror("Failed to deregister fd from kqueue");
        //   exit(EXIT_FAILURE);
        // }
			i++;
		}
	}
}
