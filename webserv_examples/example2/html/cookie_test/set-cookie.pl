$cookie = $ENV{'HTTP_COOKIE'};

print <<"EOF";
Content-Type: text/html
Set-Cookie: cookie=tHiSiSaCoOkIe

<h1>Cookie has been set!</h1>
<hr/>
<p>Showing http_cookie env. variable: <b>$cookie</b></p>
<a href=\"index.html\">Go back</a>
EOF

1;
