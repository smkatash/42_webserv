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

$status = 201;
if (file_put_contents($directory, $dataEntry, FILE_APPEND) !== false) {
	$responseFile = './temp.html';
	
	if (file_exists($responseFile)) {
		$response = file_get_contents($responseFile);
		$message = "✅ Data saved successfully to ";
		$message .=  $directory . "\r\n";
	} else {
		$status = 500;
		$message = "❌ Failed to load response template";
	}
} else {
	$status = 500;
	$message = "❌ Internal error 500";
	
}
$response = preg_replace('/{{message}}/i', $message, $response);
$date = gmdate('D, d M Y H:i:s T');

$fullResponse = "HTTP/1.1 " . $status . " " . http_response_code_message($status) . "\r\n";
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
		201 => 'Created',
		500 => 'Internal Server Error',
	);
	return isset($messages[$code]) ? $messages[$code] : '';
}
?>
