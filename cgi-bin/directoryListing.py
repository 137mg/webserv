#!/usr/bin/python3

import os

directory_path_raw = os.getenv("INPUT")
directory_path = directory_path_raw.lstrip('/')

print("<html><body><h1>Index of ")
print(directory_path_raw.rstrip("/"))
print("</h1>")
print("<hr>")

cwd = os.getcwd()
dir = os.path.join(cwd, directory_path)
if os.path.exists(dir) and os.path.isdir(dir):
	for name in os.listdir(dir):
		full_path = os.path.join(directory_path, name)
		link = name
		if os.path.isdir(full_path):
			print("<a href='" + directory_path_raw + link + "/" + "'>" + name + "/</a>")
		elif os.path.isfile(full_path):
			link = "/" + name
			print("<a href='" + link + "'>" + name + "</a>")
		print("<br>")
else:
	print("Directory not found")

print("</body></html>")