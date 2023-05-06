#ifndef CORE_HPP
#define CORE_HPP

#include "webserv.hpp"

class Core
{
	private:

	Server server_;
	// std::vector <Server> servers_;
	std::map <int, Socket> socketMap_;
	// std::vector <Server> initServers(ConfigFile conf);

	public: //
	// Core(std::vector <ConfigFile> servers, int kqFd);
	Core();
	~Core();

	void run();
};

#endif