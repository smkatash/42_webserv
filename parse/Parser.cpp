#include "Parser.hpp"

Parser::Parser(std::ifstream& file) : count_(0) {
	if (file.is_open() && file.good()) {
		std::istream_iterator<std::string>	start(file), end;
		input_ = std::vector<std::string>(start, end);
	} else {
		throw std::invalid_argument("parser: unable to open file");
	}
	if (input_.empty())
		throw std::invalid_argument("parser: empty config_file");
};

Parser::~Parser() {};

void	Parser::parseBrackets() {
	std::vector<std::string>::iterator	it = input_.begin();
	int	openBr = 0;
	int	closeBr = 0;
	for (; it != input_.end(); ++it) {
		if (*it == "{")
			openBr++;
		else if (*it == "}")
			closeBr++;
	}
	if (openBr != closeBr)
		throw std::invalid_argument("parser: unbalanced brackets");
}

Token	Parser::getToken(const std::string& str) {
	static std::string tokens[14] = {"server", "listen", "server_name",
										"root", "index", "error_page", "location",
										"method", "cgi", "autoindex"};
	for (int i = 0; i < 14; ++i) {
		if (str == tokens[i]) {
			return static_cast<Token>(i);
		}
	}
	return NA;
}

int		Parser::checkPort(std::string p) {
	if (p.back() == ';') {
		p.erase(p.end() - 1);
		for (int c = 0; c < p.length(); c++) {
			if (!isdigit(p[c]))
				throw std::invalid_argument("parser: port invalid");
		}
	} else
		throw std::invalid_argument("parser: port invalid");
	if (p.empty())
		throw std::invalid_argument("parser: port invalid");
	return std::stoi(p);
}

std::string	Parser::checkServerName(std::string s) {
	if (s.back() == ';') {
		s.erase(s.end() - 1);
		if (s.empty())
			s = "localhost";
	}
	else 
		throw std::invalid_argument("parser: server name invalid");
	return s;
}

std::string	Parser::checkRoot(std::string r) {
	if (r.back() == ';') {
		r.erase(r.end() - 1);
		if (r.empty())
			throw std::invalid_argument("parser: root invalid");
	}
	else 
		throw std::invalid_argument("parser: root invalid");
	return r;
}

std::string	Parser::checkIndexFile(std::string i) {
	const std::string html_ = ".html";
	const std::string php_ = ".php";
	if (i.empty() || i.find_last_of(".") == std::string::npos || (i.substr(i.find_last_of(".")) != html_ && \
		i.substr(i.find_last_of(".")) != php_ ))
		throw std::invalid_argument("parser: invalid index type");
	return i;
}

void	Parser::parseSyntax() {
	ConfigFile	conf;
	bool	bracketServer;
	bool	bracketLocation;
	static	std::string locationDir;
	int	i = 0;
	std::vector<std::string> idx;
	do {
		Token tok = getToken(input_[i]);
		switch (tok) {
			case SERVER:
				std::cout << "Found server directive" << std::endl;
				if (input_[++i].compare("{"))
					throw std::invalid_argument("parser: unbalanced brackets");
				break;
			case LISTEN:
				conf.setListenPort(checkPort(input_[++i]));
				std::cout << "PORT " << conf.getListenPort() << "\n";
				break;
			case SERVER_NAME:
				conf.setServerName(checkServerName(input_[++i]));
				std::cout << "ServerName " << conf.getServerName() << "\n";
				break;
			case ROOT:
				conf.setRoot(locationDir, checkRoot(input_[++i]));
				std::cout << "Root " << conf.getRoot(locationDir) << "\n";
				break;
			case INDEX:
				std::cout << "Found index directive with argument " << input_[i] << std::endl;
				i++;
				while (!input_[i].empty() && input_[i].back() != ';') {
					conf.setIndexFile(locationDir, checkIndexFile(input_[i]));
					i++;
				}
				if (input_[i].empty())
					throw std::invalid_argument("parser: invalid index type");
				input_[i].erase(input_[i].end() - 1);
				conf.setIndexFile(locationDir, checkIndexFile(input_[i]));
				idx = conf.getIndexFile(locationDir);
				for (auto it = idx.begin(); it != idx.end(); ++it) {
					std::cout << *it << " ";
				}
				std::cout << std::endl;
				break;
			case ERROR_PAGE:
				std::cout << "Found error_page directive with argument " << input_[i] << std::endl;
				i++;
				break;
			case LOCATION:
				std::cout << "Found location directive with argument " << input_[i] << std::endl;
				break;
			case METHOD:
				std::cout << "Found method directive with argument " << input_[i] << std::endl;
				break;
			case CGI:
				std::cout << "Found CGI directive with argument " << input_[i] << std::endl;
				break;
			case AUTOINDEX:
				std::cout << "Found autoindex directive with argument " << input_[i] << std::endl;
				break;
			default:
				std::cout << "Unknown directive: " << input_[i] << std::endl;
				break;
	}
	} while(++i < input_.size());
}

void	Parser::setConfigFile() {
	parseBrackets();
	parseSyntax();
}


void	Parser::debugInput() {
	std::cout << "Debug: \n";
	if (input_.empty())
		std::cout << "empty\n";
	for (std::vector<std::string>::iterator it = input_.begin(); it != input_.end(); ++it) {
		std::cout << *it << "|";
	}
}

