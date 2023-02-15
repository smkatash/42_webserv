#include "ConfigFile.hpp"


ConfigFile::ConfigFile() {}
ConfigFile::~ConfigFile() {}

bool	isValid(std::string var) {
	return var.empty();
}

void	ConfigFile::setListen(int port) {
	listen_ = port;
}

void	ConfigFile::setServer(std::string host) {
	serverName_ = host;
}

void	ConfigFile::setRoot(std::string path) {
	root_ = path;
}

void	ConfigFile::setIndexFile(std::string file, std::string path) {
	if 
}
