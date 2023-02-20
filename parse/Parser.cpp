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
	if (stripBrackets(&p) && isdigitString(p))
		return std::stoi(p);
	else
		throw std::invalid_argument("parser: port invalid");
}

std::string	Parser::checkServerName(std::string s) {
	if (stripBrackets(&s)) {
		if (s.empty())
			s = "localhost";
	}
	else 
		throw std::invalid_argument("parser: server name invalid");
	return s;
}

std::string	Parser::checkRoot(std::string r) {
	if (stripBrackets(&r)) {
		if (!r.empty())
			return r;
	}
	throw std::invalid_argument("parser: root invalid");
}

std::string	Parser::checkIndexFile(std::string i) {
	const std::string html_ = ".html";
	const std::string php_ = ".php";
	if (i.empty() || i.find_last_of(".") == std::string::npos || (i.substr(i.find_last_of(".")) != html_ && \
		i.substr(i.find_last_of(".")) != php_ ))
		throw std::invalid_argument("parser: invalid index type");
	return i;
}

int	Parser::checkErrorCode(std::string errorCode) {
	if (isdigitString(errorCode)) {
		int err = std::stoi(errorCode);
		if (err == NOTFOUND || err == INTERNALERROR)
			return err;
	}
	throw std::invalid_argument("parser: invalid error code");
	
}

std::string	Parser::checkErrorPage(std::string errorPage) {
	if (stripBrackets(&errorPage)) {
		if (!errorPage.empty())
			return errorPage;
	}
	return errorPage;
}

void	Parser::checkLocation(std::string endpoint) {
	if (endpoint.empty())
		throw std::invalid_argument("parser: invalid location");
}

std::string	Parser::checkPath(std::string p) {
	if (stripBrackets(&p)) {
		if (!p.empty())
			return p;
	}
	throw std::invalid_argument("parser: invalid cgi path");
	
}

bool	Parser::checkAutoIndex(std::string indx) {
	if (stripBrackets(&indx)) {
		if (!indx.empty() && (indx == "on"))
			return true;
		if (!indx.empty() && (indx == "off"))
			return false;
	}
	throw std::invalid_argument("parser: invalid autoindex");
}

bool	Parser::endDirectiveLocation(std::string next, std::string *locationDir) {
	if (!(*locationDir).empty() && (next == "location" || next == "}")) {
		(*locationDir).clear();
		return true;
	}
	else if (next == "server") {
		std::cout << "next server\n";
		return true;
	}
	return false;
}

void	Parser::parseSyntax() {
	ConfigFile	conf;
	std::string locationDir;
	int		err = -1;
	int		i = 0;
	std::vector<std::string>	idxv;
	std::map<int, std::string>	idxm;
	std::string	buff;
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
				i++;
				while (!input_[i].empty() && input_[i].back() != ';') {
					conf.setIndexFile(locationDir, checkIndexFile(input_[i]));
					i++;
				}
				if (stripBrackets(&input_[i]))
					conf.setIndexFile(locationDir, checkIndexFile(input_[i]));
				else
					throw std::invalid_argument("parser: invalid index type");
				idxv = conf.getIndexFile(locationDir);
				for (auto it = idxv.begin(); it != idxv.end(); ++it) {
					std::cout << *it << " ";
				}
				std::cout << std::endl;
				break;
			case ERROR_PAGE:
				err = checkErrorCode(input_[++i]);
				conf.setErrorFile(err, checkErrorPage(input_[++i]));
				idxm = conf.getErrorFile();
				for (auto it = idxm.begin(); it != idxm.end(); it++) {
					std::cout << it->first << " " << it->second << ", ";
				}
				std::cout << std::endl;
				break;
			case LOCATION:
				std::cout << "Found location directive with argument " << input_[i] << std::endl;
				locationDir = input_[++i];
				checkLocation(locationDir);
				conf.setLocation(locationDir);
				if (input_[++i].compare("{"))
					throw std::invalid_argument("parser: unbalanced brackets");
				std::cout << "endpoint " << conf.getEndPoint("/") << std::endl;
				break;
			case METHOD:
				std::cout << "Found method directive with argument " << input_[i] << std::endl;
				i++;
				while (!input_[i].empty() && input_[i].back() != ';') {
					conf.setMethod(locationDir, isValidMethod(input_[i]));
					i++;
				}
				if (stripBrackets(&input_[i]))
					conf.setMethod(locationDir, isValidMethod(input_[i]));
				else
					throw std::invalid_argument("parser: invalid method");
				break;
			case CGI:
				std::cout << "Found CGI directive with argument " << input_[i] << std::endl;
				buff = input_[++i];
				isValidLanguage(buff);
				conf.setCGI(locationDir, buff, checkPath(input_[++i]));
				break;
			case AUTOINDEX:
				std::cout << "Found autoindex directive with argument " << input_[i] << std::endl;
				conf.setAutoIndex(locationDir, checkAutoIndex(input_[++i]));
				break;
			default:
				buff = input_[i];
				if (!input_[++i].empty() && buff == "}") {
					if (!endDirectiveLocation(input_[i], &locationDir))
						std::cerr << "!!! Unknown directive " << input_[i] << std::endl;
				}
				std::cerr << "Generic Unknown directive " << input_[i] << std::endl;
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

