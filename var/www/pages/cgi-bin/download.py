#!/usr/bin/env python
import os
import sys
import cgi

# Remove PHP related headers
sys.stdout.write("Content-Type: text/html; charset=UTF-8\n")
sys.stdout.write("X-Powered-By: \n")
sys.stdout.write("\n")

if os.environ['REQUEST_METHOD'] == 'GET':
	destinationDirectory = os.path.dirname(os.path.dirname(os.path.abspath(__file__))) + '/' + 'documents' + '/'
	file = cgi.FieldStorage().getvalue('file', '')
	FilePath = destinationDirectory + file

	if os.path.exists(FilePath):
		# Set appropriate headers for file download
		with open("./download_link.html", 'r') as f:
			response = f.read()
		sys.stdout.write("HTTP/1.1 200 OK\n")
	else:
		sys.stdout.write("HTTP/1.1 404 Not Found\n")
		response = "❌ Not Found 404\n"
else:
	sys.stdout.write("HTTP/1.1 405 Method Not Allowed\n")
	response = "❌ Method Not Allowed\r\n"

date = os.popen('date -R').read().strip()
server = os.environ['SERVER_NAME']

response = response.replace('{{location}}', file)

sys.stdout.write("Content-Length: {}\n".format(len(response)))
sys.stdout.write("Date: {}\n".format(date))
sys.stdout.write("Server: {}\n".format(server))
sys.stdout.write("\n")
sys.stdout.write(response)