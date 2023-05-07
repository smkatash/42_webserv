
#include "../parse/ConfigFile.hpp"


class Server
{
    const std::vector<Socket>			&sockets;
    ConfigFile config

    public:
        Server(&ConfigFile);
        Server(const Server &other);
        Server &operator=(const Server &other);
        ~Server();
}