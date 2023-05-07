#ifndef CORE_HPP
# define CORE_HPP

# include "webserv.hpp"
# include "ConfigFile.hpp"
# include "Server.hpp"

// class Server;

class Core {
public:
	Core();
	~Core();

	void	config(std::string fileconf);
	int		setup(void);
	void	run(void);
	void	clean(void);

private:
	int							kqueueFd;
	
	Config						config;
	std::map<long, Server>		servers;
	std::map<long, Server *>	sockets;
	std::vector<int>			ready;
	fd_set						fd_set;
	unsigned int				fd_size;
	long						max_fd;
};

#endif
