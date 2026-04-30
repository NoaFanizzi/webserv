#!/usr/bin/env python3

import os
import sys
import html
from urllib.parse import parse_qs

method = os.environ.get("REQUEST_METHOD", "GET")
query_string = os.environ.get("QUERY_STRING", "")

if method == "POST":
    try:
        length = int(os.environ.get("CONTENT_LENGTH", "0"))
    except ValueError:
        length = 0
    body = sys.stdin.read(length) if length > 0 else ""
    params = parse_qs(body)
else:
    params = parse_qs(query_string)

user_input = params.get("input", [""])[0]
safe_input = html.escape(user_input)

print("HTTP/1.1 200 OK")
print("Content-Type: text/html; charset=UTF-8")
print()

print(f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Beautiful Page</title>
    <style>
        body {{
            font-family: 'Arial', sans-serif;
            background: linear-gradient(to right, #74ebd5, #ACB6E5);
            color: #333;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
        }}
        .container {{
            background: rgba(255, 255, 255, 0.85);
            padding: 40px;
            border-radius: 15px;
            box-shadow: 0 0 20px rgba(0,0,0,0.2);
            text-align: center;
        }}
        h1 {{
            margin-bottom: 20px;
        }}
        .user-input {{
            font-size: 1.2em;
            color: #334a70;
            font-weight: bold;
        }}
    </style>
</head>
<body>
    <div class="container">
        <h1>Welcome to My Beautiful Page!</h1>
        <p>This page is generated dynamically with a Python CGI script.</p>
        <p class="user-input">You typed: {safe_input}</p>
    </div>
</body>
</html>""")
