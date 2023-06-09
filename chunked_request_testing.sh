#!/bin/bash

# File path containing data
file_path="large.jpg"

# Curl command
curl_command="curl --location 'localhost:8081/upload' --header 'Content-Type: multipart/form-data' -H 'Transfer-Encoding: chunked' -F file=@$file_path"

# Execute the curl command
eval "$curl_command"
