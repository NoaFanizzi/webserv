

## ✅ DÉJÀ FIXÉ

### B1. ~~Boucle `send()` non guidée par poll()~~ — FIXÉ
`Client::pollOutHandler` (Client.cpp:112-135) : un seul `send()` par tour de POLLOUT, buffer + offset persistants, EAGAIN géré, fermeture seulement quand tout est écrit.

### B3. ~~Pas de support du **Transfer-Encoding: chunked**~~ — FIXÉ
- `checkRequest()` (Request.cpp:160-163) : détecte `transfer-encoding: chunked`, ne lève pas 411 si présent.
- `isValid()` (Request.cpp:126-132) : attend le chunk terminal `0\r\n\r\n` avant de valider la requête.
- `unchunkBody()` (Request.cpp:366-406) : décode les chunks (taille hex + data), gère les extensions de chunk (`;`).
- `parse()` (Request.cpp:462-473) : appelle `unchunkBody()`, met à jour `_contentLengthBody`, valide `client_max_body_size`.

---

## 🔥 BLOQUANTS — note 0 / défense impossible

### B4. ~~Réponse CGI **sans status line HTTP**~~ — FIXÉ
- `Response::getFullResponse()` (Response.cpp) : parse la sortie CGI, détecte `\r\n\r\n` ou `\n\n`, extrait l'en-tête `Status:` si présent (sinon `200 OK`), filtre les autres headers CGI, ajoute `Content-Length` si absent, construit `HTTP/1.1 <code> <text>\r\n` + headers + body.
- `website/cgi/hello.py` : suppression du `print("HTTP/1.1 200 OK")` — sortie CGI standard désormais.
- `website/cgi/hello2.py` : suppression de `"HTTP/1.1 200 OK\r\n"` du bloc headers.

### B5. Pas de **README.md**
Sujet, chapitre V : `README.md` à la racine, rédigé en anglais, avec :
- 1ère ligne en italique : *This project has been created as part of the 42 curriculum by <login1>[, <login2>…].*
- section **Description**
- section **Instructions** (compilation / exec)
- section **Resources** (références + utilisation IA)

**À faire** : créer le fichier (logins : `mvachon` / `nofanizz`).

---

## 🔴 GROS RISQUES À LA DÉFENSE

### R1. ~~`static bool _finalAutoIndex` dans `Response.cpp` (ligne 8)~~ — FIXÉ
Ajouté comme membre `bool _finalAutoIndex` dans `Response.hpp`, initialisé à `false` dans le constructeur et dans `setRequest()`. La variable globale dans `Response.cpp` est supprimée.

### R2. ~~CGI ne fait pas de `chdir()` avant `execve`~~ — FIXÉ
`CgiManager::start()` (child) : extrait le dossier du script via `rfind('/')`, appelle `chdir()` avant `execve`. Les scripts peuvent désormais utiliser des chemins relatifs.

### R3. `if (_version != "HTTP/1.1") throw Http505Exception();`
Vérifié : `GET / HTTP/1.0` → `505`. Sujet : *« HTTP 1.0 is suggested as a reference point »* — refuser HTTP/1.0 va à l'inverse. Telnet, certains clients de stress, et la défense utilisent 1.0.
**À faire** : accepter `HTTP/1.0` ET `HTTP/1.1`.

### R4. ~~`errno` consulté après `std::remove`~~ — FIXÉ
`Response.cpp` : `access(F_OK)` avant `remove` pour le 404, `access(W_OK)` pour le 403, `remove` restant échoue → 500. Plus de lecture fragile de `errno` après `std::remove`.

### R5. Fonctions hors-liste utilisées
- `std::remove` (cstdio) — pour DELETE
- `std::time(NULL)` (ctime) — pour les timeouts
- `perror` — `Server.cpp:78`, `CgiManager.cpp:121`
Aucune n'est dans la liste **External Function** du sujet. Toléré dans 99 % des défenses, mais peut être bloqué.
**À faire (si évaluateur strict)** : noter dans le README qu'on les considère comme libc standard.

