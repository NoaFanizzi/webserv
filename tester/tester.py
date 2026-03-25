#!/usr/bin/env python3
# ============================================================
#  Webserv 42 — HTTP Tester
#  Config associée : config.conf
#
#  Serveur 1 → port 8080  (host localhost / 127.0.0.1)
#    root: website
#    autoindex: on
#    locations:
#      /upload/         GET POST DELETE  (root: srcs)
#      /test            GET DELETE       (autoindex: off)
#      return 301 /test sur /upload/
#
#  Serveur 2 → port 6565  (host 127.0.0.1)
#    root: website
#    autoindex: on
#    locations:
#      /test            GET DELETE
#
#  Format expected :
#    int         → un seul code accepté
#    (int, ...)  → plusieurs codes acceptés
# ============================================================

import socket
import time
import os
import sys

HOST = "127.0.0.1"
PORT_1 = 8080
PORT_2 = 6565

GREEN   = "\033[32m"
RED     = "\033[31m"
YELLOW  = "\033[33m"
CYAN    = "\033[36m"
MAGENTA = "\033[35m"
RESET   = "\033[0m"
BOLD    = "\033[1m"
DIM     = "\033[2m"

passed = 0
failed = 0
errors = 0

upload_body = (
    "--BOUNDARY42\r\n"
    "Content-Disposition: form-data; name=\"file1\"; filename=\"test_upload.txt\"\r\n"
    "Content-Type: text/plain\r\n"
    "\r\n"
    "Hello from tester\r\n"
    "--BOUNDARY42--\r\n"
)
upload_req = (
    "POST / HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "Content-Type: multipart/form-data; boundary=BOUNDARY42\r\n"
    f"Content-Length: {len(upload_body)}\r\n"
    "\r\n"
    + upload_body
)

def _fmt(expected):
    if isinstance(expected, tuple):
        return "|".join(str(c) for c in expected)
    return str(expected)

def _matches(got, expected):
    if isinstance(expected, tuple):
        return got in expected
    return got == expected

def _pass(name, detail):
    global passed; passed += 1
    print(f"  [{GREEN}PASS{RESET}]  {name:<50} {detail}")

def _fail(name, detail):
    global failed; failed += 1
    print(f"  [{RED}FAIL{RESET}]  {name:<50} {detail}")

def _err(name, expected, detail):
    global errors; errors += 1
    print(f"  [{YELLOW}ERR {RESET}]  {name:<50} {YELLOW}{detail}{RESET}  {DIM}(expected {_fmt(expected)}){RESET}")

def send_test(name, expected, request, port=PORT_1, check_body=False, timeout=5):
    try:
        s = socket.socket()
        s.settimeout(timeout)
        start = time.time()
        s.connect((HOST, port))
        if isinstance(request, bytes):
            s.send(request)
        else:
            s.send(request.encode(errors="surrogateescape"))
        response = b""
        while True:
            try:
                chunk = s.recv(4096)
                if not chunk:
                    break
                response += chunk
            except socket.timeout:
                break
            except ConnectionResetError:
                break
        duration = (time.time() - start) * 1000
        s.close()

        if not response:
            _fail(name, f"NO RESPONSE  {DIM}(expected {_fmt(expected)}){RESET}")
            return

        decoded = response.decode(errors="ignore")
        first_line = decoded.split("\r\n")[0][:80]
        parts = first_line.split(" ", 2)
        got_code = int(parts[1]) if len(parts) >= 2 and parts[1].isdigit() else None

        if check_body and _matches(got_code, expected):
            body = decoded.split("\r\n\r\n", 1)[1].strip() if "\r\n\r\n" in decoded else ""
            if body:
                _pass(name, f"{first_line} + body présent  {DIM}({duration:.1f} ms){RESET}")
            else:
                _fail(name, f"{first_line} mais body VIDE  {DIM}(expected {_fmt(expected)}){RESET}  ({duration:.1f} ms)")
            return

        if _matches(got_code, expected):
            _pass(name, f"{first_line}  {DIM}({duration:.1f} ms){RESET}")
        else:
            _fail(name, f"got [{RED}{first_line}{RESET}]  {DIM}expected {BOLD}{_fmt(expected)}{RESET}  ({duration:.1f} ms)")

    except socket.timeout:
        if expected == "TIMEOUT":
            _pass(name, "timed out as expected")
        else:
            _fail(name, f"TIMEOUT  {DIM}(expected {_fmt(expected)}){RESET}")
    except ConnectionRefusedError:
        _err(name, expected, f"Connection refused on port {port} — server running?")
    except Exception as e:
        _err(name, expected, str(e))

