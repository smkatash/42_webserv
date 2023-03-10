#include "webserv.hpp"

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

	try {
		Parser	conf(file);
		conf.setConfigFile();
		conf.debugInput();
	} catch (std::invalid_argument& e) {
		std::cerr << e.what() << '\n';
		exit(EXIT_FAILURE);
	} catch (const std::out_of_range & e) {
		std::cerr << e.what() << "\n";
		exit(EXIT_FAILURE);
	}
	
}