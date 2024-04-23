#!/usr/bin/python3

import cgi, os, sys
import cgitb; cgitb.enable()

sys.path.insert(0, os.getcwd())
form = cgi.FieldStorage()

print("Content-Type: text/plain\r\n")
for field in form.keys():
    print(field + "=" + str(form[field].value))


message = None

# Test if the file is loaded for the upload
if 'filename' in form:
	fileitem = form['filename']
	print("File found!\n")


# # Test if the file was uploaded
# if fileitem.filename:
#    # strip leading path from file name to avoid 
#    # directory traversal attacks
#    fn = os.path.basename(fileitem.filename)
#    open('/tmp/' + fn, 'wb').write(fileitem.file.read())

#    message = 'The file "' + fn + '" was uploaded successfully'
   
# else:
#    message = 'No file was uploaded'
   
# print(message)
