
 if (getenv("CONTENT_TYPE") && std::string(getenv("CONTENT_TYPE")) == "multipart/form-data") {
        // Get the content length
        int contentLength = 0;
        if (getenv("CONTENT_LENGTH"))
            contentLength = std::stoi(getenv("CONTENT_LENGTH"));

        // Read the file data from stdin and write it to a temporary file
        std::ofstream tempFile("uploaded_file.tmp", std::ios::binary);
        char buffer[4096];
        int bytesRead = 0;
        while (bytesRead < contentLength) {
            int bytesToRead = std::min(contentLength - bytesRead, sizeof(buffer));
            std::cin.read(buffer, bytesToRead);
            tempFile.write(buffer, std::cin.gcount());
            bytesRead += std::cin.gcount();
        }
        tempFile.close();

        // Pass the temporary file path to the CGI script as an environment variable
        setenv("UPLOADED_FILE_PATH", "uploaded_file.tmp", 1);
    }