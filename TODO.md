Analyse basée sur le sujet officiel
✅ Ce qui est OBLIGATOIRE et déjà fait :
✅ Server non-bloquant avec poll()
✅ Configuration file parsing
✅ Multiple ports
✅ GET method
✅ POST method (upload files)
✅ CGI support (Python, PHP)
✅ Static website serving
✅ Default error pages
🔴 Ce qui est OBLIGATOIRE et manque :
1. DELETE method - CRITIQUE
Le sujet dit explicitement : "You need at least the GET, POST, and DELETE methods."

Actuellement reconnu mais non implémenté dans Client.cpp
2. Location routing - CRITIQUE
Le sujet demande "Specify rules or configurations on a URL/route" :

✅ Parsé dans config
❌ Pas utilisé dans le code - CheckUrl() ignore complètement _config.locations
Doit gérer :

✅ allowed_methods (parsé, pas vérifié)
❌ HTTP redirection
✅ root pour la location (parsé, pas appliqué)
✅ Directory listing (autoindex, implémenté mais pas par location)
✅ Default file (index, implémenté mais pas par location)
✅ Upload location (POST fonctionne mais chemin hardcodé upload)
3. client_max_body_size validation - CRITIQUE
Le sujet demande "Set the maximum allowed size for client request bodies"

✅ Parsé
❌ Jamais vérifié - un client peut envoyer 10GB sans erreur
4. HTTP redirections - OBLIGATOIRE
Le sujet liste explicitement : "HTTP redirection" dans les configurations de route

❌ Pas implémenté du tout
Doit supporter 301/302
5. CGI - Upload path configuration - IMPORTANT
Le sujet : "Uploading files from the clients to the server is authorized, and storage location is provided"

Actuellement hardcodé upload dans Request.cpp:232
Doit être configurable via la location
6. CGI - POST body - IMPORTANT
Le sujet : "The full request and arguments provided by the client must be available to the CGI"

TODO ligne 118 de CgiHandler.cpp:118 non résolu
7. CGI - Chunked requests - IMPORTANT
Le sujet : "for chunked requests, your server needs to un-chunk them"

❌ Pas géré actuellement
8. Request timeout - CRITIQUE
Le sujet : "A request to your server should never hang indefinitely"

❌ Aucun timeout implémenté
📋 Priorités par ordre d'importance :
BLOQUANT (sans ça le projet échoue) :

Location routing + application des règles
DELETE method
allowed_methods validation
client_max_body_size validation
HTTP redirections
Request timeouts
IMPORTANT (probablement testé) :
7. CGI POST body
8. Upload path configurable
9. Stress tests / robustesse

BONUS si temps :
10. Chunked transfer encoding
11. CGI path relatif correct


Faire les redirection dans le config.conf (return 301 {option})
check le "Accept" dans le GET pour savoir ce que l'on a le droit de renvoyer (Error 406)
Faire les locations
Mettre une redirection de "/" si jamais on est dans un dossier
faire le timeout
faire un meilleur parser de requete