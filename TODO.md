

## 🔥 BLOQUANTS — note 0 / défense impossible

### B1. `Connection: close` mis à part, **boucle `send()` non guidée par poll()**
Fichier : `srcs/Management/srcs/Client.cpp` (lignes 113-119, 129-135) et `Client::onTimeout`.
Sujet : *« You must never do a read or a write operation without going through poll() (or equivalent). »*
```cpp
while (sent < full.size()) {
    const ssize_t n = send(_fd, full.c_str() + sent, full.size() - sent, 0);
    if (n <= 0) break;
    sent += n;
}
```
→ `send()` est rappelé en boucle sans repasser par `poll()`. Sur grosse réponse + socket non-bloquante, EAGAIN fait tomber dans `break;` et la réponse est tronquée silencieusement.
**À faire** : conserver le buffer de réponse + offset dans `Client`, et n'envoyer **qu'un seul** appel `send()` par tour de POLLOUT. Quand `sent < full.size()`, garder `_events = POLLOUT`. Quand tout est écrit, set `_closedStatus = true`.

### B3. Pas de support du **Transfer-Encoding: chunked**
Fichier : `srcs/RequestResponse/srcs/Request.cpp::isValid` + `checkRequest`.
Sujet : *« for chunked requests, your server needs to un-chunk them, the CGI will expect EOF as the end of the body. »*
Vérifié : `POST` chunked → `400 Bad Request` (rejet sur absence de Content-Length, ligne 149-150).
**À faire** : détecter `Transfer-Encoding: chunked` dans les headers, désactiver le check Content-Length, lire les chunks `<size>\r\n<data>\r\n…0\r\n\r\n`, reconstituer le body complet avant `parse()` et le passer au CGI.

