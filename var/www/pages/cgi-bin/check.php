<?php
// Read the query parameters from the query string

$requestBody= file_get_contents("php://stdin");
if (empty($requestBody)) {
    $requestBody = "I am empty";
}
echo "Data has been saved to file: " . $requestBody;

?>
