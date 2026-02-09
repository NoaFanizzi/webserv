#!/usr/bin/env python3
print("Content-Type: text/html\n")

# Force a crash
raise Exception("Intentional CGI crash for testing 500 error")

