<?php
header_remove("X-Powered-By");
ini_set('expose_php', 'off'); // Disable PHP version information in headers

if ($_SERVER['REQUEST_METHOD'] === 'GET') {
	$destinationDirectory = dirname(dirname(__FILE__)). '/' . 'documents' . '/';
	$file = isset($_GET['file']) ? $_GET['file'] : '';
	$FilePath = $destinationDirectory . $file;

	if (file_exists($FilePath)) {
		// Set appropriate headers for file download
		$response = file_get_contents("./download_link.html");
		http_response_code(200);
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

$response = preg_replace('/{{location}}/i', $file, $response);

header("Content-Type: text/html; charset=UTF-8");
header("Content-Length: " . strlen($response));
header("Date: " . $date);
header("Server: " . $server);
echo $response;

?>
