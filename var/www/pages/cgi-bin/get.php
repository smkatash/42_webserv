<?php
header_remove("X-Powered-By");
ini_set('expose_php', 'off'); // Disable PHP version information in headers
ini_set('default_mimetype', '');  // Disable automatic "Content-type" header generation


$database = dirname(dirname(__FILE__)). '/' . 'documents' . '/' . 'kanydb';

// read the contents of the file
$file_contents = file_get_contents($database);

// split the contents into lines
$lines = explode("\n", $file_contents);

// loop through each line and check if it matches the query
$match = false;
if (isset($_GET['name']))
{
	foreach ($lines as $line)
	{
		// split the line into timestamp and data
		$parts = explode(":", $line, 2);
		$data = $parts[1];
		
		// split the data into key-value pairs
		parse_str($data, $values);

		if (isset($values['name']) && isset($values['age']) && isset($values['email'])
		&& $values['name'] == $_GET['name'])
		{
			// create the HTML file content with the name, age, and email
			$html_content = "<html>\n";
			$html_content .= "<head><title>Profile for " . $values['name'] . "</title></head>\n";
			$html_content .= "<body>\n";
			$html_content .= "<h1>Profile for " . $values['name'] . "</h1>\n";
			$html_content .= "<p>Age: " . $values['age'] . "</p>\n";
			$html_content .= "<p>Email: " . $values['email'] . "</p>\n";
			$html_content .= "</body>\n";
			$html_content .= "</html>\n";
	
			// set the Content-Type header to indicate that this is an HTML file
			header('Content-Type: text/html');
	
			// output the file content
			echo $html_content;
	
			$match = true;
			// stop looping through the lines since we found a match
			break;
		}
	}
}

if (!$match)
{
	$html_content = "<html>\n";
	$html_content .= "<head><title>No match found</title></head>\n";
	$html_content .= "<body>\n";
	$html_content .= "<h1>No match found</h1>\n";
	$html_content .= "</body>\n";
	$html_content .= "</html>\n";

	// set the Content-Type header to indicate that this is an HTML file
	header('Content-Type: text/html');

	// output the file content
	echo $html_content;
}
?>