# ──────────────────────────────────────────────────────────────
# TESTS PORT 8080
# ──────────────────────────────────────────────────────────────

tests_port_8080 = [

    # ── GET basiques ──────────────────────────────────────────

    ("GET_ROOT_200",
     200,
     "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"),

    ("GET_ROOT_WITH_HOST_IP",
     200,
     "GET / HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n"),

    ("GET_EXISTING_PAGE",
     200,
     "GET /index2.html HTTP/1.1\r\nHost: localhost\r\n\r\n"),

    ("GET_INEXISTANT_404",
     404,
     "GET /fichier_qui_nexiste_pas.html HTTP/1.1\r\nHost: localhost\r\n\r\n"),

    ("GET_ROOT_SLASH_AUTOINDEX",
     (200, 404),
     "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"),

    ("GET_CGI_HELLO_PY",
     200,
     "GET /cgi/hello.py HTTP/1.1\r\nHost: localhost\r\n\r\n"),

    ("GET_CGI_HELLO_WITH_QUERY",
     200,
     "GET /cgi/hello.py?input=test HTTP/1.1\r\nHost: localhost\r\n\r\n"),

    ("GET_CGI_ERROR_500",
     500,
     "GET /cgi/error.py HTTP/1.1\r\nHost: localhost\r\n\r\n"),

    # ── Location /test ────────────────────────────────────────

    ("GET_LOCATION_TEST",
     (200, 404),
     "GET /test HTTP/1.1\r\nHost: localhost\r\n\r\n"),

    ("GET_TEST_HTML_FILE",
     (200, 404),
     "GET /test/test.html HTTP/1.1\r\nHost: localhost\r\n\r\n"),

    # POST interdit sur /test (allow_methods GET DELETE)
    ("POST_TEST_405",
     405,
     "POST /test HTTP/1.1\r\nHost: localhost\r\nContent-Length: 5\r\n\r\nhello"),

    # ── Location /upload/ ─────────────────────────────────────

    # GET /upload/ doit retourner 301 (return 301 /test dans config)
    ("GET_UPLOAD_301_REDIRECT",
     301,
     "GET /upload/ HTTP/1.1\r\nHost: localhost\r\n\r\n"),

    # ── POST upload fichier ───────────────────────────────────

    ("POST_UPLOAD_MULTIPART", (200, 201), upload_req),

    # ── DELETE ────────────────────────────────────────────────

    # Suppression d'un fichier qui n'existe pas
    ("DELETE_INEXISTANT_404",
     (404, 400),
     "DELETE /upload/fichier_inexistant.txt HTTP/1.1\r\nHost: localhost\r\n\r\n"),

    # ── client_max_body_size 2000000 ──────────────────────────

    # Body légèrement en dessous de 2MB → OK
    ("POST_BODY_UNDER_LIMIT",
     (200, 201),
     (
         "POST /upload/ HTTP/1.1\r\n"
         "Host: localhost\r\n"
         "Content-Type: multipart/form-data; boundary=X\r\n"
         "Content-Length: {size}\r\n"
         "\r\n"
         "--X\r\n"
         "Content-Disposition: form-data; name=\"f\"; filename=\"big.bin\"\r\n"
         "Content-Type: application/octet-stream\r\n"
         "\r\n"
         + "A" * 100000 + "\r\n"
         "--X--\r\n"
     ).replace("{size}", str(100000 + 130)),
    ),

    # Body > 2MB → 413
    ("POST_BODY_OVER_LIMIT_413",
     413,
     (
         "POST /upload/ HTTP/1.1\r\n"
         "Host: localhost\r\n"
         "Content-Length: 2100000\r\n"
         "\r\n"
         + "A" * 2100000
     )),

    # ── Méthodes invalides ────────────────────────────────────

    ("HEAD_NOT_SUPPORTED",
     405,
     "HEAD / HTTP/1.1\r\nHost: localhost\r\n\r\n"),

    ("PUT_NOT_SUPPORTED",
     405,
     "PUT / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 4\r\n\r\ndata"),

    ("OPTIONS_NOT_SUPPORTED",
     405,
     "OPTIONS / HTTP/1.1\r\nHost: localhost\r\n\r\n"),

    ("PATCH_NOT_SUPPORTED",
     405,
     "PATCH / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 4\r\n\r\ndata"),

    ("UNKNOWN_METHOD",
     405,
     "HELLO / HTTP/1.1\r\nHost: localhost\r\n\r\n"),

    ("LOWERCASE_METHOD_400",
     400,
     "get / HTTP/1.1\r\nHost: localhost\r\n\r\n"),

    ("MIXEDCASE_METHOD_400",
     400,
     "Get / HTTP/1.1\r\nHost: localhost\r\n\r\n"),

    # ── Headers invalides ─────────────────────────────────────

    ("MISSING_HOST_400",
     400,
     "GET / HTTP/1.1\r\n\r\n"),

    ("HEADER_NO_COLON_400",
     400,
     "GET / HTTP/1.1\r\nHost localhost\r\n\r\n"),

    ("HEADER_NO_VALUE_400",
     400,
     "GET / HTTP/1.1\r\nHost:\r\n\r\n"),

    ("HEADER_ONLY_SPACES_400",
     400,
     "GET / HTTP/1.1\r\nHost:    \r\n\r\n"),

    # ── Body invalide ─────────────────────────────────────────

    ("POST_NO_CONTENT_LENGTH_400",
     (400, 411),
     "POST /upload/ HTTP/1.1\r\nHost: localhost\r\n\r\nhello"),

    ("CONTENT_LENGTH_NEGATIVE_400",
     400,
     "POST /upload/ HTTP/1.1\r\nHost: localhost\r\nContent-Length: -1\r\n\r\nhello"),

    ("CONTENT_LENGTH_STRING_400",
     400,
     "POST /upload/ HTTP/1.1\r\nHost: localhost\r\nContent-Length: abc\r\n\r\nhello"),

    ("CONTENT_LENGTH_FLOAT_400",
     400,
     "POST /upload/ HTTP/1.1\r\nHost: localhost\r\nContent-Length: 3.5\r\n\r\nhello"),

    # ── Version HTTP ──────────────────────────────────────────

    ("HTTP_VERSION_WRONG_400",
     400,
     "GET / HTTP/9.9\r\nHost: localhost\r\n\r\n"),

    ("HTTP_VERSION_LOWERCASE_400",
     400,
     "GET / http/1.1\r\nHost: localhost\r\n\r\n"),

    ("HTTP_VERSION_MISSING_400",
     400,
     "GET /\r\nHost: localhost\r\n\r\n"),

    # ── URI ───────────────────────────────────────────────────

    ("EMPTY_REQUEST_400",
     400,
     "\r\n"),

    ("PATH_TRAVERSAL",
     (400, 403, 404),
     "GET /../../../etc/passwd HTTP/1.1\r\nHost: localhost\r\n\r\n"),

    # ── Pages d'erreur non vides ──────────────────────────────

    ("ERROR_PAGE_404_BODY_NOT_EMPTY",
     404,
     "GET /nonexistent_page_xyz HTTP/1.1\r\nHost: localhost\r\n\r\n"),

    ("ERROR_PAGE_405_BODY_NOT_EMPTY",
     405,
     "HEAD / HTTP/1.1\r\nHost: localhost\r\n\r\n"),
]

