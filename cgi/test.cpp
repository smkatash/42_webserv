#include <iostream>
#include <fstream>
#include "../requestHandler/RequestParser.hpp"
#include "../parse/ConfigFile.hpp"
#include "../parse/Parser.hpp"
#include "cgiHandler.hpp"

std::string input = 
"GET /pages/ HTTP/1.1\n\
Host: www.kanykei.de\n\
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:88.0) Gecko/20100101 Firefox/88.0\n\
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\n\
Accept-Language: en-US,en;q=0.5\n\
Accept-Encoding: gzip, deflate, br\n\
Connection: keep-alive\n\
Content-Length: 1456\n\
Content-Type: text/html\n\
Connection: keep-alive\n\
user=Kany&email=test%40email";



int main(int argc, char **argv) {
	std::ifstream	file;

	ConfigFile conf;
	try {
		file.open(argv[1]);
		Parser	confParser(file);
		confParser.setConfigFile();
		conf = confParser.getConfigFile();
	} catch(const std::runtime_error& e) {
		std::cout << e.what() << std::endl;
	}

	Request request;
	try {
		RequestParser	req(input);
		request = req.getRequest();
	} catch(const std::runtime_error& e) {
		std::cout << e.what() << std::endl;
	}


	CGIHandler cgi(request, conf, "/random");
	std::cout << "cgi is ready to execute" << std::endl;
	try {
		cgi.execute();
	} catch(const std::runtime_error& e) {
		std::cout << e.what() << std::endl;
	}
} 
