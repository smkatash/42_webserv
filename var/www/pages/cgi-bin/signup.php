<?php
header_remove("X-Powered-By");
ini_set('expose_php', 'off'); // Disable PHP version information in headers
ini_set('default_mimetype', ''); // Disable automatic "Content-type" header generation


if ($_SERVER['REQUEST_METHOD'] === 'POST') {
$requestBody = file_get_contents("php://input");

	if (empty($requestBody)) {
		$requestBody = "Empty";
	}
	parse_str($requestBody, $params);
	$username = $params['username'];
	$password = $params['password'];
	$dataEntry = $username . ':' . $password . PHP_EOL;
	$directory = dirname(dirname(__FILE__)) . '/' . 'etc' . '/' . '.htpasswd';


	if (file_put_contents($directory, $dataEntry, FILE_APPEND) !== false) {
		$responseFile = './after_sign_up.html';

		if (file_exists($responseFile)) {
			$response = file_get_contents($responseFile);
			$message = "✅ Successfull Sign-up!";
			http_response_code(200);
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

header("HTTP/1.1" . http_response_code() . " " . http_response_code_message(http_response_code()));
header("Content-Type: text/html; charset=UTF-8");
header("Content-Length: " . strlen($response));
header("Connection: close");
header("Date: " . $date);
header("Server: " . $server);
echo $response;

function http_response_code_message($code) {
	$messages = array(
		200 => 'OK',
		500 => 'Internal Server Error',
		405 => 'Method Not Allowed'
	);
	return isset($messages[$code]) ? $messages[$code] : '';
}

?>