### R6. Condition bancale dans `main.cpp` (ligne 30)
```cpp
if (!av[1] && ac != 2) { … }
```
Si `ac == 0`, `av[1]` est un accès hors-tableau (UB). Si `ac >= 3`, on ne rentre pas dans le `if` mais on passe à `config.setFile(av[1])` avec `av[1]` non-null → OK pour ce cas. Le vrai danger : `ac == 0`.
**À faire** : `if (ac != 2) { std::cerr << "Usage: ./webserv <config>\n"; return 1; }`.

### C1. `config.conf` actuel **mal formé**
Lignes 10-13 :
```
location /upload/
    autoindex on;
    allow_methods GET POST DELETE;
    upload_dir website/upload/;
```
→ pas d'accolade ouvrante ni fermante. C'est le 1er fichier que le correcteur lance. **À corriger.**

Ligne 23 : `return 301 /upload` — **manque `;`**.

---

## 🟠 CONFIG FILE — incohérences

### C2. CGI **non configurable** dans le fichier
`CgiManager::isCgi()` détecte en dur `.py` et `.php`. Sujet : *« Execution of CGI, based on file extension (for example .php). »* — c'est une directive attendue dans la config.
**À faire** :
- Ajouter `cgi_pass .py /usr/bin/python3;` (ou similaire) dans `LocationConfig`.
- `CgiManager::start` exec `execve(interpreter, {interpreter, scriptPath, NULL}, env)` au lieu d'exec le script direct.

---

## 🟡 PARSING REQUEST — robustesse

### P1. `parsePostMethod` ne gère que `multipart/form-data`
Sans boundary, le body est silencieusement ignoré. POST `application/json` ou `application/x-www-form-urlencoded` → 200 vide.
**À faire** : si pas de multipart, stocker le body brut dans `_body` (utile pour CGI).

### P2. Pas de validation du nom de fichier uploadé
`parsePostMethod` strip le `/` final mais pas les `..`. `filename="../../../etc/passwd"` → risque de path traversal.
**À faire** : rejeter tout filename contenant `..` ou commençant par `/` après normalisation.

### P3. Headers parsing trop strict
`separateHeaders` (Request.cpp:220) refuse `Header:value` (sans espace), `Header:  value` (double espace), tabulations. La RFC autorise tout ça. **Risque de faux 400** sur clients atypiques.

---

## 🟢 À NETTOYER

### Debug `cout` en production
`Client.cpp:57-58` : `std::cout << _rawRequest << std::endl;` — affiche chaque requête brute sur stdout.
`CgiManager.cpp:62` : `std::cout << "metssshod:" << _request.getMethod() << std::endl;`
**À supprimer avant la défense.**

### R7. `BUFFER_SIZE 1000` dans `recv()`
Volontairement petit pour tester le streaming. Pour des gros uploads, la concat `std::string` + scan `find("\r\n\r\n")` à chaque tour explose en O(n²).
**À faire (si temps)** : passer à 4096 octets et ne `find("\r\n\r\n")` qu'une fois.

### R8. Listening socket pas en non-blocking
`Server::Server` ne met pas `_fd` en `O_NONBLOCK`. `accept()` est protégé par `poll(POLLIN)` donc non-bloquant en pratique, mais la tradition 42 est de tout flagger O_NONBLOCK.
**À faire** : `fcntl(_fd, F_SETFL, O_NONBLOCK)` après `socket()`.

---

## 🟢 BONNES CHOSES — déjà OK

- Makefile : règles `all/clean/fclean/re`, `-Wall -Wextra -Werror -std=c++98`, pas de relinking inutile. Bonus : `-MMD -MP` pour les deps. ✅
- `poll()` unique dans `WebServer::run()` couvre listen + clients + pipes CGI. ✅
- `accept` retourne un fd mis en `O_NONBLOCK` via `fcntl` (Server.cpp:81). ✅
- **Boucle `send()` guidée par poll()** : un seul `send()` par POLLOUT, buffer+offset persistants (Client.cpp:112-135). ✅
- Pages d'erreur en dur (400, 403, 404, 405, 408, 411, 413, 414, 500, 504, 505) ✅
- `error_page` configurable + override par config (`Response::getErrorPageContent`). ✅
- `client_max_body_size` parsé + appliqué (413 levé). ✅
- `autoindex` + `index` + `root` par location. ✅
- Redirection auto `/dossier` → `/dossier/` (301). ✅
- Multi-server / multi-ports (validé par config 8080+6565). ✅
- Stress 50 GET concurrents : 50/50 200 ✅, serveur survit ✅.
- Tester Python `tester/tester.py` présent. ✅
- DELETE 404 propre quand la cible n'existe pas. ✅
- POST body envoyé au CGI via stdin (CgiWriter). ✅

