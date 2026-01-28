# WEBSERV CONFIGURATION FILE SPECIFICATION

## Basic Structure
```conf
server {
    [server directives]
    
    location [path] {
        [location directives]
    }
}
```

---

## Server Directives

### Mandatory (all required)
```conf
port [0-65535];                    # Server port
host [ip_address];                 # Server IP address
root [path];                       # Root directory
index [filename];                  # Default index file
client_max_body_size [bytes];      # Max body size (≥ 0)
```

### Optional
```conf
autoindex [on|off];                      # Directory listing
error_page [400-599] [path];             # Custom error page (can be repeated)
```

---

## Location Directives
```conf
location [path] {
    root [path];                         # Root directory for this location
    index [filename];                    # Index file
    autoindex [on|off];                  # Directory listing
    allow_methods [GET] [POST] [DELETE]; # Allowed HTTP methods (space-separated)
}
```

---

## Syntax Rules

### ✅ Valid
```conf
# Semicolon is mandatory for directives
port 8080;
host 127.0.0.1;

# Multiple error_page allowed
error_page 404 /404.html;
error_page 500 /500.html;

# Multiple methods for allow_methods
allow_methods GET POST DELETE;

# Braces can be on same or next line
server {
location /path {
```

### ❌ Invalid
```conf
# Multiple values for single-value directives
port 8080 9090;                    # ❌ ERROR

# Missing semicolon
port 8080                          # ❌ ERROR

# Invalid port range
port 70000;                        # ❌ ERROR (must be 0-65535)

# Invalid error code
error_page 200 /ok.html;           # ❌ ERROR (must be 400-599)

# Invalid HTTP method
allow_methods GET PUT;             # ❌ ERROR (only GET, POST, DELETE)

# Negative values
port -1;                           # ❌ ERROR
client_max_body_size -100;         # ❌ ERROR

# Invalid autoindex value
autoindex yes;                     # ❌ ERROR (only on/off)
```

---

## Complete Example
```conf
server {
    port 8080;
    host 127.0.0.1;
    root /var/www/html;
    index index.html;
    client_max_body_size 5000000;
    autoindex off;
    
    error_page 404 /errors/404.html;
    error_page 500 /errors/500.html;
    
    location /upload {
        root /var/www/upload;
        autoindex on;
        allow_methods GET POST DELETE;
    }
    
    location /static {
        root /var/www/static;
        allow_methods GET;
    }
}

server {
    port 8081;
    host 127.0.0.1;
    root /var/www/site2;
    index index.html;
    client_max_body_size 2000000;
}
```

---

## Quick Reference

| Directive | Values | Semicolon | Repeatable |
|-----------|--------|-----------|------------|
| `port` | 0-65535 | ✅ Required | ❌ No |
| `host` | IP address | ✅ Required | ❌ No |
| `root` | Path | ✅ Required | ❌ No |
| `index` | Filename | ✅ Required | ❌ No |
| `client_max_body_size` | ≥ 0 | ✅ Required | ❌ No |
| `autoindex` | on/off | ✅ Required | ❌ No |
| `error_page` | Code + Path | ✅ Required | ✅ Yes |
| `allow_methods` | GET POST DELETE | ✅ Required | ❌ No |

**Note**: Whitespace, tabs, and newlines are ignored during parsing.