#ifndef CONFIGFILE_HPP
#define CONFIGFILE_HPP

#include <iostream>
#include <vector>
#include <map>
#include <utility>

enum eMethods {
	GET = 0,
	POST,
	DELETE
};

typedef struct s_endPoint {
	std::vector<int>						lmethod;
	std::string 							lroot;
	std::vector<std::string>				lindex;
	std::map<std::string, std::string>		lcgi;
	std::string 							lredirect;
	bool			 						lautoindex;
}											t_endPoint;

class	ConfigFile {
	private:
		int									listen_;
		std::string							serverName_;
		std::string							root_;
		std::vector<std::string>			indexFile_;
		std::map<int, std::string>			errorFile_;
		std::map<std::string, t_endPoint>	location_;
		unsigned long						clientMaxBodySize_;

	public:
		ConfigFile();
		~ConfigFile();

		/* setters */
		void								setListenPort(int port);
		void								setServerName(std::string host);
		void								setRoot(std::string endPoint, std::string path);
		void								setIndexFile(std::string file, std::string path);
		void								setErrorFile(int statusCode, std::string path);
		void								setMethod(std::string endPoint, int	method);
		void								setCGI(std::string endPoint, std::string lang, std::string path);
		void								setRedirect(std::string endPoint, std::string redir);
		void								setAutoIndex(std::string endPoint, bool opt);
		void								setLocation(std::string endPoint);
		void								setClientMaxBodySize(unsigned long sizeMax);

		/* getters */
		const int							&getListenPort(void) const;
		const std::string					&getServerName(void) const;
		const std::string					&getRoot(std::string endPoint) const;
		const std::vector<std::string>		&getIndexFile(std::string endPoint) const;
		const std::map<int, std::string>	&getErrorFile(void) const;
		const std::string					&getScriptCGI(std::string endPoint, std::string type) const;
		const t_endPoint					&getLocation(std::string endPoint) const;
		const std::string					&getEndPoint(std::string name) const;
		const unsigned long					&getClientMaxBodySize(void) const;

		/* print to debug */
		void	debugConfigFile(void);
};





#endif // CONFIGFILE_HPP