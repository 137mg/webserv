local ($buffer, @pairs, $pair, $name, $value, %form);
$ENV{'REQUEST_METHOD'} =~ tr/a-z/A-Z/;
if ($ENV{'REQUEST_METHOD'} eq "POST") {
        read(STDIN, $buffer, $ENV{'CONTENT_LENGTH'});
} else {
        $buffer = $ENV{'QUERY_STRING'};
}

@pairs = split(/&/, $buffer);
foreach $pair (@pairs) {
        ($name, $value) = split(/=/, $pair);
        $value =~ tr/+/ /;
        $value =~ s/%(..)/pack("C", hex($1))/eg; # ???
        $FORM{$name} = $value;
}

$input_one = $FORM{input_one};
$input_two = $FORM{input_two};

print << "EOF";
Content-Type: text/html

<HTML>
  <HEAD><TITLE>CGI GENERATED PAGE</TITLE></HEAD>
  <BODY>
    <H1>CGI generated page.</H1>
    <HR>
    <LI><B>Input received from first text input: </B>$input_one
    <LI><B>Input received from second text input: </B>$input_two
    <LI><B>Buffer read from Webserver: </B>$buffer
    <HR/>
    <BR/>
    <A href="./index.html">Go back to index.</A>
  </BODY>
</HTML>
EOF

