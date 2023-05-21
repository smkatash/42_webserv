#ifndef CORE_HPP
#define CORE_HPP

#include "Server.hpp"
#include "Parser.hpp"
#include <map>

#define MAX_EVENT 10000

class Core
{
	private:
	Parser	configs_;
	Server	server_;
	std::vector <Server> servers_;
	// std::map <int, class Socket> socketMap_;
	struct kevent eventlist_[MAX_EVENT];
	std::map<int, Server> listeningSockets_; //this are the servers
	std::map<int, Socket> sockets_;

	// std::vector <Server> initServers(ConfigFile conf);
	
	public: //
	// Core(std::vector <ConfigFile> servers, int kqFd);
	Core(Parser configs);
	~Core();
	void run();
	void populateListeningMap(std::vector <Server> );
	void populateMap(Socket socket);
	std::vector <Server> serversCreate();

	bool setNewConnection(Server server);
};

#endif