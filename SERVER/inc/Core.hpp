#ifndef CORE_HPP
#define CORE_HPP

#include "webserv.hpp"

class Core
{
	private:
	std::vector <Server> servers_;
	Server server_;
	std::map <int, Socket> socketMap_;
	int kqFd_; //the kqueue() return a fd;

	std::vector <Socket> initSocket(int max);
	std::vector <Server> initServers(ConfigFile conf);

	public: //
	// Core(std::vector <ConfigFile> servers, int kqFd);
	Core();
	~Core();
};

#endif