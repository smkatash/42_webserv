#ifndef CORE_HPP
#define CORE_HPP

#include "Server.hpp"
#include "Parser.hpp"
#include "RequestParser.hpp"
#include <map>

#define MAX_EVENT 10000
#define TIMEOUT 60

class Core
{
	private:
	Parser	configs_;
	Server	server_;
	std::vector <Server> servers_;

	std::map<int, Server> listeningSockets_;
	std::map<int, Socket> sockets_;
	
	public:
	Core(Parser configs);
	~Core();

	void run();
	void populateListeningMap(std::vector <Server> );
	void populateMap(Socket socket);
	std::vector <Server> serversCreate();

	void connectionHandler(struct kevent currentEvent);
	void createResponse(Socket *socket);
	void sendResponse(Socket *socket);
	bool setNewConnection(Server server);
	bool status();
};

#endif