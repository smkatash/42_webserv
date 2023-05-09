#include "Parser.hpp"
#include "RequestParser.hpp"
#include "ResponseHandler.hpp"
#include <fstream>

int main(int argc, char **argv) {
	std::ifstream	file;

	if (argc < 2)
		file.open("./config/default.conf");
	else if (argc == 2) {
		const std::string ext = ".conf";
		std::string input = argv[1];
		if (input.find_last_of(".") == std::string::npos || \
			input.substr(input.find_last_of(".")) != ext) {
			std::cerr << "error: file must have .conf extension" << std::endl;
			exit(EXIT_FAILURE);
		}
		file.open(argv[1]);
	}
	else {
		std::cerr << "usage: ./webserv config_file\n";
		exit(EXIT_FAILURE);
	}

	std::string request = "POST /test HTTP/1.1\n\
Host: foo.example\n\
Content-Type: application/x-www-form-urlencoded\n\
Content-Length: 27\n\
\n\
field1=value1&field2=value2\n\
field3=value3&field4=value4";

	try {


		Parser	confParser(file);
		confParser.setConfigFile();
		// confParser.debugConfigVector();
		RequestParser req(request);
		ResponseHandler resp(req.getRequest(), confParser.getConfigFile());
		resp.get();
		std::cout << resp.generateResponse() << std::endl;


	} catch (std::invalid_argument& e) {
		std::cerr << e.what() << '\n';
		exit(EXIT_FAILURE);
	} catch (const std::out_of_range & e) {
		std::cerr << e.what() << "\n";
		exit(EXIT_FAILURE);
	} catch(const std::runtime_error& e) {
		std::cout << e.what() << std::endl;
	}

}