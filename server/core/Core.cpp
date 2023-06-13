#include "Core.hpp"
#include "RequestParser.hpp"
#include "ResponseHandler.hpp"
#include "responseUtils.hpp"
#include "color.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <signal.h>
#include <ctime>

// #define DEBUG

bool loop = true;

static void catchCtrlC(int sigNum)
{
	if(sigNum)
		;
	loop = false;
}

//////////////////////////////////////////////////// canonic functions:
Core::Core(Parser configs) : configs_(configs)
{
	Server server;
	size_t i = 0;
	servers_ = serversCreate();
	while (i < servers_.size())
	{
		if ( servers_[i].serverInit() == false)
		{
			printError("ERROR: Initializzation error \t| SERVER port: ", servers_[i].getPort());
			servers_.erase(servers_.begin() + i);
		}
		else
			i++;
	}
	populateListeningMap(servers_);
}

Core::~Core()
{
}

//////////////////////////////////////////////////// member functions:

bool	Core::status()
{
	if( listeningSockets_.empty())
		return false;
	else	
		return true;
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
	Socket newSocket(server.getPort(), \
		server.getServerAddress(), \
		server.getServerSocketDescriptor(),\
		server.getConfig());
	if(newSocket.socketInit() == false)
		return(printError("ERROR: error socket init \t\t| SERVER port:", server.getPort()));
	if (server.appendNewToSocketList(newSocket) == false)
		return(printError("ERROR: error in socket list \t\t| SERVER port:", server.getPort()));
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

// void Core::createResponse(Socket* socket)
// {
// 	RequestParser      request;
// 	static std::string firstRequest; // This should be inside socket
// 	static std::string chunks; // This should be inside socket

// 	#ifdef DEBUG
// 		printLaDemande(socket->getData(), socket->getPort());
// 	#endif

// 	// TODO: Try catch for some reason

// 	if (socket->getChunkedOpt() == true)
// 	{
// 		chunks += socket->getData();
// 		socket->setResponse("HTTP/1.1 202 Accepted");
// 	}

// 	if (chunks.find("0\r\n\r\n") != std::string::npos)
// 		socket->setChunkedOpt(false); // We reached the last chunk, we stop storing in chunks string

// 	if (socket->getChunkedOpt() == false)
// 	{
// 		if (!chunks.empty())
// 		{
// 			size_t headerLoc = firstRequest.find("Expect: 100-continue");
// 			if (headerLoc != std::string::npos)
// 				firstRequest.erase(headerLoc, 22); // size of header + 2 Because we erase the \r\n as well

// 			headerLoc = firstRequest.find("Transfer-Encoding: chunked");
// 			if (headerLoc != std::string::npos)
// 				firstRequest.erase(headerLoc, 28); // size of header + 2 Because we erase the \r\n as well

// 			firstRequest += chunks;
// 			dechunk(firstRequest);
// 		}
// 		else
// 			firstRequest = socket->getData();
// 		request.initParser(firstRequest);
// 		ResponseHandler response(request.getRequest(), socket);
// 		response.handle();
// 		socket->setResponse(response.generate());
// 		chunks.clear();
// 	}
// }

void Core::createResponse(Socket *socket)
{
	RequestParser      request;

	#ifdef DEBUG
		printLaDemande(socket->getData(), socket->getPort());
	#endif

	// TODO: Try catch for some reason
	std::string& data = socket->getDataRef();
	if (socket->getChunkedOpt() == true && socket->getData().find("0\r\n\r\n") != std::string::npos)
	{
		size_t headerLoc = data.find("Transfer-Encoding: chunked");
		if (headerLoc != std::string::npos)
			data.erase(headerLoc, 28); // size of header + 2 Because we erase the \r\n as well
		dechunk(data);
		socket->setChunkedOpt(false); // We reached the last chunk, we stop storing in chunks string
	}
	request.initParser(data);
	size_t headerLoc = data.find("Expect: 100-continue");
	if (headerLoc != std::string::npos)
		data.erase(headerLoc, 22); // size of header + 2 Because we erase the \r\n as well

	ResponseHandler response(request.getRequest(), socket->getServerConfiguration());
	response.handle();
	socket->setResponse(response.generate());
}


static  bool connectionToKeepAlive( std::string response)
{
	size_t index = 0;

	std::string substring ("Connection: close");
	index = response.find(substring);
	if(index == std::string::npos)
		return (false);
	return (true);
}

bool Core::sendResponse( Socket *socket)
{
	#ifdef DEBUG
		printLaResponse(socket->getResponse(), socket->getPort());
	#endif

	bool connection = connectionToKeepAlive(socket->getResponse());
	if (socket->writeHandler(socket->getResponse(), connection) == false)
	{
		socket->closeConnection();
		sockets_.erase(socket->getSocketDescriptor());
		return (false);
	}
	else
		return true;
}

static std::map<int, Socket> checkTimeout(std::map<int, Socket> sockets)
{
	time_t check;
	time_t start;

	std::map<int, Socket> socketsCopy;
	socketsCopy = sockets;

	check = time(NULL);
	std::map<int, Socket>::iterator socket;
	for (socket = socketsCopy.begin(); socket != socketsCopy.end(); socket++)
	{
		start = socket->second.getConnectionTimer();
		if((int)(check - start) > TIMEOUT)
		{
			printAction("ACTION: \033[38;5;189mTIMED OUT \t\t\t\t\033[38;5;49m| SOCKET sd: ", socket->first);
			socket->second.closingConnectionServerSide();
			sockets.erase(socket->second.getSocketDescriptor());
		}
	}
	return (sockets);
}

void Core::connectionHandler(struct kevent currentEvent)
{
	int socketDescriptor = currentEvent.ident;
	std::map<int, Socket>::iterator socketIterator = sockets_.find(socketDescriptor);
	if(socketIterator != sockets_.end())
	{
		////////////////////////////// READ EVENT //////////////////////
		if (currentEvent.filter == EVFILT_READ && \
			socketIterator->second.getConnectionStatus() == true && \
			socketIterator->second.getRequestStatus() == false)
		{
			if( socketIterator->second.readHandler(currentEvent.data) == 0)
				sockets_.erase(socketIterator->second.getSocketDescriptor());
			else if(socketIterator->second.getRequestStatus() == true)
				createResponse(&(socketIterator->second));
			else
				;
		}
		////////////////////////////// WRITE EVENT /////////////////////
		if (currentEvent.filter == EVFILT_WRITE && \
			socketIterator->second.getConnectionStatus() == true && \
			socketIterator->second.getRequestStatus() == true)
		{
			if (socketIterator->second.getConnectionStatus() == true)
			{
				if(sendResponse(&(socketIterator->second)) == false)
				;
					// sockets_.erase(socketIterator->second.getSocketDescriptor());
			}
			if (socketIterator->second.getConnectionStatus() == false)
				sockets_.erase(socketIterator->second.getSocketDescriptor());
		}
	}
}

void	Core::run()
{
	signal(SIGINT, catchCtrlC);

	int i;
	int tmpEventDescriptor;
	int numOfEvent;

	struct timespec refresh = setTimer(1,0);
	struct kevent currentEvent;


	for(i = 0; i < MAX_EVENT; i++)
		memset(&eventlist_[i], 0, sizeof(eventlist_[i]));
	printHeader();

	while (loop)
	{
		i = 0;
		numOfEvent = kevent(kqFd, NULL, 0, eventlist_, MAX_EVENT, &refresh);
		sockets_ = checkTimeout(sockets_);
		while (i < numOfEvent)
		{
			currentEvent = eventlist_[i];
			tmpEventDescriptor = currentEvent.ident;
			std::map<int, Server>::iterator serversIterator = listeningSockets_.find(tmpEventDescriptor);
			if (serversIterator != listeningSockets_.end()) //here we should check a vector of serverFd;
			{
				if(setNewConnection(serversIterator->second) == false) // map populated with socket
				{
					std::cout << "setNewConnectionError\n" << std::endl;
					exit(0); //! We shouldn't exit and risk breaking the server, we should instead continue to the next event
				}
			}
			else
				connectionHandler(currentEvent);
			i++;
		}
	}
	printFooter();
	return;
}
