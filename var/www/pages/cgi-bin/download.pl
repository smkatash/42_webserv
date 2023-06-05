
use strict;
use warnings;
use CGI qw(:standard);
use File::Basename;

print "I am inside"

if ($ENV{'REQUEST_METHOD'} eq 'GET') {
    my $query_string = $ENV{'QUERY_STRING'};

    if ($query_string) {
        my %query_params = parse_query_string($query_string);
        my $filename = $query_params{'file'};

        if ($filename) {
			my $up_dir = File::Spec->updir();
			my $documents_dir = File::Spec->catdir($up_dir, "documents"); 
            my $file = "$documents_dir/$filename";

            if (-e $file) {
                open(my $fh, '<', $file) or die "Cannot open file: $!";
                binmode($fh);

                print "Content-Type: application/octet-stream\r\n";
                print "Content-Disposition: attachment; filename=" . basename($file) . "\r\n";
                print "Content-Length: " . (-s $fh) . "\r\n\r\n";

                while (my $buffer = <$fh>) {
                    print $buffer;
                }

                close($fh);
            } else {
                print "Content-Type: text/plain\r\n\r\n";
                print "File not found.";
            }
        } else {
            print "Content-Type: text/plain\r\n\r\n";
            print "File name not provided.";
        }
    } else {
        print "Content-Type: text/plain\r\n\r\n";
        print "Invalid query string.";
    }
} else {
    print "Content-Type: text/plain\r\n\r\n";
    print "Invalid request method.";
}

sub parse_query_string {
    my ($query_string) = @_;
    my %params;

    foreach my $param (split(/&/, $query_string)) {
        my ($name, $value) = split(/=/, $param);
        $value = '' unless defined $value;
        $value =~ tr/+/ /;
        $value =~ s/%([0-9a-fA-F]{2})/chr(hex($1))/eg;
        $params{$name} = $value;
    }

    return %params;
}