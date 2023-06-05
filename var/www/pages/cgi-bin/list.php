<?php
header_remove("X-Powered-By");
ini_set('expose_php', 'off'); // Disable PHP version information in headers
ini_set('default_mimetype', ''); // Disable automatic "Content-type" header generation

if (getenv('REQUEST_METHOD') == 'POST') {
	$database = dirname(dirname(__FILE__)) . '/' . 'data' . '/' . 'kanydb';
	$session = getenv('SESSION');
	if (file_exists($database) && $session) {
		parse_str($session, $params);
		$session_id = $params['session_id'];
	
		$fileContents = file_get_contents($database);
		$listItems = '';
		if ($fileContents) {
			$lines = explode("\n", $fileContents);
	
			$orderValues = array();
	
			foreach ($lines as $line) {
				if (strpos($line, $session_id) !== false) {
					parse_str($line, $params);
					$orderValue = isset($params['order']) ? $params['order'] : 'undefined';
					$quantityValue = isset($params['quantity']) ? $params['quantity'] : 'undefined'; // Retrieve the 'quantity' parameter
					$orderValues[] = $orderValue . ': ' . $quantityValue;
				}
			}
	
			foreach ($orderValues as $orderValue) {
				$listItems .= '<li>' . $orderValue . '</li>';
			}
		}
		$responseFile = './login.html';

		if (file_exists($responseFile)) {
			$response = file_get_contents($responseFile);
			if ($response) {
				http_response_code(200);
				$response = preg_replace('/{{orders}}/i', $listItems, $response);
			}
			else {
				http_response_code(500);
				$response = "❌ Internal error 500";

			}
		} else {
			http_response_code(500);
			$response = "❌ Internal error 500";
		}

	} else {
			http_response_code(500);
			$response = "❌ Internal error 500";
	}
} else {
	http_response_code(405);
	$response = "❌ Method Not Allowed\r\n";
}

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
	);
	return isset($messages[$code]) ? $messages[$code] : '';
}

?>