# ──────────────────────────────────────────────────────────────
# TESTS PORT 6565
# ──────────────────────────────────────────────────────────────

tests_port_6565 = [

    ("PORT6565_GET_ROOT",
     200,
     "GET / HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n"),

    ("PORT6565_GET_TEST",
     (200, 404),
     "GET /test HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n"),

    # POST interdit sur ce serveur (aucune location POST)
    ("PORT6565_POST_ROOT_405",
     405,
     "POST / HTTP/1.1\r\nHost: 127.0.0.1\r\nContent-Length: 5\r\n\r\nhello"),

    ("PORT6565_GET_INEXISTANT",
     404,
     "GET /this_does_not_exist.html HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n"),

    ("PORT6565_MISSING_HOST",
     400,
     "GET / HTTP/1.1\r\n\r\n"),
]

# ──────────────────────────────────────────────────────────────
# TESTS COMPLEXES
# ──────────────────────────────────────────────────────────────

def test_slow_body():
    """Body incomplet envoyé lentement → timeout 408"""
    name, expected = "SLOW_BODY_TIMEOUT_408", 408
    headers = "POST /upload/ HTTP/1.1\r\nHost: localhost\r\nContent-Length: 100\r\n\r\n"
    try:
        s = socket.socket()
        s.settimeout(10)
        start = time.time()
        s.connect((HOST, PORT_1))
        s.send(headers.encode())
        s.send(b"A" * 10)
        time.sleep(6)
        try:
            resp = s.recv(4096)
        except socket.timeout:
            resp = b""
        duration = (time.time() - start) * 1000
        s.close()
        if not resp:
            _fail(name, f"No response after {duration:.0f}ms — server hung (no timeout)")
            return
        fl = resp.decode(errors="ignore").split("\r\n")[0]
        p = fl.split(" ", 2)
        got = int(p[1]) if len(p) >= 2 and p[1].isdigit() else None
        if got == 408:
            _pass(name, f"{fl}  ({duration:.1f} ms)")
        else:
            _fail(name, f"got [{fl}]  {DIM}expected {BOLD}408{RESET}  ({duration:.1f} ms)")
    except Exception as e:
        _err(name, expected, str(e))


