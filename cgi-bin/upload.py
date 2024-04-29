#!/usr/bin/python3

import sys, cgi

form = cgi.FieldStorage()

if 'filename' in form:
	print("Recognises file upload")
else:
	print("Not recognising upload")