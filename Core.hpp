#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <ctime>

#include "./connectionHandler/Socket.hpp"
#include "./parse/Parser.hpp"
#include "Server.hpp"


class Core
{
	private:

	std::vector <Server> servers_;
	std::map <int, Socket> socketMap_;
	int kqFd_; //the kqueue() return a fd;

	std::vector <Socket> initSocket(int max);
	std::vector <Server> initServers(ConfigFile conf);

	public: //
	Core( std::vector <ConfigFile> servers, int kqFd);
	~Core();

};
