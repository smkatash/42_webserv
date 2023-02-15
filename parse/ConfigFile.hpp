#ifndef CONFIGFILE_HPP
#define CONFIGFILE_HPP

#include <iostream>
#include <vector>
#include <map>

typedef struct s_location {
	int										method;
	std::string 							root;
	std::vector<std::string>				index;
	std::string 							redirect;
	std::map<std::string, std::string>		cgi;
	bool			 						autoindex;
} t_location;

class	ConfigFile {
	private:
		int									listen_;
		std::string							serverName_;
		std::string							root_;
		std::vector<std::string>			indexFile_;
		std::map<int, std::string>			errorFile_;
		std::map<std::string, t_location>	location_;

	public:
		ConfigFile();
		~ConfigFile();

		bool	isValid(std::string var);
		/* setters */
		void	setListen(int port);
		void	setServer(std::string host);
		void	setRoot(std::string path);
		void	setIndexFile(std::string file, std::string path);


};

#endif // CONFIGFILE_HPP