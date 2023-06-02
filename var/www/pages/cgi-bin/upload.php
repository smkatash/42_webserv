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
			$responseFile = './temp.html';
			
			if (file_exists($responseFile)) {
				http_response_code(201);
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
$server = getenv('SERVER_NAME');

header("Content-Type: text/html; charset=UTF-8");
header("Content-Length: " . strlen($response));
header("Connection: close");
header("Date: " . $date);
header("Server: " . $server);
echo $response;

?>
