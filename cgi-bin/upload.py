#!/usr/bin/python3

import cgi, os, sys

cwd = os.getcwd()
sub_dir = 'cgi-bin/uploads'
upload_dir = os.path.join(cwd, sub_dir)

# Create the subdirectory if it doesn't already exist
if not os.path.exists(upload_dir):
    os.makedirs(upload_dir)

form = cgi.FieldStorage()
sys.path.insert(0, upload_dir)

if 'filename' in form:
    fileitem = form['filename']
    if fileitem.filename:
        filename = os.path.basename(fileitem.filename)
        with open(os.path.join(upload_dir, filename), 'wb') as file:
            file.write(fileitem.file.read())