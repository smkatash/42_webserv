#include "utils.hpp"

bool	isRequestLine(std::string line) {
	if ((line.find("OPTIONS") != std::string::npos ||
		line.find("GET") != std::string::npos ||
		line.find("HEAD") != std::string::npos ||
		line.find("POST") != std::string::npos ||
		line.find("PUT") != std::string::npos ||
		line.find("DELETE") != std::string::npos ||
		line.find("TRACE") != std::string::npos ||
		line.find("CONNECT") != std::string::npos ||
		line.find("CONNECT") != std::string::npos) &&
		line.find("HTTP") != std::string::npos)
		return true;
	return false;
}

bool	isGeneralHeader(std::string line) {
	if (line.find("Cache-Control") != std::string::npos ||
		line.find("Connection") != std::string::npos ||
		line.find("Date") != std::string::npos ||
		line.find("Pragma") != std::string::npos ||
		line.find("Trailer") != std::string::npos ||
		line.find("Transfer-Encoding") != std::string::npos ||
		line.find("Upgrade") != std::string::npos ||
		line.find("Via") != std::string::npos ||
		line.find("Warning") != std::string::npos)
		return true;
	return false;
}

bool	isRequestHeader(std::string line) {
	if (line.find("Accept") != std::string::npos ||
		line.find("Authorization") != std::string::npos ||
		line.find("Expect") != std::string::npos ||
		line.find("From") != std::string::npos ||
		line.find("Host") != std::string::npos ||
		line.find("If-") != std::string::npos ||
		line.find("Max-Forwards") != std::string::npos ||
		line.find("Proxy-Authorization") != std::string::npos ||
		line.find("Range") != std::string::npos ||
		line.find("Cookie") != std::string::npos ||
		line.find("Referer") != std::string::npos ||
		line.find("TE") != std::string::npos ||
		line.find("User-Agent") != std::string::npos)
		return true;
	return false;
}

bool	isEntityHeader(std::string line) {
	if (line.find("Allow") != std::string::npos ||
		line.find("Content-") != std::string::npos ||
		line.find("Expires") != std::string::npos ||
		line.find("Last-Modified") != std::string::npos)
		return true;
	return false;
}
