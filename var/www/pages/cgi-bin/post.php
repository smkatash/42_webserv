<?php
header_remove("X-Powered-By");
ini_set('expose_php', 'off'); // Disable PHP version information in headers
ini_set('default_mimetype', ''); // Disable automatic "Content-type" header generation

$requestBody = file_get_contents("php://input");

if (empty($requestBody)) {
	$requestBody = "Empty";
}

$dataEntry = $requestBody . PHP_EOL;
$directory = dirname(dirname(__FILE__)). '/' . 'documents' . '/' . 'kanydb';

$response = "<html>\n";
$response .= "<body>\n";

if (file_put_contents($directory, $dataEntry, FILE_APPEND) !== false) {
	http_response_code(200);
	$response .= "<h1> Data has been saved to file: ". $directory ." </h1>\n";
} else {
	http_response_code(500);
	$response .= "<h1> Error occurred while saving the data: ". $directory ." </h1>\n";
}

$response .= "</body>\n";
$response .= "</html>\n";

$fullResponse = "HTTP/1.1 " . http_response_code() . " " . http_response_code_message(http_response_code()) . "\r\n";
$fullResponse .= "Content-Type: text/html; charset=UTF-8\r\n";
$fullResponse .= "Content-Length: " . strlen($response) . "\r\n";
$fullResponse .= "\r\n";
$fullResponse .= $response;

echo $fullResponse;

function http_response_code_message($code) {
	$messages = array(
		200 => 'OK',
		500 => 'Internal Server Error',
	);
	return isset($messages[$code]) ? $messages[$code] : '';
}
?>
