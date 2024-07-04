#!/usr/bin/python3

import cgi, os, sys

cwd = os.getcwd()
upload_dir = 'uploads'

# Create the subdirectory if it doesn't already exist
if not os.path.exists(upload_dir):
    os.makedirs(upload_dir)

form = cgi.FieldStorage()
sys.path.insert(0, upload_dir)

if 'filename' in form:
    fileitem = form['filename']
    if fileitem.filename:
        filename = os.path.basename(fileitem.filename)
        filename = filename.replace(' ', '%20')
        with open(os.path.join(upload_dir, filename), 'wb') as file:
            file.write(fileitem.file.read())
print("<!DOCTYPE html><html><head>")
print("<meta charset='utf-8'>")
print("<meta name='viewport' content='width=device-width, initial-scale=1'>")
print("<title>Webserv</title>")
print("<link rel='icon' type='image/x-icon' href='/html/images/favicon.ico'>")
print("<link href='https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css' rel='stylesheet' integrity='sha384-QWTKZyjpPEjISv5WaRU9OFeRpok6YctnYmDr5pNlyT2bRjXh0JMhjY6hW+ALEwIH' crossorigin='anonymous'>")
print("<link href='/html/css/stylesheet.css' rel='stylesheet' type='text/css'>")
print("</head><body>")
print("    <div class ='navigation-bar'>")
print("        <a href='/'>Home</a>")
print("        <a href='/upload.html'>Upload</a>")
print("        <a href='/cgi-bin/showUploads.py'>Files</a>")
print("        <a href='/tests.html'>Tests</a>")
print("    </div>")
print("    <div class ='block'>")
print("        <h2>File successfully uploaded!</h2>")
print("    </div>")
print("    <script src='https://code.jquery.com/jquery-3.2.1.slim.min.js' integrity='sha384-KJ3o2DKtIkvYIK3UENzmM7KCkRr/rE9/Qpg6aAZGJwFDMVNA/GpGFF93hXpG5KkN' crossorigin='anonymous'></script>")
print("    <script src='https://cdn.jsdelivr.net/npm/popper.js@1.12.9/dist/umd/popper.min.js' integrity='sha384-ApNbgh9B+Y1QKtv3Rn7W3mgPxhU9K/ScQsAP7hUibX39j7fakFPskvXusvfa0b4Q' crossorigin='anonymous'></script>")
print("    <script src='https://cdn.jsdelivr.net/npm/bootstrap@4.0.0/dist/js/bootstrap.min.js' integrity='sha384-JZR6Spejh4U02d8jOt6vLEHfe/JQGiRRSQQxSfFWpi1MquVdAyjUar5+76PVCmYl' crossorigin='anonymous'></script>")
print("</body></html>")
