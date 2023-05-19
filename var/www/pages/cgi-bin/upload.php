<?php
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $uploadedFilePath = getenv('UPLOADED_FILE_PATH');
    $destinationDirectory = dirname(__FILE__) . '/';

    if ($uploadedFilePath && file_exists($uploadedFilePath)) {
        $fileName = getenv('FILE_NAME');

        $destinationFilePath = $destinationDirectory . $fileName;

        if (rename($uploadedFilePath, $destinationFilePath)) {
            $responseCode = 200;
            $responseMessage = "File uploaded and saved successfully.";
        } else {
            $responseCode = 500;
            $responseMessage = "Failed to move the uploaded file.";
        }
    } else {
        $responseCode = 400;
        $responseMessage = "No file uploaded.";
    }
} else {
    $responseCode = 405;
    $responseMessage = "Method Not Allowed";
}

$fullResponse = "HTTP/1.1 " . $responseCode . " " . http_response_code_message($responseCode) . "\r\n";
$fullResponse .= "Content-Type: text/plain\r\n";
$fullResponse .= "Content-Length: " . strlen($responseMessage) . "\r\n";
$fullResponse .= "\r\n";
$fullResponse .= $responseMessage;

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
