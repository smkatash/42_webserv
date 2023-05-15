<?php
$requestBody= file_get_contents("php://stdin");

if (empty($requestBody)) {
    $requestBody = "Empty";
}
echo "Request body is " . $requestBody . "\n";

$index = time();
// Prepare the data entry
$dataEntry = $index . ':' . $requestBody . PHP_EOL;
$directory = dirname(__FILE__) . "/" . 'kanydb';
file_put_contents($directory, $dataEntry, FILE_APPEND);

// Display a success message
echo "Data has been saved to file: " . $directory;
?>