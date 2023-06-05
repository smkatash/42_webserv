<?php
header_remove("X-Powered-By");
ini_set('expose_php', 'off'); // Disable PHP version information in headers
ini_set('default_mimetype', ''); // Disable automatic "Content-type" header generation

if ($_SERVER['REQUEST_METHOD'] === 'GET') {
	$destinationDirectory = dirname(dirname(__FILE__)). '/' . 'documents' . '/';
	$file = isset($_GET['file']) ? $_GET['file'] : '';
	$FilePath = $destinationDirectory . $file;

	if (file_exists($FilePath)) {
		// Set appropriate headers for file download
		$date = gmdate('D, d M Y H:i:s T');
		$server = getenv('SERVER_NAME');
		
		http_response_code(200);
		header("HTTP/1.1" . http_response_code() . " " . http_response_code_message(http_response_code()));
		header("Content-Type: application/octet-stream");
		header("Content-Disposition: attachment; filename=\"" . basename($FilePath) . "\"");
		header("Content-Length: " . filesize($FilePath));

		readfile($FilePath);
		exit();
	} else {
		http_response_code(404);
		$response = "❌ Not Found 404";
	}
} else {
	http_response_code(405);
	$response = "❌ Method Not Allowed\r\n";
}
$date = gmdate('D, d M Y H:i:s T');
$server = getenv('SERVER_NAME');

header("HTTP/1.1" . http_response_code() . " " . http_response_code_message(http_response_code()));
header("Content-Type: text/html; charset=UTF-8");
header("Connection: close");
header("Date: " . $date);
header("Server: " . $server);
echo $response;

function http_response_code_message($code) {
	$messages = array(
		200 => 'OK',
		404 => 'Not Found',
		500 => 'Method Not Allowed',
	);
	return isset($messages[$code]) ? $messages[$code] : '';
}

?>