def test_delete_uploaded_file():
    """Upload un fichier puis le DELETE"""
    upload_name = "tester_delete_me.txt"
    upload_body = (
        "--DELBOUNDARY\r\n"
        f"Content-Disposition: form-data; name=\"f\"; filename=\"{upload_name}\"\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"
        "delete me\r\n"
        "--DELBOUNDARY--\r\n"
    )
    upload_req = (
        "POST /upload/ HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Type: multipart/form-data; boundary=DELBOUNDARY\r\n"
        f"Content-Length: {len(upload_body)}\r\n"
        "\r\n"
        + upload_body
    )

    name_post = "UPLOAD_THEN_DELETE_POST"
    name_del  = "UPLOAD_THEN_DELETE_DELETE"

    # 1. Upload
    try:
        s = socket.socket(); s.settimeout(5); s.connect((HOST, PORT_1))
        s.send(upload_req.encode())
        resp = b""
        while True:
            chunk = s.recv(4096)
            if not chunk: break
            resp += chunk
        s.close()
        fl = resp.decode(errors="ignore").split("\r\n")[0]
        p = fl.split(" ", 2)
        got = int(p[1]) if len(p) >= 2 and p[1].isdigit() else None
        if got in (200, 201):
            _pass(name_post, fl)
        else:
            _fail(name_post, f"got [{fl}]  expected 200|201")
            return
    except Exception as e:
        _err(name_post, (200, 201), str(e))
        return

    # 2. DELETE le fichier uploadé (dans upload/ directory)
    try:
        s = socket.socket(); s.settimeout(5); s.connect((HOST, PORT_1))
        s.send(f"DELETE /upload/{upload_name} HTTP/1.1\r\nHost: localhost\r\n\r\n".encode())
        resp = b""
        while True:
            chunk = s.recv(4096)
            if not chunk: break
            resp += chunk
        s.close()
        fl = resp.decode(errors="ignore").split("\r\n")[0]
        p = fl.split(" ", 2)
        got = int(p[1]) if len(p) >= 2 and p[1].isdigit() else None
        if got in (200, 204, 404):
            _pass(name_del, fl)
        else:
            _fail(name_del, f"got [{fl}]  expected 200|204|404")
    except Exception as e:
        _err(name_del, (200, 204), str(e))


