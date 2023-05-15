#include <iostream>
#include <dirent.h>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>

static std::string getTemplateHtml() {
	std::ifstream templateFile("./response/autoindex/template.html");
	if (!templateFile.is_open() || !templateFile.good()) {
		std::cerr << "Autoindex html failed to open!" << std::endl;
		return "";
	}
	std::stringstream	buffer;
	buffer << templateFile.rdbuf();
	return buffer.str();;
}

static std::string insertHeader(std::string endpoint) {
	std::string templateFile = getTemplateHtml();
	std::string	targetPos = "<header>";
	std::string	header = "\n<h1>...";
	header += endpoint;
	header += "</h1>";
	size_t pos = templateFile.find(targetPos);
	if (pos != std::string::npos) {
		templateFile.insert(pos + targetPos.length(), header);
	}
	return templateFile;

}

static std::string getListedDir(std::string root) {
	struct dirent *dir;
	std::vector<std::string> dir_content;
	std::string files;

	root.insert(0, ".");
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
		files += "\n<li>";
		files += *it;
		files += "</li>";
	}
	return files;
}

std::string	initAutoIndex(std::string endpoint, std::string root) {
	std::string templateFile = insertHeader(endpoint);
	std::string	targetPos = "<ul class=\"item5\">";
	size_t pos = templateFile.find(targetPos);
	std::string files = getListedDir(root);
	if (pos != std::string::npos) {
		templateFile.insert(pos + targetPos.length(), files);
	}
	return templateFile;
}
