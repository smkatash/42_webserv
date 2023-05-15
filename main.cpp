#include "Parser.hpp"
#include "RequestParser.hpp"
#include "ResponseHandler.hpp"
#include <fstream>

std::string base64Encode(const std::vector<char>& binaryData) {
    std::string base64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string base64Encoded;

    size_t remainingBytes = binaryData.size() % 3;
    size_t i = 0;

    while (i + 2 < binaryData.size()) {
        unsigned int triplet = (static_cast<unsigned char>(binaryData[i]) << 16) |
                               (static_cast<unsigned char>(binaryData[i + 1]) << 8) |
                               static_cast<unsigned char>(binaryData[i + 2]);

        for (int j = 0; j < 4; j++) {
            int index = (triplet >> (18 - j * 6)) & 0x3F;
            base64Encoded += base64Chars[index];
        }

        i += 3;
    }

    // Handle remaining bytes (if any)
    if (remainingBytes == 1) {
        unsigned int value = static_cast<unsigned char>(binaryData[i]);

        int index1 = (value & 0xFC) >> 2;
        int index2 = (value & 0x03) << 4;

        base64Encoded += base64Chars[index1];
        base64Encoded += base64Chars[index2];
        base64Encoded += "==";
    } else if (remainingBytes == 2) {
        unsigned int value1 = static_cast<unsigned char>(binaryData[i]);
        unsigned int value2 = static_cast<unsigned char>(binaryData[i + 1]);

        int index1 = (value1 & 0xFC) >> 2;
        int index2 = ((value1 & 0x03) << 4) | ((value2 & 0xF0) >> 4);
        int index3 = (value2 & 0x0F) << 2;

        base64Encoded += base64Chars[index1];
        base64Encoded += base64Chars[index2];
        base64Encoded += base64Chars[index3];
        base64Encoded += "=";
    }

    return base64Encoded;
}

std::vector<char> readBinaryFile(std::string filePath) {
    std::ifstream file(filePath, std::ios::binary);
    
    if (!file) {
        // Handle file open error
        // For example, throw an exception or return an empty vector
        return std::vector<char>();
    }
    
    // Determine the size of the file
    file.seekg(0, std::ios::end);
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // Create a vector with the appropriate size
    std::vector<char> buffer(fileSize);
    
    // Read the file content into the vector
    if (file.read(buffer.data(), fileSize))
        return buffer;
    
    // Handle file read error
    // For example, throw an exception or return an empty vector
    return std::vector<char>();
}

std::string insertBinaryIntoRequest(const std::string& request, const std::string& binaryData) {
    std::string placeholder = "binary";
    size_t pos = request.find(placeholder);
    if (pos != std::string::npos) {
        std::string updatedRequest = request;
        updatedRequest.replace(pos, placeholder.length(), binaryData);
        return updatedRequest;
    }
    return request;
}

int main(int argc, char **argv) {
	std::ifstream	file;

	if (argc < 2)
		file.open("./config/default.conf");
	else if (argc == 2) {
		const std::string ext = ".conf";
		std::string input = argv[1];
		if (input.find_last_of(".") == std::string::npos || \
			input.substr(input.find_last_of(".")) != ext) {
			std::cerr << "error: file must have .conf extension" << std::endl;
			exit(EXIT_FAILURE);
		}
		file.open(argv[1]);
	}
	else {
		std::cerr << "usage: ./webserv config_file\n";
		exit(EXIT_FAILURE);
	}

	// std::string filePath = "./notes.txt";
	// std::vector<char> fileData = readBinaryFile(filePath); // Path to the file you want to upload
	std::string requestUpload =
"DELETE /deleteme.txt HTTP/1.1\n";

	std::vector<char> bin = readBinaryFile("./img.jpg");
	if (!bin.empty()) {
		std::string base64Encoded = base64Encode(bin);
		requestUpload = insertBinaryIntoRequest(requestUpload, base64Encoded);
	}

	try {


		Parser	confParser(file);
		confParser.setConfigFile();
		RequestParser req(requestUpload);
		//req.debug();
		ResponseHandler resp(req.getRequest(), confParser.getConfigFile());
		resp.del();
		std::cout << resp.generateResponse() << std::endl;


	} catch (std::invalid_argument& e) {
		std::cerr << e.what() << '\n';
		exit(EXIT_FAILURE);
	} catch (const std::out_of_range & e) {
		std::cerr << e.what() << "\n";
		exit(EXIT_FAILURE);
	} catch(const std::runtime_error& e) {
		std::cout << e.what() << std::endl;
	}

}