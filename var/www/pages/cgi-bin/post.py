import os

def http_response_code_message(code):
	messages = {
		200: "OK",
		500: "Internal Server Error",
	}
	return messages.get(code, "")

# Remove "X-Powered-By" header
os.environ["TZ"] = "UTC"
os.environ.pop("TZ", None)

# Disable PHP version information in headers
os.environ["expose_php"] = "off"

# Disable automatic "Content-type" header generation
os.environ["default_mimetype"] = ""

# Read request body from stdin
request_body = input()

if not request_body:
	request_body = "Empty"

data_entry = request_body + "\n"
directory = os.path.join(os.path.dirname(os.path.dirname(__file__)), "documents", "kanydb")

if os.path.isfile(directory):
	with open(directory, "a") as file:
		file.write(data_entry)
	response_file = "./temp.html"
	
	if os.path.isfile(response_file):
		with open(response_file, "r") as file:
			response = file.read()
		message = "✅ Data saved successfully to " + directory + "\n"
	else:
		message = "❌ Failed to load response template"
else:
	response_code = 500
	message = "❌ Internal error 500"

response = response.replace("{{message}}", message)

full_response = "HTTP/1.1 " + str(response_code) + " " + http_response_code_message(response_code) + "\r\n"
full_response += "Content-Type: text/html; charset=UTF-8\r\n"
full_response += "Content-Length: " + str(len(response)) + "\r\n"
full_response += "Server: " + os.environ.get("SERVER_NAME", "") + "\r\n"
full_response += "Connection: close\r\n"
full_response += "\r\n"
full_response += response

print(full_response)

