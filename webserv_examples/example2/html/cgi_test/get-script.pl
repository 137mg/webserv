print "Content-Type: text/html\n";
print "Status: 200 OK(hello from get-script.pl)\n\n";

print "<!DOCTYPE>\n";
print "<HTML>\n";
print "<HEAD><TITLE>CGI Get test</TITLE></HEAD>\n";
print "<BODY>\n";
print "<H1>CGI Get test</H1><HR/>\n";
print "<P>Env. variables received from Webserver:</P><BR/><UL>\n";

foreach (sort keys %ENV) {
    print "<LI><B>$_</B>: $ENV{$_}<BR/>\n";
}

print "</UL></BODY></HTML>\n";

1;
