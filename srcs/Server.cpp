/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 15:47:12 by lde-merc          #+#    #+#             */
/*   Updated: 2025/10/22 11:04:23 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

std::map<int, Reponse> Server::_static_responses;
std::map<std::string, std::string> Server::_extensionsToType;

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
		_static_responses = other._static_responses;
		_extensionsToType = other._extensionsToType;
	}
	return *this;
}

void Server::copy_socket(std::vector<ServerSocket> other) {
	this->_sockets = other;
}

std::vector<ServerSocket> Server::get_Socket() { return _sockets; }

void Server::display_Serv() {
	for (size_t i = 0; i < _sockets.size(); i++){
		// std::cout << "------- Server " << i + 1 << " -------" << std::endl;
		std::cout << "Host : ";
		if (!_sockets[i]._host.empty())
			std::cout << _sockets[i]._host << std::endl;
		std::cout << "Port : ";
		if (_sockets[i]._port)
			std::cout << _sockets[i]._port << std::endl;
		std::cout << "Server name : ";
		if (!_sockets[i]._server_name.empty())
			std::cout << _sockets[i]._server_name << std::endl;
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

	while (true) {
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
				// Si c’est un socket serveur, on accepte un client
				// Sinon, c’est un client existant
				if (isServerSocket(fd)) {
					accept_client(fd);
				} else {
					Client *cl = _socketToClient[fd];
					cl->readFromSocket();
					if (cl->tryParseRequest()) {
						handle_request(*cl);
						_fds[i].events = POLLOUT;
					}
				}
			}
			if (re & POLLOUT) {
				if (_socketToClient.count(fd)) {
					Client *cl = _socketToClient[fd];
					cl->writeToSocket();
					if (cl->outputEmpty())
						_fds[i].events &= ~POLLOUT;
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

	std::cout << "New client connected on port " << server._port
			  << " (fd=" << client_fd << ")" << std::endl;
}

void Server::handle_request(Client &client) {
	const std::string method = client.getRequest().get_method();
	Reponse res;
	ServerSocket* server = &_clientToSocket[client.get_fd()];
	if (!is_method_allowed(method, client)) {
		res = server->_autoResponse[405];
	} else if (method == "GET") {
		res = client.getRequest().handle_get();
	} else if (method == "POST") {
		res = client.getRequest().handle_post();
	}else if (method == "DELETE") {
		res = client.getRequest().handle_delete();
	} else {
		res = server->_autoResponse[400];
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
	pos = tmpLocation.find('/');
	if (pos + 1 < 15)
		tmpLocation = tmpLocation.substr(0, pos + 1);
	else
		tmpLocation = tmpLocation.substr(0, tmpLocation.length());
	if ((server->_allowedMethods[tmpLocation] & nummethode) != 0)
		return true;
	return false;
}
