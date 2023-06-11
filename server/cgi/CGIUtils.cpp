#include "CGIHandler.hpp"


std::string base64Decode(const std::string& data) {
	const char	fillchar = '=';
	static std::string  cvt = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
								"abcdefghijklmnopqrstuvwxyz"
								"0123456789+/";
	std::string::size_type  i;
	char c;
	char ch;
	std::string::size_type  len = data.length();
	std::string  decoded;

	for (i = 0; i < len; ++i)
	{
		if (data[i] == ' ')
			continue;
		c = (char) cvt.find(data[i]);
		++i;
		ch = (char) cvt.find(data[i]);
		c = (c << 2) | ((ch >> 4) & 0x3);
		decoded.push_back(c);
		if (++i < len)
		{
			c = data[i];
			if (fillchar == c)
				break;
			c = (char) cvt.find(c);
			ch = ((ch << 4) & 0xf0) | ((c >> 2) & 0xf);
			decoded.push_back(ch);
		}
		if (++i < len)
		{
			ch = data[i];
			if (fillchar == ch)
				break;
			ch = (char) cvt.find(ch);
			c = ((c << 6) & 0xc0) | ch;
			decoded.push_back(c);
		}
	}
	return(decoded);
}

std::string	getCwd() {
	char path[MAX_PATH_LEN];

	if (!getcwd(path, MAX_PATH_LEN)) {
		throw std::runtime_error("Failed to get current working directory.");
	}
	return std::string(path);
}

std::string	getCgiAbsolutePath(int type) {
	switch (type) {
		case PHP:
			return getAbsolutePath(PHP_ROOT, PHP_CGI_PATH);
			break;
		case PYTHON:
			return getAbsolutePath(PHP_ROOT, PYTHON_CGI_PATH);
			break;
		case PERL:
			return getAbsolutePath(PHP_ROOT, PERL_CGI_PATH);
			break;
		default:
			break;
	}
	throw std::runtime_error("Script type not recognized.");

}

static void	checkFilePath(const std::string& path) {
	if (path.find(".tmp") == std::string::npos) {
		std::ifstream file(path, std::ios::binary);
		if (!file.is_open())
			throw std::runtime_error("Failed to find file");
	}
}

std::string getAbsolutePath(std::string rootPath, std::string scriptPath) {

	std::string currentDir = getCwd();
	if (rootPath.length() > 0 && rootPath[rootPath.length()] != '/') {
		rootPath.insert(0, "/");
	}
	std::cout << currentDir << std::endl;
	if (currentDir.length() > 0 && currentDir[currentDir.length()] != '/'){
		currentDir += rootPath;
		if (scriptPath.length() > 0 && scriptPath[0] != '/') {
			scriptPath.insert(0, "/");
		}
		currentDir += scriptPath;
		std::cout << currentDir << std::endl;
	}
	else if (currentDir.length() > 0 && currentDir[currentDir.length()] == '/') {
		currentDir.erase(currentDir.length() - 1);
		currentDir += scriptPath;
	}
	else
		throw std::runtime_error("Failed to get absolute path to the cgi script");
	std::cout << currentDir << std::endl;

	for (size_t i = 0; i < currentDir.length() - 1; ++i) {
		if (currentDir[i] == '/' && currentDir[i + 1] == '/') {
			currentDir.erase(i + 1, 1);
			std::cout << "Erasing: " << currentDir << std::endl;
		}
	}
	checkFilePath(currentDir);
	return currentDir;
}

char **setArgArray(std::string cgiPath, std::string scripPath) {
	size_t len1 = strlen(cgiPath.c_str()) + 1;
	size_t len2 = strlen(scripPath.c_str()) + 1;

	char **argv = (char**)malloc(sizeof(char*) * 3);
	argv[0] = (char *)malloc(sizeof(char) * len1);
	argv[1] = (char *)malloc(sizeof(char) * len2);
	strcpy(argv[0], cgiPath.c_str());
	strcpy(argv[1], scripPath.c_str());
	argv[2] = NULL;

	return argv;
}

void	freeArgArray(char** argv) {
	free(argv[0]);
	free(argv[1]);
	free(argv);
}