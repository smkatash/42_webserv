<?php

$method = getenv('REQUEST_METHOD');
$uploadedFilePath = getenv('UPLOADED_FILE_PATH');
$uploadedFilePath = '/Users/ktashbae/Desktop/webserv/42_webserv' . '/' . $uploadedFilePath;
echo $uploadedFilePath . "\n";
echo $method . "\n";


if ($uploadedFilePath && is_uploaded_file($uploadedFilePath)) {
    $fileName = basename($uploadedFilePath);
    $destinationPath = '/Users/ktashbae/Desktop/webserv/42_webserv' . '/' . $fileName;

    if (move_uploaded_file($uploadedFilePath, $destinationPath)) {
        echo "File uploaded successfully to: " . $destinationPath;
    } else {
        echo "Failed to move the uploaded file.";
    }
} else {
    echo "No file was uploaded.";
}

?>
