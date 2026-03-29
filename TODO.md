# TODO — webserv

## ✅ Fait

- Server non-bloquant avec poll()
- Configuration file parsing
- Multiple ports / virtual servers
- GET, POST, DELETE methods
- Upload de fichiers (POST multipart)
- Serving de fichiers statiques
- Pages d'erreur par défaut + configurables
- allowed_methods par location (vérifié dans generate())
- client_max_body_size (parsé + validé, throw 413)
- Request timeout (isTimeout + onTimeout → 408)
- autoindex (AutoIndex généré)
- Redirection 301 automatique quand dossier sans `/` final

---

## 🔴 Ce qui manque

### 1. HTTP redirections depuis le config — `return 301/302`
Le sujet demande de pouvoir écrire dans une location :
```
return 301 https://example.com;
```
- Le champ n'existe pas encore dans `LocationConfig` (`// TODO put the "return"`)
- À faire : ajouter `redirect_code` + `redirect_url` dans `LocationConfig`, parser dans `LocationParser`, appliquer dans `generate()` avant `checkUrl`

### 2. Location routing — `root` / `index` / `autoindex` ignorés
`checkUrl` utilise toujours `config.root` et `config.index` du serveur global.
Les champs `root`, `index`, `autoindex` sont parsés dans `LocationConfig` mais jamais appliqués.
- Si une location a son propre `root`, il faut l'utiliser à la place de `config.root`
- Même chose pour `index` et `autoindex`

### 3. Upload path par location
Dans `Request.cpp`, le chemin d'upload est calculé avec `config.root + _path`.
Il devrait utiliser le `root` de la location active si elle en a un.

---

## 📋 Ordre de priorité

| # | Tâche | Impact |
|---|-------|--------|
| 1 | `return 301/302` dans les locations | Critique — testé à l'éval |
| 2 | `root`/`index`/`autoindex` par location | Critique |
| 3 | Upload path par location | Moyen |
| 4 | Retirer les logs de debug | Propreté / éval |

---

Ne pas avoir de doublons dans la requette