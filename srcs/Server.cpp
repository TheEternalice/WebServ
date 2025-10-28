/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-rese <ade-rese@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 15:47:12 by lde-merc          #+#    #+#             */
/*   Updated: 2025/10/28 11:07:47 by ade-rese         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
volatile bool g_running(true);
Server* g_server = NULL;

// std::map<int, Reponse> Server::_static_responses;
std::map<std::string, std::string> Server::_extensionsToType;

void handle_sigint(int signum) {
	(void)signum;
	std::cout << "\nSignal reçu, arrêt du serveur..." << std::endl;
	g_running = false;
	g_server->cleanup();
}

void Server::cleanup() {
	std::cout << "Nettoyage des connexions..." << std::endl;

	for (size_t i = 0; i < _fds.size(); ++i) {
		int fd = _fds[i].fd;
		// Si ce n’est pas un socket serveur
		if (!isServerSocket(fd)) {
			std::map<int, Client*>::iterator it = _socketToClient.find(fd);
			if (it != _socketToClient.end()) {
				if (it->second) {
					delete it->second; // libère le client
					it->second = NULL;
				}
				close(fd); // ferme le descripteur
			}
		} else {
			close(fd);
		}
	}
	_socketToClient.clear();
	_fds.clear();
	std::cout << "Serveur arrêté proprement." << std::endl;
}


// Constructeur
Server::Server() {
	std::ifstream file("mime.types");
	if (!file.is_open())
		throw std::runtime_error("Could not open mime.types");

	std::string line;
	while (std::getline(file, line)) {
		// Ignore comments et lignes vides
		if (line.empty() || line[0] == '#')
			continue;

		std::istringstream iss(line);
		std::string mime_type;
		if (!(iss >> mime_type))
			continue;

		std::string ext;
		while (iss >> ext) {
			if (ext[0] != '.')
				ext = "." + ext; // normaliser avec un point
			_extensionsToType[ext] = mime_type;
		}
	}
	file.close();
}

Server::~Server() {}

Server::Server(const Server &other) { *this = other; }

Server &Server::operator=(const Server &other) {
	if (this != &other) {
		// copy attributes here
		_fds = other._fds;
		_sockets = other._sockets;
		_extensionsToType = other._extensionsToType;
	}
	return *this;
}

void Server::copy_socket(std::vector<ServerSocket> other) {
	this->_sockets = other;
}

std::vector<ServerSocket> Server::get_Socket() {
	return _sockets;
}

void Server::display_Serv() {
	std::string msg = "";
	for (size_t i = 0; i < _sockets.size(); i++){
		// std::cout << "------- Server " << i + 1 << " -------" << std::endl;
		// std::cout << "Host-> ";
		// if (!_sockets[i]._host.empty())
		// 	std::cout << _sockets[i]._host << std::endl;
		// else
		// 	msg = "No host found";
		std::cout << "Port-> ";
		if (_sockets[i]._port)
			std::cout << _sockets[i]._port << std::endl;
		else
			msg = "No port found";
		std::cout << "Server name-> ";
		if (!_sockets[i]._server_name.empty())
			std::cout << _sockets[i]._server_name << std::endl;
		else
			msg = "No server name found";
		// std::cout << "Error pages : " << std::endl;
		// if (!_sockets[i]._error_pages.empty()) {
		// 	for (size_t j = 0; j < 600; j++){
		// 		if (_sockets[i]._error_pages[j] != "")
		// 			std::cout << "   _ " << _sockets[i]._error_pages[j] << std::endl;
		// 	}
		// }
		// std::cout << std::endl;
		// std::cout << "Max body size : ";
		// if (!_sockets[i]._max_body_size)
		// 	std::cout << _sockets[i]._max_body_size << std::endl;
		// std::cout << "Path : ";
		// if (!_sockets[i]._path.empty())
		// 	std::cout << _sockets[i]._path << std::endl;
		// std::cout << "Index : ";
		// if (!_sockets[i]._index.empty())
		// 	std::cout << _sockets[i]._index << std::endl;
		// std::cout << "Root : ";
		// if (!_sockets[i]._root.empty())
		// 	std::cout << _sockets[i]._root << std::endl;
		// std::cout << "Auto index : ";
		// if (_sockets[i]._autoIndex)
		// 	std::cout << _sockets[i]._autoIndex << std::endl;
		// std::cout << "Allowed Methods :" << std::endl;
		// if (!_sockets[i]._allowedMethods.empty()) {
		// 	std::map<std::string, int>::const_iterator it;
		// 	for (it = _sockets[i]._allowedMethods.begin(); it != _sockets[i]._allowedMethods.end(); ++it) {
		// 		std::cout << "  " << it->first << " : " << it->second << std::endl;
		// 	}
		// }
		// std::cout << std::endl << "CGI Extensions : " << std::endl;
		// if (!_sockets[i]._cgiExtensions.empty()) {
		// 	for (size_t l = 0; l < _sockets[i]._cgiExtensions.size(); l++)
		// 		std::cout << "   " << l << " : " << _sockets[i]._cgiExtensions[l] << std::endl;
		// }
		// std::cout << "Path : " << std::endl;
		// if (!_sockets[i]._returnPath.empty())
		// 	std::cout << _sockets[i]._returnPath << std::endl;
		// std::cout << "Upload dir : " << std::endl;
		// if (!_sockets[i]._upload_dir.empty())
		// 	std::cout << _sockets[i]._upload_dir << std::endl;
		std::cout << "------------------------" << std::endl;
		if (!msg.empty())
			throw(std::runtime_error(msg));
	}
}

std::string Server::get_content_type(const std::string &path) {
	std::string::size_type dot = path.rfind('.');
	if (dot == std::string::npos)
		return "application/octet-stream";

	std::string ext = path.substr(dot);
	std::map<std::string, std::string>::const_iterator it = _extensionsToType.find(ext);

	if (it != _extensionsToType.end())
		return it->second;
	else
		return "application/octet-stream";
}

void Server::init() {
	for (int i = 0; i < (int)_sockets.size(); i++) {
		_sockets[i].fd = socket(AF_INET, SOCK_STREAM, 0);
		if (_sockets[i].fd < 0)
			throw std::runtime_error("Failed to create socket");
		_listeningSockets[_sockets[i].fd] = _sockets[i];
		// Set the socket to non-blocking mode
		// This allows the server to handle multiple clients without blocking
		int flags = fcntl(_sockets[i].fd, F_GETFL, 0);
		if (flags == -1) {
			std::cerr << "fcntl F_GETFL" << std::endl;
			exit(1);
		}
		if (fcntl(_sockets[i].fd, F_SETFL, flags | O_NONBLOCK) == -1)
			throw std::runtime_error("fcntl F_SETFL");

		memset(&_sockets[i].address, 0, sizeof(_sockets[i].address));
		_sockets[i].address.sin_family = AF_INET;
		_sockets[i].address.sin_addr.s_addr = INADDR_ANY;
		_sockets[i].address.sin_port = htons(_sockets[i]._port);

		int opt = 1;
		if (setsockopt(_sockets[i].fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
			close(_sockets[i].fd);
			throw std::runtime_error("setsockopt failed");
		}

		if (bind(_sockets[i].fd, (struct sockaddr *)&_sockets[i].address, sizeof(_sockets[i].address)) < 0) {
			for (int j = 0; j <= i; j++)
				close(_sockets[j].fd);
			throw std::runtime_error("Failed to bind socket");
		}

		if (listen(_sockets[i].fd, 5) < 0) {
			for (int j = 0; j <= i; j++)
				close(_sockets[j].fd);
			throw std::runtime_error("Failed to listen on socket");
		}
	}
}

void Server::run() {
	for (size_t i = 0; i < _sockets.size(); i++) {
		struct pollfd pfd;
		pfd.fd = _sockets[i].fd;
		pfd.events = POLLIN;
		pfd.revents = 0;
		_fds.push_back(pfd);
	}

	while (g_running) {
		int ret = poll(_fds.data(), _fds.size(), 100);
		if (ret < 0) {
			if (errno == EINTR)
				continue;
			std::cerr << "Poll error" << std::endl;
			exit(EXIT_FAILURE);
		}
		if (ret == 0) continue;

		for (size_t i = 0; i < _fds.size(); ++i) {
			int fd = _fds[i].fd;
			int re = _fds[i].revents;

			if (re & POLLIN) {
				if (isServerSocket(fd)) {
					accept_client(fd);
				} else {
					Client *cl = _socketToClient[fd];
					if (!cl) continue;
					cl->readFromSocket();

					if (cl->tryParseRequest()) {
						handle_request(*cl);
						_fds[i].events = POLLOUT;
					}
				}
			}

			if (re & POLLOUT) {
				std::map<int, Client*>::iterator it = _socketToClient.find(fd);
				if (it == _socketToClient.end())
					continue;

				Client *cl = it->second;
				cl->writeToSocket();

				if (cl->outputEmpty()) {
					if (cl->getReponse().isKeepAlive()) {
						cl->resetForNextRequest(); // ← ici le point clé
						_fds[i].events = POLLIN;
					} else {
						disconnectClient(fd);
					}
				}
			}
		}
	}
}

bool Server::isServerSocket(int fd) {
	for (size_t i = 0; i < _sockets.size(); i++)
		if (fd == _sockets[i].fd)
			return true;
	return false;
}

void Server::accept_client(int fd) {
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);

	int client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_len);
	if (client_fd < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return; // Rien à accepter
		std::cerr << "accept() failed on fd " << fd << ": "
				  << strerror(errno) << std::endl;
		return;
	}

	// Rend le client non bloquant
	int flags = fcntl(client_fd, F_GETFL, 0);
	if (flags == -1 || fcntl(client_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
		std::cerr << "fcntl() failed: " << strerror(errno) << std::endl;
		close(client_fd);
		return;
	}

	ServerSocket &server = _listeningSockets[fd];

    // Créer un nouveau client et lier au serveur
    Client* client = new Client(client_fd);
    _socketToClient[client_fd] = client;
    _clientToSocket[client_fd] = server;
	
	if (_listeningSockets.count(fd) == 0) {
        std::cerr << "Unknown listening socket fd: " << fd << std::endl;
        close(client_fd);
        return;
    }


	// Ajouter à poll()
	struct pollfd pfd;
	pfd.fd = client_fd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_fds.push_back(pfd);

	// std::cout << "New client connected on port " << server._port
	// 		  << " (fd=" << client_fd << ")" << std::endl;
}

void Server::handle_request(Client &client) {
	const std::string method = client.getRequest().get_method();
	Reponse res;
	ServerSocket* server = &_clientToSocket[client.get_fd()];
	try {
		if (!is_method_allowed(method, client)) {
			res = server->_autoResponse[405];
		} else if (method == "GET") {
			res = client.getRequest().handle_get();
		} else if (method == "POST") {
			Request req = client.getRequest();
			ServerSocket socket = _clientToSocket[client.get_fd()];
			std::string contentType = req.getHeader("Content-Type");
			if (contentType.find("multipart/form-data") != std::string::npos) {
				if (handleFileUpload(req.get_body(), contentType, socket._upload_dir)) {
					res = Reponse(client.getRequest().get_url());
					res.set_header("Content-Type", "text/html");
					std::string boby ="<p style='color:green;'>Upload réussi !</p>";
					res.set_body(boby);
					std::ostringstream oss_len;
					oss_len << boby.size();
					res.set_header("Content-Length", oss_len.str());
				}
			}else {
				res = client.getRequest().handle_post();
			}
		}else if (method == "DELETE") {
			res = client.getRequest().handle_delete();
		} else {
			res = server->_autoResponse[400];
		}
	} catch (std::runtime_error &e) {
		res = server->_autoResponse[404];
	}

	if (client.getRequest().getHeader("Connection") == "keep-alive") {
		res.set_header("Connection", "keep-alive");
		res.set_header("Keep-Alive", "timeout=20, max=100");
	} else {
		res.set_header("Connection", "close");
	}

	client.setResponse(res);

}

// C'est l'idee qui compte !
bool Server::is_method_allowed(const std::string &method, Client &client) {
	ServerSocket *server = &_clientToSocket[client.get_fd()];
	int nummethode = 0;
	switch (method[0]) {
		case 'G':
			nummethode = 1;
			break;
		case 'P':
			nummethode = 2;
			break;
		case 'D':
			nummethode = 4;
			break;
		default:
			return false;
	}
	std::string tmpLocation = client.getRequest().get_url();
	size_t pos = 0;	
	pos = tmpLocation.find_last_of('/');
	if (pos != std::string::npos && pos > 0)
		tmpLocation = tmpLocation.substr(0, pos);
	else
		tmpLocation = "/";
	if ((server->_allowedMethods[tmpLocation] & nummethode) != 0)
		return true;
	return false;
}

void Server::disconnectClient(int fd) {
	std::map<int, Client*>::iterator it = _socketToClient.find(fd);
	if (it != _socketToClient.end()) {
		if (it->second)
			delete it->second;
		_socketToClient.erase(it);
	}
	close(fd);
	removeFdFromPoll(fd);
	std::cout << "Client (fd=" << fd << ") disconnected." << std::endl;
}

void Server::removeFdFromPoll(int fd) {
	for (std::vector<struct pollfd>::iterator it = _fds.begin(); it != _fds.end(); ++it) {
		if (it->fd == fd) {
			_fds.erase(it);
			return;
		}
	}
}

bool Server::handleFileUpload(const std::string& body, const std::string& contentType, const std::string& uploadDir) {
	// Extraire le boundary
	std::string boundaryKey = "boundary=";
	size_t bpos = contentType.find(boundaryKey);
	if (bpos == std::string::npos) return false;
	std::string boundary = "--" + contentType.substr(bpos + boundaryKey.size());

	size_t start = body.find(boundary);
	if (start == std::string::npos) return false;

	// On saute le boundary et les headers de la partie
	size_t headerEnd = body.find("\r\n\r\n", start);
	if (headerEnd == std::string::npos) return false;
	headerEnd += 4; // sauter \r\n\r\n

	// Récupérer le nom du fichier depuis Content-Disposition
	size_t fnStart = body.find("filename=\"", start);
	if (fnStart == std::string::npos) return false;
	fnStart += 10;
	size_t fnEnd = body.find("\"", fnStart);
	if (fnEnd == std::string::npos) return false;
	std::string filename = body.substr(fnStart, fnEnd - fnStart);
	
	// Contenu du fichier
	size_t dataEnd = body.find(boundary, headerEnd);
	if (dataEnd == std::string::npos) return false;

	std::string fileContent = body.substr(headerEnd, dataEnd - headerEnd - 2); // enlever le \r\n avant boundary

	// Écriture sur le disque
	std::ofstream file((uploadDir + "/" + filename).c_str(), std::ios::binary);
	if (!file.is_open()) return false;


	file.write(fileContent.c_str(), fileContent.size());
	file.close();

	return true;
}
