#!/bin/bash

# File path containing data
file_path="lorem_ipsum.txt"

# Curl command
curl_command="curl --location 'localhost:8081/chunked_request_testing' --header 'Content-Type: text/plain' -H 'Transfer-Encoding: chunked' --data-binary @$file_path"

# Execute the curl command
eval "$curl_command"
