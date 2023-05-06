#include <iostream>


int main() {
	std::string locationConfig = "/user/";
	std::string locationRoot = "var/bin/userlist";

	std::string requestURI = "/user/";
	std::string fullPathInfo = requestURI;
	fullPathInfo.replace(0, locationConfig.size(), locationRoot);
	std::cout << fullPathInfo << std::endl;
}