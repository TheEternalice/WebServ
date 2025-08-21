# WebServ
WebServer in cpp
# WebServ

## WebServer in cpp by :
- [ade-rese] github ->	[TheEternalice]
- [lde-merc] github ->	[LucLgdm]
- [gpichon]	 github ->	[Gebzof]

## But d'un webserveur :

### Definition :

src -> https://www.ovhcloud.com/fr/learn/what-is-web-server/
Un serveur web est un système informatique qui repond a une requete afin de consulter un page web.

src -> https://developer.mozilla.org/fr/docs/Learn_web_development/Howto/Web_mechanics/What_is_a_web_server

c'est un ordinateur qui stock des fichier qui compose un site web (HTML Css, JavaScript, images, ...).

### Fonctionnement

pour resumer le fonctionnement -> le client fait une requette pour afficher les ressources, le serveur luis heberge ces fichier est s'occupe de comprendre la requete pour luis envoyer les bonne ressources.
En plus de ca, le protocole HTTP (HyperText Transfert Protocol ) intervient -> il donne le format de conversation entre le client et le serveur.

ca ressemble donc a ca :

1. Le client établit une connexion TCP avec le serveur (généralement sur le port 80 pour HTTP, 443 pour HTTPS).
2. Le client envoie une requête HTTP.
3. Le serveur traite la requête.
4. Le serveur renvoie une réponse HTTP.
5. La connexion est fermée (ou réutilisée pour de nouvelles requêtes).

## notion a connaitre : 

### nginx (engine x) -> serveur web :
	src -> https://nginx.org/en/docs/
	Nginx n'est pas juste un serveur web. C'est une pièce maîtresse de l'infrastructure web moderne, un couteau suisse de la performance et de la fiabilité qui agit comme un trafic manager ultra-efficace devant vos applications.

### configuration files :
	https://doc.ubuntu-fr.org/nginx -> 3.5 Graphique
	le fichier ce configuration du serveur il comporte plusieurs choses indispensble comme le nom du serveur ou de la page(index), les logs, le repertoire de la page, le port.
	pour plus d'info -> voir dans le dossier conf_file

### methodes HTTP :

1. Get	-> https://developer.mozilla.org/fr/docs/Web/HTTP/Reference/Methods/GET.
		Lie ou recupere les ressources et retourne les informations demandées des entités spécifiées.
2. HEAD	-> https://developer.mozilla.org/fr/docs/Web/HTTP/Reference/Methods/HEAD
		demande les en-tetes sert pour economiser des ressources si elle a un corps, elle doit etre ignoree
3. POST	-> https://developer.mozilla.org/fr/docs/Web/HTTP/Reference/Methods/POST
		Envoie les donnees au serveur -> envoyer souvent par form HTML

### protocole http :
	src -> https://www.rfc-editor.org/rfc/rfc2616.html#page-51
	HTTP est le langage universel qui permet à votre navigateur de discuter avec les serveurs du monde entier pour fetch le contenu web. C'est un protocole simple, textuel et sans état, construit autour du cycle requête-réponse, et qui a évolué (HTTPS, HTTP/2) pour devenir plus rapide et sécurisé.

### CGI (Common Gateway Interface) :
		src -> https://fr.wikipedia.org/wiki/Common_Gateway_Interface
		au lieu du HTML nous pouvons executer du code sur le serveur, La CGI permet la transmission la requette qu serveur et comment la recuperer.
		
## plan pour le realiser :

### 1. Parsing du fichier de configuration

	Lire le fichier .conf donné en argument.
	Stocker les informations dans des structures :
		ServerConfig : ports (listen), host, root, index, error_page, client_max_body_size, etc.
		LocationConfig : path, methods autorisées, règles CGI, redirections, upload store, etc.
	Supporter plusieurs serveurs virtuels (plusieurs blocs server).
	Associer Host: des requêtes HTTP avec le bon serveur.

### 2. Mise en place du serveur

	Créer un socket par listen.
	bind() + listen().
	Passer en mode non-bloquant (fcntl).
	Ajouter chaque socket d’écoute dans poll() (ou select()).
	Préparer les réponses d’erreur par défaut (400, 404, 405, 500, etc.).

### 3. Boucle d’événements

	poll() surveille :
		nouveaux clients sur sockets d’écoute,
		sockets clients pour lecture/écriture.
	Quand un client se connecte : accept() et ajouter au poll.
	Stocker un objet Client par connexion avec :
		fd,
		buffer d’entrée,
		état de parsing,
		buffer de sortie,
		last_activity (timestamp).

### 4. Parsing des requêtes HTTP

	Lire depuis le socket client (non bloquant).
	Parser la requête :
		Request line : GET /path HTTP/1.1
		Headers : Host:, Content-Length, Transfer-Encoding, Connection, Cookie:…
		Body (si Content-Length ou chunked).
	Vérifier que la méthode est autorisée (sinon → 405 Method Not Allowed).
	Vérifier la taille max du body (413 Payload Too Large si > client_max_body_size).

### 5. Routing & Mapping

	Associer l’URI demandée avec la bonne LocationConfig.
	Déterminer la ressource cible :
		fichier statique (root + uri),
		index (si répertoire),
		CGI (si configuré),
		redirection (si configurée).
	Vérifier les permissions (méthodes autorisées).

### 6. Gestion des méthodes HTTP

	GET : lire et envoyer fichier.
	HEAD : identique à GET mais sans body.
	POST :
		soit écrire dans un fichier (upload),
		soit envoyer en entrée à un CGI.
	DELETE : supprimer fichier si autorisé.

### 7. Gestion des CGI

	Déterminer si la requête doit passer par un script (ex: /cgi-bin/script.py).
	Créer un pipe pour rediriger la sortie du script.
	fork() + execve() du script, en passant les variables d’environnement CGI (REQUEST_METHOD, QUERY_STRING, CONTENT_LENGTH, etc.).
	Lire la sortie du script via le pipe.
	Construire une réponse HTTP avec cette sortie.

### 8. Réponses HTTP

	Générer une réponse bien formée :
		HTTP/1.1 200 OK\r\n
		Content-Type: text/html\r\n
		Content-Length: 123\r\n
		\r\n
		[body]
	Gérer les erreurs personnalisées (error_page 404 /404.html).
	Gérer redirects (301, 302).
	Gérer cookies :
	Set-Cookie dans réponse,
	Cookie dans requête.

### 9. Gestion des connexions

	Timeout : si time(NULL) - last_activity > client_timeout, fermer connexion.
	Respecter le header Connection:
	keep-alive → garder la connexion ouverte pour plusieurs requêtes.
	close (ou HTTP/1.0) → fermer après la réponse.