### B4. Réponse CGI **sans status line HTTP**
Fichier : `srcs/RequestResponse/includes/Response.hpp` (ligne 43-47) + `Client::setCgiOutput`.
```cpp
std::string getFullResponse() {
    if (_isCgi) return _body;       // ⚠️ pas de "HTTP/1.1 200 OK\r\n"
    return _header + _body;
}
```
Le serveur balance la sortie brute du CGI au client. Le seul rempart, c'est que le script `website/cgi/hello.py` triche en imprimant lui-même `print("HTTP/1.1 200 OK")`, ce qui n'est PAS du CGI standard. Un CGI tiers (`php-cgi`, n'importe quel script écrit selon RFC 3875) ne sortira jamais la status line → réponse cassée.
**À faire** :
1. Parser la sortie CGI : lire les en-têtes jusqu'à la ligne vide.
2. Si en-tête `Status:` trouvé, l'utiliser ; sinon `200 OK`.
3. Construire la status line `HTTP/1.1 <code> <text>\r\n`, fusionner avec les headers du CGI (ajouter `Content-Length` si absent), puis le body.
4. Retirer le `print("HTTP/1.1 …")` de `website/cgi/hello.py`.

### B5. Pas de **README.md**
Sujet, chapitre V : `README.md` à la racine, rédigé en anglais, avec :
- 1ère ligne en italique : *This project has been created as part of the 42 curriculum by <login1>[, <login2>…].*
- section **Description**
- section **Instructions** (compilation / exec)
- section **Resources** (références + utilisation IA)

**À faire** : créer le fichier (les logins visibles dans les commits sont `mvachon` / `nofanizz`).

---

## 🔴 GROS RISQUES À LA DÉFENSE

### R1. `static bool _finalAutoIndex` dans `Response.cpp` (ligne 8)
Variable **globale partagée** entre tous les clients. Deux requêtes simultanées sur des URL différentes → état corrompu. **À transformer en membre `_finalAutoIndex` de la classe `Response`.**

### R2. CGI ne fait pas de `chdir()` avant `execve`
Sujet : *« The CGI should be run in the correct directory for relative path file access. »*
`CgiManager::start()` exec depuis le CWD du serveur. Un `include` ou `open("data.txt")` dans un script échouera.
**À faire** : `chdir(dirname(_scriptPath))` dans le child avant `execve`.

### R3. `if (_version != "HTTP/1.1") throw Http505Exception();`
Vérifié : `GET / HTTP/1.0` → `505`. Sujet : *« HTTP 1.0 is suggested as a reference point »* — refuser HTTP/1.0 c'est aller à l'inverse. Telnet, certains clients de stress, et la défense potentielle utilisent 1.0.
**À faire** : accepter `HTTP/1.0` ET `HTTP/1.1`.

### R4. `errno` consulté après `std::remove`
`Response.cpp:259` : `if (errno == EACCES)`. `remove()` n'est pas dans la liste autorisée, mais surtout consulter `errno` après reste mal vu.
**À faire** : remplacer `std::remove` par `unlink()`/`rmdir()` (interdits aussi… cf. R5) **OU** utiliser `access(path, W_OK)` avant `std::remove` pour distinguer 403 vs 404 sans toucher `errno`.

### R5. Fonctions hors-liste utilisées
- `std::remove` (cstdio) — pour DELETE
- `std::time(NULL)` (ctime) — partout pour les timeouts
- `perror` — `WebServer.cpp:70`, `Server.cpp:78`, `CgiManager.cpp:116`
Aucune n'est dans la liste **External Function** du sujet. À discuter avec un correcteur strict. Toléré dans 99 % des défenses, mais peut être bloqué par certains.
**À faire (si évaluateur strict)** : remplacer `std::time` par `gettimeofday`… qui n'est pas non plus dans la liste 🙃 → la stratégie habituelle est de noter dans le README qu'on les considère comme libc standard.

### R6. UB dans `main.cpp` (lignes 28-33)
```cpp
if (!av[1] && ac != 2) { … }
```
Si `ac == 1`, `av[1]` est NULL — déréférencé/utilisé en bool, c'est OK ici, mais la condition est **bancale** : `ac == 0 && ac != 2` est faux donc on continue. Si `ac >= 3`, `av[1]` non-null → entre dans le `if` à tort ? Non, `!av[1]` est faux, le `if` ne déclenche pas. Mais si quelqu'un lance `./webserv`, on tombe ensuite dans `config.setFile(av[1])` avec av[1]=NULL → crash.
**À faire** : `if (ac != 2) { std::cerr << "Usage: ./webserv <config>\n"; return 1; }`.

### R7. `BUFFER_SIZE 1000` dans `recv()`
Volontairement petit pour tester le streaming. OK fonctionnellement, mais pour des gros uploads (`tek.mp4` = 1.4 Mo), l'analyse de `request` (concat std::string + scan `find("\r\n\r\n")` à chaque tour) explose en O(n²). Pas un blocage mais **stress test va ramer**.
**À faire (si temps)** : passer à 4096 octets et ne `find("\r\n\r\n")` qu'une fois.

### R8. Listening socket pas en non-blocking
`Server::Server` ne met pas `_fd` en `O_NONBLOCK`. `accept()` est protégé par `poll(POLLIN)`, donc en pratique non-bloquant, mais la tradition 42 est de tout flagger O_NONBLOCK.
**À faire** : `fcntl(_fd, F_SETFL, O_NONBLOCK)` après `socket()`.

---

## 🟠 CONFIG FILE — incohérences

### C1. `config.conf` actuel **mal formé**
Lignes 10-13 :
```
location /upload/
    autoindex on;
    allow_methods GET POST DELETE;
    upload_dir website/upload/;
```
→ pas d'accolade ouvrante ni fermante. Le bloc se mélange avec le suivant. Le parser tolère ?? Surtout, à la défense, c'est le 1er fichier que le correcteur lance. **À corriger**.

Ligne 23 : `return 301 /upload` — **manque `;`**. Le parser strip le `;` final si présent, sinon n'avertit pas.

### C2. CGI **non configurable** dans le fichier
`CgiManager::isCgi()` détecte en dur `.py` et `.php`. Sujet : *« Execution of CGI, based on file extension (for example .php). »* — c'est une directive qu'on s'attend à trouver dans la config (mapping extension → interpréteur).
**À faire** :
- Ajouter `cgi_pass .py /usr/bin/python3;` (ou similaire) dans `LocationConfig`.
- `CgiManager::start` exec `execve(interpreter, {interpreter, scriptPath, NULL}, env)` au lieu d'exec le script direct (PHP-CGI ne se lance pas comme ça).

### C3. `host` ne gère que les IP
`Server::createSocketAdress` utilise `getaddrinfo` mais `_servAddr` est figé en AF_INET et copie `sin_addr`. `host localhost;` ou `host 0.0.0.0;` doit fonctionner — c'est le cas, OK. Mais pas testé en IPv6 (de toute façon, hors scope).

### C4. Host duplication sur même port (virtual hosts)
Bien que le sujet mette les virtual hosts hors-scope, plusieurs `server { port 8080 host X }` `server { port 8080 host Y }` doivent au moins ne pas planter. `validateDuplicatePorts` rejette les paires (port,host) identiques mais autorise (8080,A) + (8080,B), ce qui fera échouer le `bind()` de la 2ᵉ instance.
**À faire (optionnel)** : ouvrir un seul socket par port, dispatcher sur le bon `ServerConfig` selon le `Host:` header. Bonus, pas critique.

---

## 🟡 PARSING REQUEST — robustesse

### P1. `parsePostMethod` ne gère que `multipart/form-data`
Sans boundary (`_webKitForm` vide), `split` retourne 1 part, la boucle `parts.size() - 1` ne déclenche pas, le body est silencieusement ignoré. POST `application/json` ou `application/x-www-form-urlencoded` → 200 vide.
**À faire** : si pas de multipart, stocker le body brut dans `_body` (utile pour CGI cf. B2).

### P2. Pas de validation du nom de fichier uploadé
`parsePostMethod` ligne 337 strip le `/` final mais pas les `..`. `filename="../../../etc/passwd"` → `passwd` après strip de `/`, OK pour celui-là, mais `filename="..%2Fpouet"` non décodé peut poser souci (ici `%2F` n'est pas remis en `/` car le filename n'est pas urldecodé). À vérifier en pratique.
**À faire** : reject filename avec `..` ou commençant par `/` après normalisation.

### P3. Headers parsing trop strict
`separateHeaders` (Request.cpp:220) :
```cpp
if (value.empty() || value[0] != ' ' || (value.size() > 1 && value[1] == ' '))
    throw Http400Exception();
```
→ refuse `Header:value` (sans espace), `Header:  value` (double espace), tabulations. La RFC autorise tout ça. NGINX accepte. **Risque de faux 400** sur clients atypiques.

### P4. `_path.size() > 2048` → 414
OK mais sujet ne demande pas cette limite. Inoffensif.

### P5. Limites checkRequest manquantes
- Pas de limite sur le nombre de headers
- Pas de limite sur la taille totale des headers (DoS slowloris : envoyer 1 octet/sec d'en-têtes infinies, le buffer `_rawRequest` grossit)
- Pas de limite sur la taille de la request line

### P6. `Request::isValid` lit `_method` une fois et ne le re-vérifie plus
La méthode est captée au premier morceau reçu (`recv` partial). Si l'attaquant envoie `GET ` puis change après, OK ce n'est pas un vrai problème. Note seulement.

---

## 🟢 BONNES CHOSES — déjà OK

- Makefile : règles `all/clean/fclean/re`, `-Wall -Wextra -Werror -std=c++98`, pas de relinking inutile (vérifié : `make` après `make` fait `Nothing to be done`). Bonus : `-MMD -MP` pour les deps.
- `poll()` unique dans `WebServer::run()` couvre listen + clients + pipes CGI. ✅
- `accept` retourne un fd mis en `O_NONBLOCK` via `fcntl` (Server.cpp:81). ✅
- Pages d'erreur en dur (400, 403, 404, 405, 408, 411, 413, 414, 500, 504, 505) ✅
- `error_page` configurable + override par config (`Response::getErrorPageContent`). ✅
- `client_max_body_size` parsé + appliqué (413 levé). ✅
- `autoindex` + `index` + `root` par location (à finaliser, cf. R1, voir aussi `checkUrl`). ✅
- Redirection auto `/dossier` → `/dossier/` (301). ✅
- Multi-server / multi-ports (validé par config 8080+6565). ✅
- Stress 50 GET concurrents : 50/50 200 ✅, serveur survit ✅.
- Tester Python `tester/tester.py` présent. ✅
- DELETE 404 propre quand la cible n'existe pas. ✅

---

## 📋 ORDRE D'ATTAQUE RECOMMANDÉ

| # | Tâche | Fichier principal | Priorité |
|---|-------|-------------------|----------|
| 1 | **Boucle send sans poll** (B1) | `Client.cpp` | 🔥 |
| 2 | **Body POST → CGI stdin** (B2) | `CgiManager.cpp`, `Request.cpp` | 🔥 |
| 3 | **Status line manquante CGI** (B4) | `Response.hpp`, `Client.cpp` | 🔥 |
| 4 | **`README.md`** (B5) | racine | 🔥 |
| 5 | **Chunked transfer-encoding** (B3) | `Request.cpp` | 🔥 |
| 6 | **HTTP/1.0 accepté** (R3) | `Request.cpp::checkRequest` | 🔴 |
| 7 | **`_finalAutoIndex` static → membre** (R1) | `Response.cpp/hpp` | 🔴 |
| 8 | **`chdir` dans le child CGI** (R2) | `CgiManager.cpp::start` | 🔴 |
| 9 | **`config.conf` mal formé** (C1) | `config.conf` | 🔴 |
| 10 | **CGI configurable par extension** (C2) | `Config.hpp`, parser, `CgiManager` | 🟠 |
| 11 | `errno` après remove (R4) | `Response.cpp` | 🟠 |
| 12 | `main.cpp` argument check (R6) | `main.cpp` | 🟠 |
| 13 | POST non-multipart → garder body (P1) | `Request.cpp` | 🟠 |
| 14 | Listening socket O_NONBLOCK (R8) | `Server.cpp` | 🟡 |
| 15 | Filename upload : reject `..` (P2) | `Request.cpp::parsePostMethod` | 🟡 |
| 16 | Headers parsing tolérant (P3) | `Request.cpp::separateHeaders` | 🟡 |
| 17 | Retirer `std::cout << _rawRequest` debug | `Client.cpp:60-61`, `DEBUG_PrintConfig.cpp` | 🟢 |
| 18 | Variables d'env CGI étendues (PATH_INFO, HTTP_*, REMOTE_ADDR…) | `CgiManager::buildEnv` | 🟢 |
| 19 | Bonus : cookies / sessions | nouveau module | 🔵 |
| 20 | Bonus : multi-CGI types | déjà partiellement faisable via #10 | 🔵 |

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
