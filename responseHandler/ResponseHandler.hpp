#ifndef RESPONSEHANDLER_HPP
#define RESPONSEHANDLER_HPP

#include <iostream>
#include "Response.hpp"

class ResponseHandler {
private:
	Response res_;
	void fillResponseLine(response_line* rline);
	void fillGeneralHeader(general_header* gheader);
	void fillResponseHeader(response_header* rheader);
	void fillEntityHeader(entity_header* eheader);

	std::string appendResponseLine();
	std::string appendGeneralHeader();
	std::string appendResponseHeader();
	std::string appendEntityHeader();
public:
	ResponseHandler();
	~ResponseHandler();

	void		fill(int identifier, void *header);
	std::string	getResponse();
};

#endif
