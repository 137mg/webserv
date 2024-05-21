#!/usr/bin/python3

import cgi, os

form = cgi.FieldStorage()

# Get a filename here
fileitem = form['filename']

# Define the directory to save files
upload_dir = '/home/juvan-to/Downloads/Webserv_42/cgi-bin/tmp/'

# Test if the file was uploaded
if fileitem.filename:
   # Construct the full path to save the file
   filepath = os.path.join(upload_dir, os.path.basename(fileitem.filename))
    
   # Save the file
   with open(filepath, 'wb') as f:
      f.write(fileitem.file.read())
   message = 'The file "' + os.path.basename(fileitem.filename) + '" was uploaded successfully'
else:
   message = 'Uploading Failed'

print("Content-Type: text/html;charset=utf-8")
print ("Content-type:text/html\r\n")
print("<H1> " + message + " </H1>")
