#! /usr/bin/env perl
# Prints "1"if the "main" jar file is signed, "0" otherwise.
$outlines = `jarsigner -verify devise_js.jar`;
if ($outlines =~ /jar verified/) {
	print "1\n";
} else {
	print "0\n";
}
