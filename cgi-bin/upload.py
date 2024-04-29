#!/usr/bin/python3

import cgi, os, sys

# Get current directory
sub_dir = 'cgi-bin/uploads'
cwd = os.getcwd()
upload_dir = os.path.join(cwd, sub_dir)
# Create the subdirectory if it doesn't already exist
if not os.path.exists(upload_dir):
    os.makedirs(upload_dir)

form = cgi.FieldStorage()
sys.path.insert(0, upload_dir)
message = 'No file was uploaded'

if 'filename' in form:
    fileitem = form['filename']
    filename = os.path.basename(fileitem.filename)
    open(os.path.join(upload_dir, filename), 'wb').write(fileitem.file.read())
