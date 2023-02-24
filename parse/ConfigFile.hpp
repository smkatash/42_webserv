#ifndef CONFIGFILE_HPP
#define CONFIGFILE_HPP

#include <iostream>
#include <vector>
#include <map>
#include <utility>

typedef struct s_endpoint {
	std::vector<int>						lmethod;
	std::string 							lroot;
	std::vector<std::string>				lindex;
	std::map<std::string, std::string>		lcgi;
	std::string 							lredirect;
	bool			 						lautoindex;
}											t_endpoint;

class	ConfigFile {
	private:
		int									listen_;
		std::string							serverName_;
		std::string							root_;
		std::vector<std::string>			indexFile_;
		std::map<int, std::string>			errorFile_;
		std::map<std::string, t_endpoint>	location_;
		unsigned long						clientMaxBodySize_;

	public:
		ConfigFile();
		~ConfigFile();

		/* setters */
		void								setListenPort(int port);
		void								setServerName(std::string host);
		void								setRoot(std::string endpoint, std::string path);
		void								setIndexFile(std::string file, std::string path);
		void								setErrorFile(int statusCode, std::string path);
		void								setMethod(std::string endpoint, int	method);
		void								setCGI(std::string endpoint, std::string lang, std::string path);
		void								setRedirect(std::string endpoint, std::string redir);
		void								setAutoIndex(std::string endpoint, bool opt);
		void								setLocation(std::string endpoint);
		void								setClientMaxBodySize(unsigned long sizeMax);

		/* getters */
		const int							getListenPort(void) const;
		const std::string					getServerName(void) const;
		const std::string					getRoot(std::string endpoint) const;
		const std::vector<std::string>		&getIndexFile(std::string endpoint) const;
		const std::map<int, std::string>	&getErrorFile(void) const;
		const std::string					getScriptCGI(std::string endpoint, std::string type) const;
		const t_endpoint					&getLocation(std::string endpoint) const;
		const std::string					getEndPoint(std::string name) const;
		const int							getMethod(std::string endpoint) const;
		const unsigned long					getClientMaxBodySize(void) const;

		void								clear();

		/* print all to debug */
		void								debugConfigFile(void);
};





#endif // CONFIGFILE_HPP