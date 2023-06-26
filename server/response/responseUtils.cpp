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

size_t determineChunkSize(const std::string& data, size_t& i)
{
	std::string num;


	size_t lineBr = data.find("\r\n", i);
	if (lineBr == std::string::npos)
		return 0;
	for (; i < lineBr; i++) {
		if (isxdigit(data[i]))
			num.push_back(data[i]);
		else
			return 0;
	}
	std::stringstream ss;
	size_t ret;
	ss << std::hex << num;
	ss >> ret;
	i += 2;
	return ret;
}

std::string unchunkData(const std::string& data)
{
	std::string newData;
	std::string chunk;
	size_t      chunkSize;

	size_t dataLen = data.length();
	for (size_t i = 0; i < dataLen; i++) {
		chunkSize = determineChunkSize(data, i);
		if (chunkSize == 0)
			break;
		chunk = data.substr(i, chunkSize);
		i += chunkSize + 1;
		newData += chunk;
	}
	return newData;
}

void dechunk(std::string& data)
{
	size_t seperation = data.find("\r\n\r\n");
	if (seperation == std::string::npos)
		return;
	std::string headers = data.substr(0, seperation);
	std::string body = unchunkData(data.substr(seperation + 4));
	data = headers + "\r\nContent-Length: " + toString(body.length()) + "\r\n\r\n" + body;
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

void addContentLength(std::string& cgi)
{
	size_t bodyLength;

	if (cgi.find("Content-Length") != std::string::npos)
		return;

	size_t bodyLoc = cgi.find("\r\n\r\n");
	if (bodyLoc == std::string::npos)
		bodyLength = 0;
	else
	{
		std::string body = cgi.substr(bodyLoc + 4);
		bodyLength = body.size();
	}
	std::string contentLength = "Content-Length: " + toString(bodyLength) + "\r\n";
	cgi.insert(cgi.find("\n") + 1, contentLength);
}
