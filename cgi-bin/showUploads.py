#!/usr/bin/python3

import os

bs()

print("<!DOCTYPE html>")
print("<html>")
print("<head>")
print("    <meta charset='utf-8'>")
print("    <meta name='viewport' content='width=device-width, initial-scale=1'>")
print("    <title>Webserv</title>")
print("    <link rel='icon' type='image/x-icon' href='/html/images/favicon.ico'>")
print("    <link href='https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css' rel='stylesheet' integrity='sha384-QWTKZyjpPEjISv5WaRU9OFeRpok6YctnYmDr5pNlyT2bRjXh0JMhjY6hW+ALEwIH' crossorigin='anonymous'>")
print("    <link href='/html/css/stylesheet.css' rel='stylesheet' type='text/css'>")
print("</head>")
print("<body>")
print("    <div class ='navigation-bar'>")
print("        <a href='/'>Home</a>")
print("        <a href='/upload.html'>Upload</a>")
print("        <a href='/cgi-bin/showUploads.py'>Files</a>")
print("        <a href='/tests.html'>Tests</a>")
print("    </div>")
print("    <script>")
print("    function deleteFile(filename) {")
print("        const filePath = 'uploads/' + filename;")
print("        fetch(filePath,")
print("        {")
print("            method: 'DELETE'")
print("        })")
print("        .then(response =>")
print("        {")
print("            if (response.ok)")
print("            {")
print("                window.location.reload();")
print("            }")
print("            else")
print("            {")
print("                return response.text().then(text => {")
print("                    document.open();")
print("                    document.write(text);")
print("                    document.close();")
print("                });")
print("            }")
print("        })")
print("    }")
print("    </script>")

cwd = os.getcwd()
sub_dir = 'uploads'
upload_dir = os.path.join(cwd, sub_dir)

print("    <div class ='block form-block files-block'>")
print("        <h2>Uploaded files</h2>")
print("        <ul>")

if os.path.exists(upload_dir) and os.path.isdir(upload_dir):
	count = 0
	for filename in os.listdir(upload_dir):
		display_filename = filename.replace('%20', ' ')
		line = "<div class='entry'><button class='delete-button'"
		line += "value='"
		line += filename
		line += "'onclick='deleteFile(\""
		line += filename
		line += "\")'>Delete</button><p>"
		line += display_filename
		line += "</p><br></div>"
		print(line)
		count += 1
	if count == 0:
		print("<p>No files uploaded</p>")
else:
	print("<p>No files uploaded</p>")

print("</ul></div></body></html>")
