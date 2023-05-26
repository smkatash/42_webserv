<?php
$database = dirname(__FILE__) . "/" . 'kanydb';

// read the contents of the file
$file_contents = file_get_contents($database);

// split the contents into lines
$lines = explode("\n", $file_contents);

// loop through each line and check if it matches the query
$match = false;
if (isset($_GET['name']))
{
	foreach ($lines as $key => $line)
	{
		// split the data into key-value pairs
		parse_str($line, $values);

		if (isset($values['name']) && isset($values['order']) && isset($values['email'])
		&& $values['name'] == $_GET['name'])
		{
			// remove the line from the array
			unset($lines[$key]);

			// join the array back into a string
			$file_contents = implode("\n", $lines);
			
			// write the string back to the file
			file_put_contents($database, $file_contents);
			// create the HTML file content with the name, order, and email
			$html_content = "<html>\n";
			$html_content .= "<head><title>Profile for " . $values['name'] . "</title></head>\n";
			$html_content .= "<body>\n";
			$html_content .= "<h1> Entry deleted successfully</h1>\n";
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
