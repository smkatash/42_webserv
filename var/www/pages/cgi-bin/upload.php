<?php

// Check if the request method is POST
$uploadedFilePath = getenv('UPLOADED_FILE_PATH');
$destinationDirectory = '/Users/kanykei/Desktop/42_webserv/';  // Specify the destination directory

if ($uploadedFilePath && file_exists($uploadedFilePath)) {
    // Specify the destination file name
    $destinationFileName = 'img2.jpg';

    // Build the destination file path
    $destinationFilePath = $destinationDirectory . $destinationFileName;

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
