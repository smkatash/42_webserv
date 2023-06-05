#!/usr/bin/perl

use strict;
use warnings;
use File::Basename;
use CGI qw(:standard);

$ENV{'expose_php'} = 'off';
$ENV{'default_mimetype'} = '';

if ($ENV{'REQUEST_METHOD'} eq 'GET') {
	my $destinationDirectory = dirname(dirname($0)) . '/' . 'documents' . '/';
	my $file = param('file') || '';
	my $FilePath = $destinationDirectory . $file;

	if (-e $FilePath) {
		# Set appropriate headers for file download
		my $date = gmtime();
		my $server = $ENV{'SERVER_NAME'};

		print "HTTP/1.1 200 OK\n";
		print "Content-Type: application/octet-stream\n";
		print "Content-Disposition: attachment; filename=\"" . basename($FilePath) . "\"\n";
		print "Content-Length: " . -s $FilePath . "\n";
		print "Connection: close\n";
		print "Date: $date\n";
		print "Server: $server\n\n";

		# Read and output the file content
		open(my $fh, '<', $FilePath) or die "Unable to open file: $!";
		binmode($fh);
		while (my $chunk = <$fh>) {
			print $chunk;
		}
		close($fh);
		exit;
	} else {
		print "HTTP/1.1 404 Not Found\n";
		print "Content-Type: text/html; charset=UTF-8\n";
		print "Connection: close\n";
		print "Server: $ENV{'SERVER_NAME'}\n\n";
		print "❌ Not Found 404";
		exit;
	}
} else {
	print "HTTP/1.1 405 Method Not Allowed\n";
	print "Content-Type: text/html; charset=UTF-8\n";
	print "Connection: close\n";
	print "Server: $ENV{'SERVER_NAME'}\n\n";
	print "❌ Method Not Allowed";
	exit;
}
