<?php

// Check if the request method is POST
$uploadedFilePath = getenv('UPLOADED_FILE_PATH');
$destinationDirectory = '/Users/kanykei/Desktop/42_webserv/cgi/';  // Specify the destination directory

if ($uploadedFilePath && file_exists($uploadedFilePath)) {
    // Specify the destination file name
    $fileName = getenv('FILE_NAME');

    // Build the destination file path
    $destinationFilePath = $destinationDirectory . $fileName;

    // Move the uploaded file to the destination directory
    if (rename($uploadedFilePath, $destinationFilePath)) {
        // Output a success message
        echo "File uploaded and saved successfully.";
    } else {
        echo "Failed to move the uploaded file.";
    }
} else {
    echo "No file uploaded.";
}
?>
