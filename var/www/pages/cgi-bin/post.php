<?php
$requestBody= file_get_contents("php://stdin");

if (empty($requestBody)) {
    $requestBody = "I am empty";
}
echo "Request body is " . $requestBody . "\n";


// Generate a unique file name
$filename = 'kanydb';

// Specify the directory where you want to save the file
$directory = dirname(__FILE__) . "/";

// Save the request body into the file
file_put_contents($directory . $filename, $requestBody);

// Display a success message
echo "Data has been saved to file: " . $directory . $filename;
?>