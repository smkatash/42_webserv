#include <iostream>
#include "RequestParser.hpp"

std::string input = 
"PUT /index.html HTTP/1.1\n\
Host: www.example.com\n\
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:88.0) Gecko/20100101 Firefox/88.0\n\
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\n\
Accept-Language: en-US,en;q=0.5\n\
Accept-Encoding: gzip, deflate, br\n\
Connection: keep-alive\n\
Referer: https://www.google.com/\n\
If-Modified-Since: Sat, 21 Jan 2023 12:00:00 GMT\n\
If-None-Match: \"etag12345\"\n\
Cookie: something\n\
Cache-Control: max-age=0\n";

int main() {

	try {
		RequestParser	req(input);
		req.debug();
	} catch(const std::runtime_error& e) {
		std::cout << e.what() << std::endl;
	}
} 