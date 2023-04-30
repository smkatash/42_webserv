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
	public:
		Core( std::vector <ConfigFile> servers);
		~Core();

	private:

	std::vector <Server> servers_;
	std::map <int, Socket> socketMap_;
	int kqFd_; //the kqueue() return a fd;

	std::vector <Socket> initSocket(int max);
	std::vector <ConfigFile> servers_ initServers(ConfigFile);
};

