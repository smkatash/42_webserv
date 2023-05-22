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
	size_t i = 0;
	servers_ = serversCreate();
	while (i < servers_.size())
	{
		if ( servers_[i].serverInit() == false)
			std::cout << "server Initializzation error" << std::endl;
		i++;
	}
	populateListeningMap(servers_);
}

std::vector <Server> Core::serversCreate()
{
	size_t i = 0;
	std::vector<ConfigFile> serversConfig;
	std::vector <Server> servers;

	serversConfig = configs_.getArrayConfigFile();
	while( i < serversConfig.size())
	{
		Server server(serversConfig[i]);
		servers.push_back(server);
		i++;
	}
	return servers;
}


Core::~Core()
{
}

void Core::populateListeningMap(std::vector <Server> servers)
{
	size_t i = 0;
	while ( i < servers.size())
	{
		listeningSockets_.insert(std::pair<int, Server>(
				servers[i].getServerSocketDescriptor(), servers[i]));
		i++;
	}
}

void Core::populateMap(Socket socket)
{
	int sD;
	
	sD = socket.getSocketDescriptor();
	sockets_.insert(std::pair<int, Socket>(sD, socket));
}

bool Core::setNewConnection(Server server)
{
	Socket newSocket(server.getPort(), server.getServerAddress(), server.getServerSocketDescriptor());
	if(newSocket.socketInit() == false)
	{
		printf("error socket init");
		return (false);
	}
	if (server.appendNewToSocketList(newSocket) == false)
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
	struct timespec timeout;  // Timeout structure
	timeout.tv_sec = 1;
	timeout.tv_nsec = 0; // Timeout duration in nanoseconds


	struct kevent currentEvent;
	for(i = 0; i < 10; i++)
		memset(&eventlist_[i], 0, sizeof(eventlist_[i]));
	printf("Server Listening\n");

	while(1)
	{
		i = 0;
		numOfEvent = kevent(kqFd, NULL, 0, eventlist_, MAX_EVENT, &timeout);
		while( i < numOfEvent)
		{
			currentEvent = eventlist_[i];
			tmpEventDescriptor = currentEvent.ident;
			std::map<int, Server>::iterator serversIterator = listeningSockets_.find(tmpEventDescriptor);
			if (serversIterator != listeningSockets_.end()) //here we should check a vector of serverFd;
			{
				if(setNewConnection(serversIterator->second) == false) // map populated with socket
				{
					printf("setNewConnectionError\n");
					exit(0);
				}
			}
			else
			{
				std::map<int, Socket>::iterator socketIterator = sockets_.find(tmpEventDescriptor);
				if(socketIterator != sockets_.end())
				{
					RequestParser request;
					if (currentEvent.filter == EVFILT_READ)
					{
						if(socketIterator->second.readHandler(currentEvent.data) >= 0)
						{
							std::cout << "<<------------------------------------------------------------------------------------------------------------------------>>" << std::endl;
							std::cout << "La demande:" << std::endl;
							std::cout << socketIterator->second.getData() << std::endl;
							std::cout << "<<------------------------------------------------------------------------------------------------------------------------>>" << std::endl;
							request.initParser(socketIterator->second.getData());
							ResponseHandler response(request.getRequest(), configs_.getConfigFile());
							response.handle();
							socketIterator->second.setResponse(response.generate());
							std::cout << "<<------------------------------------------------------------------------------------------------------------------------>>" << std::endl;
							std::cout << "La réponse:" << std::endl;
							std::cout << response.generate() << std::endl;
							std::cout << "<<------------------------------------------------------------------------------------------------------------------------>>" << std::endl;
							// it->second.writeHandler(it->second.getResponse());
						}
					}
					if (currentEvent.filter == EVFILT_WRITE && !socketIterator->second.getResponse().empty() )
					{
						if (socketIterator->second.completeTransfer_ == true)
						{
							socketIterator->second.writeHandler(socketIterator->second.getResponse());
							close(tmpEventDescriptor);
							sockets_.erase(tmpEventDescriptor);
						}
					}
				}
				else
				{
					std::cout << "We don't find it in the map" << std::endl;
					// TODO: INTERNAL SERVER ERROR
				}
			}
			i++;
		}
	}
}
