#!/usr/bin/env python3

import os
import sys
import html
from urllib.parse import parse_qs

method = os.environ.get("REQUEST_METHOD", "GET").upper()
content_type = os.environ.get("CONTENT_TYPE", "")
content_length = int(os.environ.get("CONTENT_LENGTH", "0") or 0)

raw_body = sys.stdin.buffer.read(content_length).decode("utf-8", errors="replace")

if method == "POST":
    params = parse_qs(raw_body, keep_blank_values=True)
else:
    params = parse_qs(os.environ.get("QUERY_STRING", ""), keep_blank_values=True)

user_input = params.get("input", [""])[0]

body = f"""<!DOCTYPE html>
<html>
<head><meta charset="UTF-8"><title>CGI</title></head>
<body>
  <h1>Hello from CGI</h1>
  <p>Method: {html.escape(method)}</p>
  <p>Content-Type: {html.escape(content_type)}</p>
  <p>Content-Length: {content_length}</p>
  <p>Raw body: {html.escape(raw_body)}</p>
</body>
</html>
"""

body_bytes = body.encode("utf-8")

headers = (
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n"
    f"Content-Length: {len(body_bytes)}\r\n"
    "Connection: close\r\n"
    "\r\n"
).encode("utf-8")

sys.stdout.buffer.write(headers + body_bytes)
