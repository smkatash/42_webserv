<?php
$requestBody = file_get_contents("php://input");

if (empty($requestBody)) {
	$requestBody = "Empty";
}

$dataEntry = $requestBody . PHP_EOL;
$directory = dirname(__FILE__) . "/" . 'kanydb';

$response = '';

if (file_put_contents($directory, $dataEntry, FILE_APPEND) !== false) {
	http_response_code(200);
	$response .= "Data has been saved to file: " . $directory;
} else {
	http_response_code(500);
	$response = "Error occurred while saving the data.";
}

$fullResponse = "HTTP/1.1 " . http_response_code() . " " . http_response_code_message(http_response_code()) . "\r\n";
$fullResponse .= "Content-Type: text/plain\r\n";
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
