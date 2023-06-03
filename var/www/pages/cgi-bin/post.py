import os
import sys
import datetime


def http_response_code_message(code):
	messages = {
		200: 'OK',
		405: 'Method Not Allowed',
		500: 'Internal Server Error',
	}
	return messages.get(code, '')

# Disable PHP version information in headers
os.environ['expose_php'] = 'off'
# Disable automatic "Content-type" header generation
os.environ['default_mimetype'] = ''

if os.getenv('REQUEST_METHOD') == 'POST':
	requestBody = sys.stdin.read()

	if not requestBody:
		requestBody = "Empty"

	session = os.getenv('SESSION')
	dataEntry = session + requestBody + '\n'
	directory = os.path.join(os.path.dirname(os.path.dirname(__file__)), 'documents', 'kanydb')
	current_directory = os.path.dirname(os.path.abspath(__file__))
	responseFile = os.path.join(current_directory, 'temp.html')
	if os.path.exists(responseFile):
		with open(responseFile, 'r') as file:
			response = file.read()
	else:
		response = "Failed to load response template"
		httpStatusCode = 500
	try:
		with open(directory, 'a') as file:
			file.write(dataEntry)
			message = "Thanks for your order!"
			httpStatusCode = 200
			response = response.replace('{{message}}', message)
	except:
		response = "Internal error 500"
		httpStatusCode = 500


date = datetime.datetime.utcnow().strftime('%a, %d %b %Y %H:%M:%S GMT')
server = os.getenv('SERVER_NAME')

print("HTTP/1.1 " + str(httpStatusCode) + " " + http_response_code_message(httpStatusCode))
print("Content-Type: text/html; charset=UTF-8")
print("Content-Length: " + str(len(response)))
print("Connection: close")
print("Date: " + date)
print("Server: " + server)
print
print(response)
