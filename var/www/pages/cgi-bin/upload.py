#!/usr/bin/env python

import os
import cgi

# Get the uploaded file data from the form
form = cgi.FieldStorage()
file_item = form['uploaded_file']

if file_item.filename:
	# Get the current working directory
	current_directory = os.getcwd()

	# Specify the destination subdirectory
	destination_subdirectory = '/uploads/'

	# Build the destination directory path
	destination_directory = os.path.join(current_directory, destination_subdirectory)

	# Get the destination file path from the environment variable
	destination_file_path = os.environ.get('UPLOADED_FILE_PATH')

	# Move the uploaded file to the destination directory
	try:
		os.rename(file_item.file.name, os.path.join(destination_directory, destination_file_path))
		# Set the response code and message
		response_code = 200
		response_message = "File uploaded and saved successfully."
	except OSError:
		# Set the response code and message
		response_code = 500
		response_message = "Failed to move the uploaded file."
else:
	# Set the response code and message
	response_code = 400
	response_message = "No file uploaded."

# Print the HTTP response headers
print("Content-Type: text/plain")
print("Status: " + str(response_code) + " " + http_response_code_message(response_code))
print()

# Print the response message
print(response_message)


# Helper function to get the HTTP response code message
def http_response_code_message(code):
	messages = {
		200: 'OK',
		400: 'Bad Request',
		500: 'Internal Server Error',
		# Add more codes and messages as needed
	}
	return messages.get(code, '')
