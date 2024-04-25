#!/usr/bin/python3

import os

for param in os.environ.keys():
	print ("%20s: %s" % (param, os.environ[param]))

# import os, cgi, sys
# import cgitb; cgitb.enable()

# form = cgi.FieldStorage()

# print('Content-type: text/html')
# sys.path.insert(0, os.getcwd())

# if 'filename' in form.keys():
# 	print("Recognised a file upload")
# else:
# 	print("No file was uploaded")
