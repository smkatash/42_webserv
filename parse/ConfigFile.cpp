#include "ConfigFile.hpp"

ConfigFile::ConfigFile(): listen_(-1), serverName_(""), root_(""), clientMaxBodySize_(0) {}

ConfigFile::~ConfigFile() {}

/********************* setters *************************/

void	ConfigFile::setListenPort(int port) {
	listen_ = port;
}

void	ConfigFile::setServerName(std::string host) {
	serverName_ = host;
}  
  
void	ConfigFile::setRoot(std::string endpoint, std::string path) {
	if (path.length() > 1 && path[path.length() - 1] != '/') {
		path += '/';
	}
	if (!endpoint.empty()) {
		std::map<std::string, t_endpoint>::iterator	it = location_.find(endpoint);
		if (it != location_.end())
			it->second.lroot = path;
	} else {
		root_ = path;
	}
}

void	ConfigFile::setIndexFile(std::string endpoint, std::string file) {
	if (!endpoint.empty()) {
		std::map<std::string, t_endpoint>::iterator it = location_.find(endpoint);
		if (it != location_.end()) {
			it->second.lindex.push_back(file);
		}
	} else {
		indexFile_.push_back(file);
	}
}

void	ConfigFile::setErrorFile(int statusCode, std::string path) {
	errorFile_.insert(std::pair<int, std::string>(statusCode, path));
}

void	ConfigFile::setMethod(std::string endpoint, int	method) {
	std::map<std::string, t_endpoint>::iterator it = location_.find(endpoint);
		if (it != location_.end()) {
			it->second.lmethod.push_back(method);
		}
}

void	ConfigFile::setCGI(std::string endpoint, std::string lang, std::string path) {
	std::map<std::string, t_endpoint>::iterator it = location_.find(endpoint);
	if (it != location_.end()) {
		it->second.lcgi.insert(std::pair<std::string, std::string>(lang, path));
	}
}

void	ConfigFile::setRedirect(std::string endpoint, std::string redir) {
	std::map<std::string, t_endpoint>::iterator it = location_.find(endpoint);
	if (it != location_.end()) {
		it->second.lredirect = redir;
	}
}

void	ConfigFile::setAutoIndex(std::string endpoint, bool opt) {
	std::map<std::string, t_endpoint>::iterator it = location_.find(endpoint);
	if (it != location_.end()) {
		it->second.lautoindex = opt;
	}
}

void	ConfigFile::setLocation(std::string endpoint) {
	t_endpoint	locPath;
	if (endpoint.compare("/") == 0)
		locPath.lindex = indexFile_;
	else if (endpoint.length() > 0 && endpoint[endpoint.length() - 1] == '/')
		endpoint.erase(endpoint.length() - 1);
	location_.insert(std::pair<std::string, t_endpoint>(endpoint, locPath));
}

void	ConfigFile::setClientMaxBodySize(unsigned long sizeMax) {
	clientMaxBodySize_ = sizeMax;
}

/********************* getters *************************/

int	ConfigFile::getListenPort(void) const {
	return listen_;
}

const std::string	ConfigFile::getServerName(void) const {
	return	serverName_;
}

const std::string	ConfigFile::getRoot(std::string endpoint) const {
	if (!endpoint.empty()) {
		std::map<std::string, t_endpoint>::const_iterator	it = location_.find(endpoint);
		if (it != location_.end())
			return it->second.lroot;
		return "";
	} else {
		return root_;
	}
}

const std::vector<std::string>	&ConfigFile::getIndexFile(std::string endpoint) const {
	if (!endpoint.empty()) {
		std::map<std::string, t_endpoint>::const_iterator	it = location_.find(endpoint);
		if (it != location_.end())
			return it->second.lindex;
	}
	return indexFile_;
}

const std::map<int, std::string>	&ConfigFile::getErrorFile(void) const {
	return errorFile_;
}

const std::string	ConfigFile::getScriptCGI(std::string endpoint, std::string type) const {
	std::cout << "Here" << std::endl;
	std::cout << endpoint << " " << type << std::endl;
	if (!endpoint.empty() && !type.empty()) {
		std::map<std::string, t_endpoint>::const_iterator it = location_.find(endpoint);
		if (it != location_.end()) {
			std::map<std::string, std::string>::const_iterator itc = it->second.lcgi.find(type);
			if (itc != it->second.lcgi.end()) {
				std::cout << "Here2" << std::endl;
				std::cout << itc->second << std::endl; 
				return itc->second;
			}
		}
	}
	return NULL;
}

const t_endpoint	&ConfigFile::getLocation(std::string endpoint) const {
	std::map<std::string, t_endpoint>::const_iterator it = location_.find(endpoint);
	if (it != location_.end())
		return it->second;
	throw std::runtime_error("endpoint not found");
}

const std::string	ConfigFile::getEndPoint(std::string name) const {
	if (name.empty())
		return "";
	std::map<std::string, t_endpoint>::const_iterator it = location_.find(name);
	if (it != location_.end())
		return it->first;
	return "";
}

unsigned long	ConfigFile::getClientMaxBodySize(void) const {
	return clientMaxBodySize_;
}


void	ConfigFile::clear(void) {
	listen_ = 0;
	serverName_.clear();
	root_.clear();
	indexFile_.clear();
	clientMaxBodySize_ = 0;
	location_.clear();
	errorFile_.clear();
}

/********************* debugger *************************/

void	ConfigFile::debugConfigFile(void) {
	std::cout << "Listen port: " << listen_ << std::endl;
	std::cout << "Server name: " << serverName_ << std::endl;
	std::cout << "Root: " << root_ << std::endl;

	std::cout << "Index files: " << std::endl;
	std::vector<std::string>::iterator	it = indexFile_.begin();
	for (; it != indexFile_.end(); ++it) {
		std::cout << *it << " ";
	}
	std::cout << std::endl;

	std::cout << "Error files: " << std::endl;
	std::map<int, std::string>::iterator	itm = errorFile_.begin();
	for (; itm != errorFile_.end(); ++itm) {
		std::cout << itm->first << " " << itm->second << " | ";
	}
	std::cout << std::endl;

	std::cout << "Max client bodysize: " <<  clientMaxBodySize_ << std::endl;

	std::cout << "Location files : " << std::endl;
	for (std::map<std::string, t_endpoint>::iterator itm = location_.begin(); itm != location_.end(); ++itm) {
		std::cout << std::endl << "location name: " << itm->first << " "; 
		std::cout << std::endl << "methods: ";
		for (std::vector<int>::iterator it1 = itm->second.lmethod.begin(); it1 != itm->second.lmethod.end(); ++it1)
			std::cout << *it1 << " ";
		std::cout << std::endl << "root: " << itm->second.lroot;
		std::cout << std::endl << "index files: ";
		for (std::vector<std::string>::iterator it2 = itm->second.lindex.begin(); it2 != itm->second.lindex.end(); ++it2)
			std::cout << *it2 << " ";
		std::cout << std::endl << "cgi files: ";
		for (std::map<std::string, std::string>::iterator it3 = itm->second.lcgi.begin(); it3 != itm->second.lcgi.end(); ++it3) {
			std::cout << it3->first << ": " << it3->second << " ";
		}
		std::cout << std::endl << "redirects: " << itm->second.lredirect;
		std::cout << std::endl << "autoindex: " << std::boolalpha << itm->second.lautoindex;
	}
	std::cout << std::endl;
}