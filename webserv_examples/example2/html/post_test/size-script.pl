local ($buffer);
$ENV{'REQUEST-METHOD'} =~ tr/a-z/A-Z/;
read(STDIN, $buffer, $ENV{'CONTENT_LENGTH'});

print << "EOF";
Content-Type: text/html

<html>
  <head>
    <title>Client_max_body_size result</title>
  </head>
  <body>
    <h1>Client_max_body_size success</h1>
    <hr/>
    <p>Webserv was capable of handling your request!</p>
    <li>Script path: <b>$ENV{'PATH_INFO'}</b>
    <li>Your input: <b>$buffer</b>
    <br/><hr/>
    <a href="index.html">Go back.</a>
  </body>
</html>
EOF

1;
