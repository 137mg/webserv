#!/usr/bin/python3

import os

directory_path = os.getenv("INPUT")

print("<html><body><h1>Index of ")
print(directory_path)
print("</h1>")

cwd = os.getcwd()
dir = os.path.join(cwd, directory_path)
print(dir)
print("</body></html>")