<?php
header_remove("X-Powered-By");
ini_set('expose_php', 'off'); // Disable PHP version information in headers
ini_set('default_mimetype', ''); // Disable automatic "Content-type" header generation

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
	$uploadedFilePath = getenv('UPLOADED_FILE_PATH');
	$destinationDirectory = dirname(dirname(__FILE__)). '/' . 'documents' . '/';

	if ($uploadedFilePath && file_exists($uploadedFilePath)) {
		$fileName = getenv('FILE_NAME');

		$destinationFilePath = $destinationDirectory . $fileName;

		if (rename($uploadedFilePath, $destinationFilePath)) {
			http_response_code(200);
			$responseFile = './temp.html';
	
			if (file_exists($responseFile)) {
				$response = file_get_contents($responseFile);
				$message = "✅ File uploaded and saved successfully to ";
				$message .=  $destinationFilePath . "\n";
			} else {
					$message = "❌ Failed to load response template";
			}

		} else {
			http_response_code(500);
			$message = "❌ Internal error 500";
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
		200 => 'OK',
		400 => 'Bad Request',
		405 => 'Method Not Allowed',
		500 => 'Internal Server Error',
	);
	return isset($messages[$code]) ? $messages[$code] : '';
}
?>
