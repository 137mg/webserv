#!/usr/bin/python3

import cgi, os, sys


print("<!DOCTYPE html> \
<html> \
<head> \
	<meta charset='utf-8'> \
	<meta name='viewport' content='width=device-width, initial-scale=1'> \
	<title>Webserv</title> \
	<link rel='icon' type='image/x-icon' href='/html/images/favicon.ico'> \
	<link href='https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css' rel='stylesheet' integrity='sha384-QWTKZyjpPEjISv5WaRU9OFeRpok6YctnYmDr5pNlyT2bRjXh0JMhjY6hW+ALEwIH' crossorigin='anonymous'> \
	<link href='/html/css/stylesheet.css' rel='stylesheet' type='text/css'> \
</head><body> \
	\
	<div class ='navigation-bar'> \
	<a href='/'>Home</a> \
	<a href='/upload.html'>Upload</a> \
	<a href='/cgi-bin/showUploads.py'>Files</a> \
	<a href='/tests.html'>Tests</a> \
	</div>")

print("<script type='text/javascript' src='../js/showUploads.js'></script> \
	<div class ='block form-block files-block'> \
		<h2>Uploaded files</h2> \
		<ul> \
")

line = "<div class='entry'><button class='delete-button' value='hey'onclick='deleteFile(this.value)'>Delete</button><p>hey</p><br></div>"
print(line)
# cwd = os.getcwd()
# sub_dir = 'cgi-bin/uploads'

# print(f"Current directory: {cwd}")
# print(f"Sub directory: {sub_dir}")

print("</ul></div></body></html>")