def test_autoindex_content():
    """GET sur un dossier avec autoindex on → HTML listé"""
    name, expected = "AUTOINDEX_HTML_CONTENT", 200
    try:
        s = socket.socket(); s.settimeout(5); s.connect((HOST, PORT_1))
        s.send(b"GET / HTTP/1.1\r\nHost: localhost\r\n\r\n")
        resp = b""
        while True:
            chunk = s.recv(4096)
            if not chunk: break
            resp += chunk
        s.close()
        decoded = resp.decode(errors="ignore")
        fl = decoded.split("\r\n")[0]
        body = decoded.split("\r\n\r\n", 1)[1] if "\r\n\r\n" in decoded else ""
        p = fl.split(" ", 2)
        got = int(p[1]) if len(p) >= 2 and p[1].isdigit() else None
        if got == 200 and ("<html" in body.lower() or "<HTML" in body):
            _pass(name, f"{fl} + HTML body")
        elif got == 200:
            _fail(name, f"{fl} mais body non-HTML: {body[:60]!r}")
        else:
            _fail(name, f"got [{fl}]  expected 200 with HTML")
    except Exception as e:
        _err(name, expected, str(e))


def test_redirect_301_location_header():
    """GET /upload/ → 301 avec header Location"""
    name, expected = "REDIRECT_301_LOCATION_HEADER", 301
    try:
        s = socket.socket(); s.settimeout(5); s.connect((HOST, PORT_1))
        s.send(b"GET /upload/ HTTP/1.1\r\nHost: localhost\r\n\r\n")
        resp = b""
        while True:
            chunk = s.recv(4096)
            if not chunk: break
            resp += chunk
        s.close()
        decoded = resp.decode(errors="ignore")
        fl = decoded.split("\r\n")[0]
        p = fl.split(" ", 2)
        got = int(p[1]) if len(p) >= 2 and p[1].isdigit() else None
        has_location = "Location:" in decoded or "location:" in decoded
        if got == 301 and has_location:
            _pass(name, f"{fl} + Location header présent")
        elif got == 301:
            _fail(name, f"{fl} mais header Location ABSENT")
        else:
            _fail(name, f"got [{fl}]  expected 301")
    except Exception as e:
        _err(name, expected, str(e))


def test_cgi_post_body():
    """POST vers un script CGI avec body → CGI reçoit le body"""
    name, expected = "CGI_POST_BODY_PASSED", 200
    body = "input=helloworld"
    req = (
        "POST /cgi/hello.py HTTP/1.1\r\n"
        "Host: localhost\r\n"
        f"Content-Length: {len(body)}\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "\r\n"
        + body
    )
    try:
        s = socket.socket(); s.settimeout(5); s.connect((HOST, PORT_1))
        s.send(req.encode())
        resp = b""
        while True:
            chunk = s.recv(4096)
            if not chunk: break
            resp += chunk
        s.close()
        decoded = resp.decode(errors="ignore")
        fl = decoded.split("\r\n")[0]
        p = fl.split(" ", 2)
        got = int(p[1]) if len(p) >= 2 and p[1].isdigit() else None
        if got == 200:
            _pass(name, fl)
        else:
            _fail(name, f"got [{fl}]  expected 200")
    except Exception as e:
        _err(name, expected, str(e))


# ──────────────────────────────────────────────────────────────
# SECTIONS
# ──────────────────────────────────────────────────────────────

