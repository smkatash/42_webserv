#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <dirent.h>
#include <exception>
#include <vector>
#include <algorithm>
#include <map>
#include "response_utils.hpp"

std::string findCurrentTimeGMT()
{
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::time_t timeNow = std::chrono::system_clock::to_time_t(now);

	std::tm* gmTimeNow = std::gmtime(&timeNow);

	std::stringstream ss("");
	ss << std::put_time(gmTimeNow, "%a, %d %b %Y %H:%M:%S GMT");
	return ss.str();
}

std::string findContentType(std::string fileExtension)
{
	std::ifstream file("./config/mime.types");
	if (!file.is_open() || !file.good())
		throw std::runtime_error("server error");
	std::map<std::string, std::string> mimeTypes;
	std::string line;
	while (std::getline(file, line))
	{
		if (line.empty() || line[0] == '#')
			continue;
		std::string key = line.substr(0, line.find_first_of('\t'));
		std::string val = line.substr(line.find_last_of('\t') + 1);
		mimeTypes[key] = val;
	}
	file.close();
	std::map<std::string, std::string>::iterator it = mimeTypes.find(fileExtension);
	if (it == mimeTypes.end())
		return "";
	return it->second;
}

std::string findUsableFile(std::vector<std::string> files, std::string directory)
{
	DIR* dir;
	struct dirent* ent;
	if ((dir = opendir(directory.c_str())) == NULL)
		throw std::runtime_error("bad directory");
	std::vector<std::string>::iterator it;
	while((ent = readdir(dir)) != NULL)
	{
		it = std::find(files.begin(), files.end(), ent->d_name);
		if (it != files.end())
		{
			closedir(dir);
			return std::string(ent->d_name);
		}
	}
	closedir(dir);
	return "";
}

bool isNumber(std::string str)
{
	if (str.empty())
		return false;
	for (size_t i = 0; i < str.length(); i++)
	{
		if (!isdigit(str[i]))
			return false;
	}
	return true;
}

int	strtoi(std::string const& str)
{
	int i;
	std::stringstream(str) >> i;
	return i;
}

bool Pred(char a, char b)
{
	if (a == b && a == '/')
		return 1;
	return 0;
}

std::string removeDuplicateSlashes(const std::string& str)
{
	// TODO: Should remove slashes only until '?'
	std::string res = str;
	std::string::iterator last = res.begin();
	last = std::unique(res.begin(), res.end(), &Pred);
	res.erase(last, res.end());
	if (res.back() == '/' && res.length() != 1)
		res.pop_back();
	return res;
}
