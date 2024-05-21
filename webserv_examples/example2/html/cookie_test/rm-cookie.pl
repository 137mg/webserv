$cookie = $ENV{'HTTP_COOKIE'};

print <<"EOF";
Content-Type: text/html
Set-Cookie: cookie=; Expires=Sun, 06 Nov 1994 08:49:37 GMT

<h1>Cookie has been reset!</h1>
<hr/>
<p>Showing http_cookie env. variable: <b>$cookie</b></p>
<a href="index.html">Go back</a>
EOF

1;
