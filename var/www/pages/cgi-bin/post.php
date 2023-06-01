<?php
header_remove("X-Powered-By");
ini_set('expose_php', 'off'); // Disable PHP version information in headers
ini_set('default_mimetype', ''); // Disable automatic "Content-type" header generation


if ($_SERVER['REQUEST_METHOD'] === 'POST') {
$requestBody = file_get_contents("php://input");

	if (empty($requestBody)) {
		$requestBody = "Empty";
	}

	$dataEntry = $requestBody . PHP_EOL;
	$directory = dirname(dirname(__FILE__)) . '/' . 'documents' . '/' . 'kanydb';

	if (file_put_contents($directory, $dataEntry, FILE_APPEND) !== false) {
		$responseFile = './temp.html';

		if (file_exists($responseFile)) {
			$response = file_get_contents($responseFile);
			$message = "✅ Data saved successfully to ";
			$message .= $directory . "\r\n";
			http_response_code(201);
		} else {
			http_response_code(500);
			$message = "❌ Failed to load response template";
		}
	} else {
		http_response_code(500);
		$message = "❌ Internal error 500";
	}
} else {
	http_response_code(405);
	$message = "❌ Method Not Allowed\r\n";
}

$response = preg_replace('/{{message}}/i', $message, $response);
$date = gmdate('D, d M Y H:i:s T');
$server = getenv('SERVER_NAME');

header("Content-Type: text/html; charset=UTF-8");
header("Content-Length: " . strlen($response));
header("Connection: close");
header("Date: " . $date);
header("Server: " . $server);
echo $response;

?>
