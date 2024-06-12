#!/usr/bin/python3

import os


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

cwd = os.getcwd()
sub_dir = 'cgi-bin/uploads'
upload_dir = os.path.join(cwd, sub_dir)

if os.path.exists(upload_dir) and os.path.isdir(upload_dir):
	for filename in os.listdir(upload_dir):
		line = "<div class='entry'><button class='delete-button'"
		line += "value='"
		line += filename
		line += "'onclick='deleteFile(\""
		line += filename
		line += "\")'>Delete</button><p>"
		line += filename
		line += "</p><br></div>"
		print(line)
else:
	print("<p>No files uploaded</p>")

print("</ul></div></body></html>")