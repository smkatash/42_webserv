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
#include <random>
#include "responseUtils.hpp"

std::string findCurrentTimeGMT()
{
	std::time_t now = std::time(NULL);
	std::tm* gmTimeNow = std::gmtime(&now);

	char buffer[80];
	std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmTimeNow);

	std::stringstream ss("");
	ss << buffer;
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
	if (directory[directory.size() - 1] != '/')
		directory += "/";
	directory.insert(0, ".");
	DIR* dir;
	struct dirent* ent;
	if ((dir = opendir(directory.c_str())) == NULL)
		throw std::runtime_error("server error");
	std::vector<std::string>::iterator it;
	while((ent = readdir(dir)) != NULL)
	{
		it = std::find(files.begin(), files.end(), ent->d_name);
		if (it != files.end())
		{
			std::string fileName = ent->d_name;
			closedir(dir);
			return fileName;
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

bool Pred(char a, char b)
{
	if (a == b && a == '/')
		return 1;
	return 0;
}

std::string removeDuplicateSlashes(const std::string& str)
{
	std::string beforeQuery = str.substr(0, str.find('?'));
	std::string::iterator last = beforeQuery.begin();
	last = std::unique(beforeQuery.begin(), beforeQuery.end(), &Pred);
	beforeQuery.erase(last, beforeQuery.end());
	if (beforeQuery.length() != 1 && beforeQuery[beforeQuery.size() - 1] == '/')
		beforeQuery.erase(beforeQuery.size() - 1);
	if (str.find('?') != std::string::npos)
		return beforeQuery + str.substr(str.find('?'));
	return beforeQuery;
}

std::string unchunkData(const std::string& data) {
	std::string num;
	std::string chunk;
	std::string body;
	u_long	byt;

	for (size_t i = 0; i < data.length(); i++) {
		if (data[i] == '0')
			break;
		while (isxdigit(data[i])) {
			num.push_back(data[i]);
			i++;
		}
		if (data[i] && data[i] == '\r' && data[++i] == '\n') {
			std::stringstream ss;
			ss << std::hex << num;
			ss >> byt;

			// byt = std::stoi(num);
			num.clear();
		}

		while (data[++i]) {
			if (data[i] && data[i] == '\r' && data[++i] == '\n') {
				break;
			}
			chunk.push_back(data[i]);
		}
		if (chunk.length() != byt)
			chunk = chunk.substr(0, byt);
		body += chunk;
		chunk.clear();
	}
	return body;
}

std::string get_uuid()
{
	static std::random_device dev;
	static std::mt19937 rng(dev());

	std::uniform_int_distribution<int> dist(0, 15);

	const char *v = "0123456789abcdef";
	const bool dash[] = { 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0 };

	std::string res;
	for (int i = 0; i < 16; i++) {
		if (dash[i]) res += "-";
		res += v[dist(rng)];
		res += v[dist(rng)];
	}
	return res;
}
