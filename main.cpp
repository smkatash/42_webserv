#include "Parser.hpp"
#include "Core.hpp"
#include <fstream>

void	configFileOpen(int argc, char* argv[], std::ifstream& file)
{
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
}

Parser configInit(std::ifstream& file)
{
	Parser	confParser;
	try
	{
		confParser.openFile(file);
		confParser.setConfigFile();
	}
	catch (std::invalid_argument& e)
	{
		std::cerr << e.what() << '\n';
		exit(EXIT_FAILURE);
	}
	catch (const std::out_of_range & e)
	{
		std::cerr << e.what() << "\n";
		exit(EXIT_FAILURE);
	}
	catch(const std::runtime_error& e)
	{
		std::cout << e.what() << std::endl;
	}
	return confParser;
}

int kqFd = 0;

bool kqCreate()
{
	kqFd = kqueue();
	if (kqFd < 0 )
		return (false);
	return (true);
}

int main(int argc, char **argv)
{
	std::ifstream	file;
	Parser			configs;

	configFileOpen(argc, argv, file);
	configs = configInit(file);

	if( kqCreate() == false )
		return (0);

	Core core(configs);
	core.run();
	return (EXIT_SUCCESS);
}













/* 	RequestParser req(requestUpload);
	ResponseHandler resp(req.getRequest(), confParser.getConfigFile());
	resp.post();
	std::cout << resp.generateResponse() << std::endl; */
