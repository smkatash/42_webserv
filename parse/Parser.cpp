#include "Parser.hpp"

Parser::Parser(std::ifstream& file) {
	if (file.is_open() && file.good()) {
		std::istream_iterator<std::string>	start(file), end;
		input_ = std::vector<std::string>(start, end);
		count_ = 0;
	} else {
		throw std::invalid_argument("parser: unable to open file");
	}
	if (input_.empty())
		throw std::invalid_argument("parser: empty config_file");
};

Parser::~Parser() {};

/********************* tokenizer *************************/

Token	Parser::getToken(const std::string& str) {
	static std::string tokens[14] = {"server", "listen", "server_name",
									"root", "index", "error_page", "location",
									"method", "cgi", "autoindex", "}", "return",
									"max_client_body"};
	for (int i = 0; i < 14; ++i) {
		if (str == tokens[i]) {
			return static_cast<Token>(i);
		}
	}
	return NA;
}

/********************* syntax checker *************************/

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

std::string	Parser::checkRedirect(std::string redir) {
	if (stripBrackets(&redir)) {
		if (!redir.empty())
			return redir;
	}
	throw std::invalid_argument("parser: invalid redirect");
}

unsigned long	Parser::isValidLimit(std::string num) {
	if (stripBrackets(&num) && isdigitString(num)) {
		unsigned long  max = std::stoi(num);
		if (max > 0 && max < ULONG_MAX)
			return max;
	}
	throw std::invalid_argument("parser: invalid redirect");
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
	if (next == "location") {
		(*locationDir).clear();
		return true;
	}
	return false;
}


/********************* parser *************************/

void	Parser::parseSyntax() {
	ConfigFile					conf;
	std::string 				locationDir;
	std::string					buff;
	int							server_count = 0;
	int							err = -1;
	unsigned long				i = 0;
	do {
		Token tok = getToken(input_[i]);
		switch (tok) {
			case SERVER:
				if (input_[++i].compare("{"))
					throw std::invalid_argument("parser: unbalanced brackets");
				server_count++;
				break;
			case LISTEN:
				conf.setListenPort(checkPort(input_[++i]));
				break;
			case SERVER_NAME:
				conf.setServerName(checkServerName(input_[++i]));
				break;
			case ROOT:
				conf.setRoot(locationDir, checkRoot(input_[++i]));
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
				break;
			case ERROR_PAGE:
				err = checkErrorCode(input_[++i]);
				conf.setErrorFile(err, checkErrorPage(input_[++i]));
				break;
			case LOCATION:
				locationDir = input_[++i];
				checkLocation(locationDir);
				conf.setLocation(locationDir);
				if (input_[++i].compare("{"))
					throw std::invalid_argument("parser: unbalanced brackets");
				break;
			case METHOD:
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
				buff = input_[++i];
				isValidLanguage(buff);
				conf.setCGI(locationDir, buff, checkPath(input_[++i]));
				break;
			case AUTOINDEX:
				conf.setAutoIndex(locationDir, checkAutoIndex(input_[++i]));
				break;
			case BR:
				if (locationDir.empty() && server_count) {
					conf_.push_back(conf);
					conf.clear();
					server_count--;
				}
				else if (!locationDir.empty())
					locationDir.clear();
				else
					throw std::invalid_argument("parser: unbalanced brackets");
				break;
			case REDIRECT:
				conf.setRedirect(locationDir, checkRedirect(input_[++i]));
				break;
			case MAX_CLIENT:
				conf.setClientMaxBodySize(isValidLimit(input_[++i]));
				break;
			default:
				std::cerr << "Generic Unknown directive " << input_[i] << std::endl;
				break;
	}
	} while(++i < input_.size());
}

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


/********************* setter *************************/

void	Parser::setConfigFile() {
	parseBrackets();
	parseSyntax();
	// debugConfigVector();
}

/********************* getter *************************/

ConfigFile	Parser::getConfigFile() {
	return conf_.front(); // TODO: Needs attention
}

/********************* debugger *************************/

void	Parser::debugConfigVector() {
	std::vector<ConfigFile>::iterator it = conf_.begin();
	for (; it != conf_.end(); it++) {
		(*it).debugConfigFile();
		std::cout << std::endl;
	}
}

void	Parser::debugInput() {
	std::cout << "Debug: \n";
	if (input_.empty())
		std::cout << "empty\n";
	for (std::vector<std::string>::iterator it = input_.begin(); it != input_.end(); ++it) {
		std::cout << *it << "|";
	}
}

