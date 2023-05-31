<?php
header_remove("X-Powered-By");
ini_set('expose_php', 'off'); // Disable PHP version information in headers
ini_set('default_mimetype', '');

$database = dirname(dirname(__FILE__)). '/' . 'documents' . '/' . 'kanydb';
$responseFile = './temp.html';

$file_contents = file_get_contents($database);

// split the contents into lines
$lines = explode("\n", $file_contents);

// loop through each line and check if it matches the query
$match = false;
if (isset($_GET['name']))
{
	foreach ($lines as $key => $line)
	{
		parse_str($line, $values);

		if (isset($values['name']) && isset($values['order']) && isset($values['email'])
		&& $values['name'] == $_GET['name'])
		{
			unset($lines[$key]);

			$file_contents = implode("\n", $lines);
			file_put_contents($database, $file_contents);
			if (file_exists($responseFile)) {
				http_response_code(204);
				$response = file_get_contents($responseFile);
				$message = "Profile for " . $values['name'] . PHP_EOL;
				$message =. "✅ Entry deleted successfully\r\n";
			} else {
				http_response_code(500);
				$message = "❌ Failed to load response template\r\n";
			}
			
			$match = true;
			// stop looping through the lines since we found a match
			break;
		}
	}
}

if (!$match)
{
	http_response_code(404);
	$message = "❌ No match found\r\n";
}

$response = preg_replace('/{{message}}/i', $message, $response);
$date = gmdate('D, d M Y H:i:s T');

$fullResponse = "HTTP/1.1 " . http_response_code() . " " . http_response_code_message(http_response_code()) . "\r\n";
$fullResponse .= "Content-Type: text/html; charset=UTF-8\r\n";
$fullResponse .= "Content-Length: " . strlen($response) . "\r\n";
$fullResponse .= "Connection: "  . "close\r\n";
$fullResponse .= "Date: " . $date . "\r\n";
$fullResponse .= "Server: "  . getenv('SERVER_NAME') . "\r\n";
$fullResponse .= "\r\n";
$fullResponse .= $response;

echo $fullResponse;

function http_response_code_message($code) {
	$messages = array(
		204 => 'No Content',
		404 => 'Not Found',
		500 => 'Internal Server Error',
	);
	return isset($messages[$code]) ? $messages[$code] : '';
}

?>
