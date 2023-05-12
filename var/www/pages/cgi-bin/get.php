<?php
// Read the query parameters from the query string
parse_str($_SERVER['QUERY_STRING'], $queryParams);

// Extract the 'name' parameter from the query string
echo("Customer : " . $queryParams['name'] . "\n");
echo("Placed order: " . $queryParams['order'] . "\n");
echo("Contact email: " . $queryParams['email'] . "\n");

?>