SECTION_NAMES = {
    "GET_ROOT": "GET basiques",
    "GET_EXISTING": "GET basiques",
    "GET_INEXISTANT": "GET basiques",
    "GET_CGI": "CGI",
    "GET_LOCATION": "Locations",
    "GET_TEST": "Locations",
    "POST_TEST": "Locations",
    "GET_UPLOAD": "Location /upload/ + redirections",
    "POST_UPLOAD": "Upload (POST)",
    "DELETE": "DELETE",
    "POST_BODY": "client_max_body_size",
    "HEAD": "Méthodes non autorisées",
    "PUT": "Méthodes non autorisées",
    "OPTIONS": "Méthodes non autorisées",
    "PATCH": "Méthodes non autorisées",
    "UNKNOWN": "Méthodes non autorisées",
    "LOWERCASE": "Méthodes non autorisées",
    "MIXEDCASE": "Méthodes non autorisées",
    "MISSING_HOST": "Headers invalides",
    "HEADER": "Headers invalides",
    "POST_NO": "Body invalide",
    "CONTENT": "Body invalide",
    "HTTP_VERSION": "Version HTTP",
    "EMPTY_REQUEST": "Format requête",
    "PATH_TRAVERSAL": "Sécurité",
    "ERROR_PAGE": "Pages d'erreur",
}

def get_section(name):
    for prefix, section in sorted(SECTION_NAMES.items(), key=lambda x: -len(x[0])):
        if name.startswith(prefix):
            return section
    return "Autre"

# ──────────────────────────────────────────────────────────────
# RUNNER
# ──────────────────────────────────────────────────────────────

print(f"\n{BOLD}{'='*68}{RESET}")
print(f"{BOLD}  WEBSERV 42 — HTTP TESTER  —  config.conf{RESET}")
print(f"{BOLD}  Serveur 1 : {HOST}:{PORT_1}   root=website  autoindex=on  max_body=2MB{RESET}")
print(f"{BOLD}  Serveur 2 : {HOST}:{PORT_2}   root=website  autoindex=on{RESET}")
print(f"{BOLD}{'='*68}{RESET}")

# ── Port 8080
print(f"\n{CYAN}{BOLD}╔══ PORT {PORT_1} ══════════════════════════════════════════════════╗{RESET}")
current_section = None
for name, expected, request in tests_port_8080:
    check_body = name.startswith("ERROR_PAGE")
    section = get_section(name)
    if section != current_section:
        current_section = section
        print(f"\n  {CYAN}{BOLD}── {section} ──{RESET}")
    send_test(name, expected, request, port=PORT_1, check_body=check_body)

# ── Port 6565
print(f"\n{MAGENTA}{BOLD}╔══ PORT {PORT_2} ══════════════════════════════════════════════════╗{RESET}")
for name, expected, request in tests_port_6565:
    send_test(name, expected, request, port=PORT_2)

# ── Tests complexes
print(f"\n{YELLOW}{BOLD}╔══ TESTS COMPLEXES ══════════════════════════════════════════════╗{RESET}")
complex_tests = [
    ("Upload + Delete séquentiel", test_delete_uploaded_file),
    ("Autoindex HTML content", test_autoindex_content),
    ("Redirection 301 + Location header", test_redirect_301_location_header),
    ("CGI POST body passé au script", test_cgi_post_body),
    ("Slow body → timeout 408", test_slow_body),
]
for section_name, fn in complex_tests:
    print(f"\n  {YELLOW}── {section_name}{RESET}")
    fn()

# ── Résumé
total = passed + failed + errors
bar_w = 32
p_fill = int(bar_w * passed / total) if total else 0
f_fill = int(bar_w * failed / total) if total else 0
e_fill = bar_w - p_fill - f_fill
bar = f"{GREEN}{'█'*p_fill}{RED}{'█'*f_fill}{YELLOW}{'█'*e_fill}{RESET}"

print(f"\n{BOLD}{'='*68}{RESET}")
print(f"  {bar}  {BOLD}{GREEN}{passed} passed{RESET} / {RED}{failed} failed{RESET} / {YELLOW}{errors} errors{RESET} / {total} total")
print(f"{BOLD}{'='*68}{RESET}\n")