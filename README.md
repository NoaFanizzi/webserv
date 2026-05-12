*This project has been created as part of the 42 curriculum by nofanizz, mvachon, lmarcucc*

# Project Name

## Description

This project was developed as part of the 42 curriculum.
Its goal is to explore C++ and apply the concepts related to HTTP servers.

The project focuses on:
- Making & parsing a formated configuration file
- HTTP protocol understanding
- using only 1 non blocking Poll for all the I/O

### Overview

The program is designed to:
- parse a configuration file
- launch a local server
- let you communicate with that server

This project helped us improve our understanding of:
- HTTP protocol
- poll, poll.h library
- Team collaboration and project organization

---

# Instructions

## Requirements

- GCC / Clang
- Make
- Linux / macOS

## Compilation

Clone the repository and compile the project using:

```bash
git clone git@github.com:NoaFanizzi/webserv.git
```
```bash
cd git@github.com:NoaFanizzi/webserv.git
```
```bash
make
```

## Config file

- (see default config file for an example)
- yourconfigfile.conf (.conf extension is essential)

## Executable

```bash
./webserv ConfigFile.conf
```

---

# Ressources

## Useful links
HTTP protocol
- https://developer.mozilla.org/fr/docs/Web/HTTP/Guides/Overview

Response Status
- https://developer.mozilla.org/fr/docs/Web/HTTP/Reference/Status

Configuration file
- https://nginx.org/en/docs/http/configuring_https_servers.html

CGI (common Gateway Interface)
- https://en.wikipedia.org/wiki/Common_Gateway_Interface

## A.I.
Mainly used for specific cases information and repetitive tasks

- Claude Code
- Chat GPT
