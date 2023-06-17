#include "Parser.hpp"
#include "Core.hpp"
#include "color.hpp"
#include <fstream>

bool	configFileOpen(int argc, char* argv[], std::ifstream& file)
{
	if (argc < 2) {
		file.open("./config/default.conf");
		return true;
	}
	else if (argc == 2) {
		const std::string ext = ".conf";
		std::string input = argv[1];
		if (input.find_last_of(".") == std::string::npos || \
			input.substr(input.find_last_of(".")) != ext) {
			std::cerr << "error: file must have .conf extension" << std::endl;
			return false;
		}
		file.open(argv[1]);
		return true;
	}
	else {
		std::cerr << "usage: ./webserv config_file\n";
	}
	return false;
}

Parser configInit(std::ifstream& file)
{
	Parser	confParser;

	try {
		confParser.openFile(file);
		confParser.setConfigFile();
	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << '\n';
		exit(EXIT_FAILURE);
	}
	return confParser;
}

int kqFd = 0;

bool kqCreate() {
	kqFd = kqueue();
	if (kqFd < 0 )
		return (false);
	return (true);
}


// void leaksCheck() {
// 	system("leaks webserv");
// }

int main(int argc, char **argv)
{
	std::ifstream	file;
	Parser			configs;

	//atexit(leaksCheck);
	if (!configFileOpen(argc, argv, file))
		return EXIT_FAILURE;
	configs = configInit(file);

	if (!kqCreate())
		return EXIT_FAILURE;

	Core core(configs);
	if(core.status() == false)
		return(printError("ERROR: shade... \t\t| you could config better next time :(", 0));
	core.run();
	return EXIT_SUCCESS;
}
