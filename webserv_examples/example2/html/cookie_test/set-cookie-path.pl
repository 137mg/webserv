$cookie = $ENV{'HTTP_COOKIE'};

print <<"EOF";
Content-Type: text/html
Set-Cookie: cookie=ThIsIsAcOoKiE; Expires=Sun, 06 Nov 2024 08:49:37 GMT; Path=/

<h1>Cookie has been set!</h1>
<hr/>
<p>Showing http_cookie env. variable: <b>$cookie</b></p>
<a href="index.html">Go back</a>
EOF

1;
