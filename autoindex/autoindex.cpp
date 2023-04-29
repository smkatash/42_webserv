#include <iostream>
#include <dirent.h>
#include <vector>
#include <algorithm>


std::string	initAutoIndex(std::string endpoint, std::string root) {
	struct dirent *dir;
	std::vector<std::string> dir_content;

	DIR *dh = opendir(root.c_str());
	/* dir not found or does not exist */
	if (!dh) {
		return "";
	} else {
		while ((dir = readdir(dh)) != NULL) {
			if ((std::string)dir->d_name == ".." || (std::string)dir->d_name == ".")
				continue;
			dir_content.push_back((std::string)dir->d_name);
		}
		closedir(dh);
	}
	/* sorted content */
	std::sort(dir_content.begin(), dir_content.end());
	std::vector<std::string>::iterator it = dir_content.begin();
	for (; it != dir_content.end(); it++) {
		std::cout << *it << " | ";
	}
	std::cout << std::endl;
	return "done";
}

int main() {
	std::cout << std::endl << initAutoIndex("/test/","../responseHandler");

}