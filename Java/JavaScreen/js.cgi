#!/s/std/bin/perl

#  ========================================================================
#  DEVise Data Visualization Software
#  (c) Copyright 2000
#  By the DEVise Development Group
#  Madison, Wisconsin
#  All Rights Reserved.
#  ========================================================================
#
#  Under no circumstances is this software to be copied, distributed,
#  or altered in any way without prior permission from the DEVise
#  Development Group.

#  This is a script to check whether the jspop, jss, and devised are running,
#  and restart them if they're not.

############################################################

#  $Id$

#  $Log$
#  Revision 1.1.2.9  2000/12/22 15:03:45  wenger
#  More minor fixes to the CGI stuff.
#
#  Revision 1.1.2.8  2000/12/21 23:28:27  wenger
#  Got CGI mode working with applet, minor cleanup still needed; more debug
#  output in DEViseCommSocket, jspop, and js.cgi; comments updated for
#  new command format that always includes ID and CGI flag.
#
#  Revision 1.1.2.7  2000/12/19 22:50:30  wenger
#  Fixed CGI code for current command format (always sending ID and CGI flag).
#
#  Revision 1.1.2.6  2000/12/19 18:12:08  xuk
#  Also receive cgiflag from JS. Send msgtype as the first argument to jspop.
#
#  Revision 1.1.2.5  2000/11/22 17:44:40  wenger
#  Finished cleanup of static variables fix; re-changed CGI command code to
#  match the current version of the CGI script.
#
#  Revision 1.1.2.4  2000/11/10 16:19:43  wenger
#  JavaScreen sends message type (API_JAVA_WID) to cgi script.
#
#  Revision 1.1.2.3  2000/11/09 21:43:13  wenger
#  js.cgi now uses CGI library, various other cleanups; changed CGI parameter
#  names slightly; added -cgi argument to JS usage message.
#
#  Revision 1.1.2.2  2000/11/09 16:53:28  xuk
#  Receives cmdport from JavaScreen.
#
#  Revision 1.1.2.1  2000/11/08 22:04:15  wenger
#  First version.
#

############################################################

# Parameters:
#   cmdport=<command port of jspop>
#   msgtype=<type of message -- see API_JAVA, etc. in DEViseGlobals.java>
#   id=<JS ID value>
#   nelem=<number of arguments>
#   size=<total size of arguments, in bytes>
#   len1=<length of argument 1>
#   arg1=<value of argument 1>
#   len2=<length of argument 2>
#   arg2=<value of argument 2>
#      ...

############################################################

use CGI; # qw/:standard :html3/;
use strict;
use Socket;

my $debug = 1;

  if ($debug) {
    chmod 0666, "/tmp/js_cgi_log";
    open(LOG, ">/tmp/js_cgi_log");
    my $date = `date`;
    print LOG "js.cgi run at $date\n";
  }

print "Content-type:java_screen\n\n";

my $tmpbuf;
read(STDIN, $tmpbuf, $ENV{'CONTENT_LENGTH'});
  print LOG "tmpbuf = <$tmpbuf>\n" if $debug;
my $query = new CGI($tmpbuf);


#
# Get the port and message header parameters.
#
my ($port, $msgtype, $id, $cgi, $nelem, $size);

my @params = $query->param;
  print LOG "Parameters: @params\n" if $debug;

if (grep /port/, @params) {
  $port = $query->param('cmdport');
} else {
  $port = 6666;
}
  print LOG "port = $port\n" if $debug;


if (grep /msgtype/, @params) {
  $msgtype = $query->param('msgtype');
    print LOG "msgtype = $msgtype\n" if $debug;
} else {
  dienice("msgtype not specified");
}

if (grep /id/, @params) {
  $id = $query->param('id');
    print LOG "id = $id\n" if $debug;
} else {
  dienice("id not specified");
}

$cgi = 1;

if (grep /nelem/, @params) {
  $nelem = $query->param('nelem');
    print LOG "nelem = $nelem\n" if $debug;
} else {
  dienice("nelem not specified");
}

if (grep /size/, @params) {
  $size = $query->param('size');
    print LOG "size = $size\n" if $debug;
} else {
  dienice("size not specified");
}


#
# Set up the socket.
#
my ($remote, $iaddr, $paddr, $proto);
$remote  = 'localhost';
$iaddr   = inet_aton($remote)  || dienice("no host: $remote");
$paddr   = sockaddr_in($port, $iaddr);
$proto   = getprotobyname('tcp');

socket(SOCK, PF_INET, SOCK_STREAM, $proto)  || dienice("socket: $!");
connect(SOCK, $paddr)    || dienice("connect: $!");
  print LOG "connected socket\n" if $debug;


#
# Send the command "header".
#
my ($out, $written);
$out = pack "n5", $msgtype, $id, $cgi, $nelem, $size;
$written = syswrite(SOCK, $out, 10) || dienice("send: $!");
dienice("System write error: $!\n") unless defined $written;
  print LOG "Sent command header to jspop\n" if $debug;


#
# Get and send the command arguments.
#
my ($n);
for ($n = 0; $n < $nelem; $n++) {
    my ($name, $length);

    # Argument length.
    $name = 'len' . $n;
    $length = $query->param($name);
    $out = pack "n", $length;
    $written = syswrite(SOCK, $out, 2) || dienice("send: $!");
    dienice("System write error: $!\n") unless defined $written;

    # Argument value.
    $name = 'arg' . $n;
    $out = $query->param($name);
      print LOG "Arg $n = <$out>\n" if ($debug >= 2);
    my $foo = substr($out, 0, -1); #TEMPTEMP? -- strip off last char
    $out = $foo . "\0"; # Append null to match command format.
      print LOG "Arg $n = <$out>\n" if ($debug >= 2);
    $written = syswrite(SOCK, $out, $length) || dienice("send: $!");	
    dienice("System write error: $!\n") unless defined $written;
}
  print LOG "Sent command arguments to jspop\n" if $debug;

	
# Now read the reply from the jspop, and pass it along to the JavaScreen.
my($bytesRead, $buf);
$bytesRead = 1;
my $totalBytes = 0;
while ($bytesRead > 0) {
    # Read and write up to 1k bytes at a time.
    $bytesRead = sysread(SOCK, $buf, 1024);
    if (!defined($bytesRead)) {
      dienice("recv: $!");
    }
    if ($bytesRead > 0) { 
	print $buf;
	  print LOG $buf if ($debug >= 3);
        $totalBytes += $bytesRead;
    }
}
  print LOG "Received $totalBytes bytes from jspop\n" if $debug;


close (SOCK) || dienice("close: $!");
  close(LOG) if $debug;
exit;

#------------------------------------------------------------------------
# Die, saving message to debug log file.

sub dienice {
  my ($errmsg);
  $errmsg = $_[0];
  if ($debug) {
    print LOG "$errmsg\n";
  }
  die $errmsg;
}
