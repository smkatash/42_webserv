#include "Core.hpp"
#include "RequestParser.hpp"
#include "ResponseHandler.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <ctime>

#define DEBUG

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

static struct timespec setTimer(int sec, int nsec)
{
	struct timespec timeout;
	timeout.tv_sec = sec;
	timeout.tv_nsec = nsec;
	return (timeout);
}

void Core::receiver(RequestParser *request, Socket *socket)
{
	#ifdef DEBUG
		std::cout << "LA DEMANDE ---------------------------------------------------------------------------------------------------------------->>" << std::endl;
		std::cout << socket->getData() << std::endl;
		std::cout << "<<------------------------------------------------------------------------------------------------------------------------END" << std::endl;
	#endif

	request->initParser(socket->getData());
	ResponseHandler response(request->getRequest(), configs_.getConfigFile());
	response.handle();
	socket->setResponse(response.generate());
}

void Core::sender( Socket *socket)
{
	#ifdef DEBUG
		std::cout << "LA RESPONSE--------------------------------------------------------------------------------------------------------------->>" << std::endl;
		std::cout << socket->getResponse() << std::endl;
		std::cout << "<<-----------------------------------------------------------------------------------------------------------------------END" << std::endl;
	#endif
	
	if (socket->writeHandler(socket->getResponse()) == false)
	{
		socket->closeConnection();
		sockets_.erase(socket->getSocketDescriptor());
	}
}

// static bool checkTimeout(Socket *socket)
// {
// 	time_t check;
// 	time_t start;

// 	check = time(NULL);
// 	start = socket->getConnectionTimer();
// 	if (check - start > 10) //should be the variable in the config file;
// 	{
// 		ResponseHandler response(RequestParser request, ConfigFile file);
// 		socket->closeConnection();
// 		return (false);
// 	}
// 	else
// 		socket->setConnectionTimer();
// 	return (true);
// }

void Core::connectionHandler(struct kevent currentEvent)
{
	int socketDescriptor = currentEvent.ident;
	std::map<int, Socket>::iterator socketIterator = sockets_.find(socketDescriptor);
	if(socketIterator != sockets_.end())
	{
		RequestParser request;
		if (currentEvent.filter == EVFILT_READ \
			 && socketIterator->second.getConnectionStatus() == true \
			 && socketIterator->second.getRequestStatus() == false)
		{
			// if(checkTimeout(&(socketIterator->second)) == true)
			// {
				int read_status = 0;
				read_status = socketIterator->second.readHandler(currentEvent.data);
				if(read_status == 0 )
					sockets_.erase(socketIterator->second.getSocketDescriptor());
				else if(socketIterator->second.getRequestStatus() == true)// && socketIterator->second.getRequestStatus() == false)
					receiver(&request, &(socketIterator->second)); //receiver is a request maker;
			// }
		}
		if (currentEvent.filter == EVFILT_WRITE && socketIterator->second.getRequestStatus() == true)
		{
			// if(checkTimeout(&(socketIterator->second)) == true)
			// {
				if (socketIterator->second.getConnectionStatus() == true)
					sender(&(socketIterator->second));
				sockets_.erase(socketIterator->second.getSocketDescriptor());
			// }
		}
	}
}

void	Core::run()
{
	int i;
	int tmpEventDescriptor;
	int numOfEvent;
	struct timespec refresh = setTimer(1,0);
	struct kevent currentEvent;

	for(i = 0; i < MAX_EVENT; i++)
		memset(&eventlist_[i], 0, sizeof(eventlist_[i]));
	std::cout << "Server Listening...   ╭∩╮ʕ•ᴥ•ʔ╭∩╮   <><   " << std::endl;

	while (1)
	{
		i = 0;
		numOfEvent = kevent(kqFd, NULL, 0, eventlist_, MAX_EVENT, &refresh);
		// numOfEvent = kevent(kqFd, NULL, 0, eventlist_, MAX_EVENT, NULL);
		if(numOfEvent == 0)
		{
			
		}
		while (i < numOfEvent)
		{
			currentEvent = eventlist_[i];
			tmpEventDescriptor = currentEvent.ident;
			std::map<int, Server>::iterator serversIterator = listeningSockets_.find(tmpEventDescriptor);
			if (serversIterator != listeningSockets_.end()) //here we should check a vector of serverFd;
			{
				if(setNewConnection(serversIterator->second) == false) // map populated with socket
				{
					printf("setNewConnectionError\n");
					exit(0); //! We shouldn't exit and risk breaking the server, we should instead continue to the next event
				}
			}
			else
				connectionHandler(currentEvent);
			i++;
		}
	}
}