---

## 📋 ORDRE D'ATTAQUE RECOMMANDÉ

| # | Tâche | Fichier principal | Priorité |
|---|-------|-------------------|----------|
| 1 | ~~**Status line manquante CGI** (B4)~~ — FIXÉ | `Response.hpp`, `Client.cpp` | ✅ |
| 2 | **`README.md`** (B5) | racine | 🔥 |
| 3 | ~~**Chunked transfer-encoding** (B3)~~ — FIXÉ | `Request.cpp` | ✅ |
| 4 | **HTTP/1.0 accepté** (R3) | `Request.cpp::checkRequest` | 🔴 |
| 5 | **`_finalAutoIndex` static → membre** (R1) | `Response.cpp/hpp` | 🔴 |
| 6 | **`chdir` dans le child CGI** (R2) | `CgiManager.cpp::start` | 🔴 |
| 7 | **`config.conf` mal formé** (C1) | `config.conf` | 🔴 |
| 8 | **`main.cpp` argument check** (R6) | `main.cpp` | 🔴 |
| 9 | **Supprimer les debug `cout`** | `Client.cpp:57`, `CgiManager.cpp:62` | 🟢 |
| 10 | `errno` après remove (R4) | `Response.cpp` | 🟠 |
| 11 | CGI configurable par extension (C2) | `Config.hpp`, parser, `CgiManager` | 🟠 |
| 12 | POST non-multipart → garder body (P1) | `Request.cpp` | 🟠 |
| 13 | Listening socket O_NONBLOCK (R8) | `Server.cpp` | 🟡 |
| 14 | Filename upload : reject `..` (P2) | `Request.cpp::parsePostMethod` | 🟡 |
| 15 | Headers parsing tolérant (P3) | `Request.cpp::separateHeaders` | 🟡 |
| 16 | Bonus : cookies / sessions | nouveau module | 🔵 |

---

## 🧪 CHECKLIST DE VALIDATION FINALE

À cocher avant la défense :

- [ ] `make re` compile sans warning, `make` 2x ne relinke pas
- [ ] `./webserv` (sans arg) → message d'erreur propre, pas de crash
- [ ] `./webserv config_invalide` → erreur propre
- [ ] `./webserv config.conf` → 2 ports actifs (8080, 6565)
- [ ] `curl http://127.0.0.1:8080/` → 200 + index.html
- [ ] `curl http://127.0.0.1:8080/inexistant` → 404 stylé
- [ ] `curl -X POST -F "file=@photo.png" http://127.0.0.1:8080/upload/` → fichier dans `website/upload/`
- [ ] `curl -X DELETE http://127.0.0.1:8080/upload/photo.png` → 200 + fichier supprimé
- [ ] `curl -X POST -d "input=Toto" http://127.0.0.1:8080/cgi/hello.py` → page avec `You typed: Toto`
- [ ] `curl http://127.0.0.1:8080/cgi/hello.py?input=Toto` → idem (GET CGI)
- [ ] `curl http://127.0.0.1:8080/error.py` → 500 stylé (CGI plante)
- [ ] `curl http://127.0.0.1:8080/cgi/sleep.py` (CGI > 5s) → 504 stylé
- [ ] `curl --http1.0 http://127.0.0.1:8080/` → 200 (pas 505)
- [ ] `curl -H 'Transfer-Encoding: chunked' --data-binary @gros.txt http://127.0.0.1:8080/upload/` → 200
- [ ] Stress siege/wrk : 100 % availability sur 30 s
- [ ] Lecture du README.md par un peer non-prévenu : il sait compiler + tester en < 2 min
- [ ] Aucun fd leak (`make val`)
- [ ] Aucun `cout` de debug dans la sortie standard
