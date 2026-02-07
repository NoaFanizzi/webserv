#!/usr/bin/env python3

import cgi
import cgitb
import html

# Enable debugging
cgitb.enable()

# Get the query parameter
form = cgi.FieldStorage()
user_input = form.getfirst("input", "")

# CGI header
print("HTTP/1.1 200 OK")
print("Content-Type: text/html")
print()

# Safely escape user input
safe_input = html.escape(user_input)

html_page = f"""
<!DOCTYPE html>
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
</html>
"""

print(html_page)

