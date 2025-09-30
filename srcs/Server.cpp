/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 15:47:12 by lde-merc          #+#    #+#             */
/*   Updated: 2025/09/30 13:17:40 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

std::map<int, Reponse> Server::_static_responses;
std::map<std::string, std::string> Server::_extensionsToType;

// Constructeur
Server::Server() {
	std::ifstream file("mime.types");
	if (!file.is_open()) throw std::runtime_error("Could not open mime.types");

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

std::vector<ServerSocket> Server::get_Socket() { return _sockets; }

void Server::display_Serv() {
	for(size_t i = 0; i < _sockets.size(); i++) {
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
		// std::cout << "Allowed Methods : " << std::endl;
		// if (!_sockets[i]._allowedMethods.empty()) {
		// 	for (size_t k = 0; k < _sockets[i]._allowedMethods.size(); k++)
		// 		std::cout << "   _ " << _sockets[i]._allowedMethods[k] << std::endl;
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


std::string Server::get_content_type(const std::string& path) {
	std::string::size_type dot = path.rfind('.');
	if (dot == std::string::npos)
		return "application/octet-stream";

	std::string ext = path.substr(dot);
	std::map<std::string, std::string>::const_iterator it =_extensionsToType.find(ext);
	
	if (it != _extensionsToType.end())
		return it->second;
	else
		return "application/octet-stream";
}

void Server::init() {
	for(int i = 0; i < (int)_sockets.size(); i++) {
		_sockets[i].fd = socket(AF_INET, SOCK_STREAM, 0);
		if (_sockets[i].fd < 0)
			throw std::runtime_error("Failed to create socket");
		// Set the socket to non-blocking mode
		// This allows the server to handle multiple clients without blocking
		int flags = fcntl(_sockets[i].fd, F_GETFL, 0);
		if (flags == -1) { std::cerr << "fcntl F_GETFL" << std::endl; exit(1); }
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
			for(int j = 0; j <= i; j++)
				close(_sockets[j].fd);
			throw std::runtime_error("Failed to bind socket");
		}

		if (listen(_sockets[i].fd, 5) < 0) {
			for(int j = 0; j <= i; j++)
				close(_sockets[j].fd);
			throw std::runtime_error("Failed to listen on socket");
		}
	}
}

void Server::run() {
	for(size_t i = 0; i < _sockets.size(); i++) {
		struct pollfd pfd;
		pfd.fd = _sockets[i].fd;
		pfd.events = POLLIN;
		pfd.revents = 0;
		_fds.push_back(pfd);
	}
	
	while (true) {
		int ret = poll(&_fds[0], _fds.size(), 100);
		if (ret < 0) {
			if (errno == EINTR)  continue;
			std::cerr << "Poll error" << std::endl;
			exit(EXIT_FAILURE);
		}
		if (ret == 0) continue;
		
		for(int i = 0; i < (int)_sockets.size(); i++){
			if (_fds[i].revents & POLLIN)
				accept_client(_sockets[i]);
		}
		for(size_t i = _sockets.size(); i < _fds.size(); i++) {
			if (_fds[i].revents & POLLIN) {
				handle_request(i);
				i--; // on erase le client, donc on decremente l'index
			}
		}
	}
}

void Server::accept_client(ServerSocket& s) {
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);

	int client_fd = accept(s.fd, (struct sockaddr *)&client_addr, &client_len);
	if (client_fd < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return; // rien à accepter
		std::cerr << "accept() failed on port " << s._port
				<< ": " << strerror(errno) << std::endl;
		return;
	}

	std::cout << "Client connected on port " << s._port << std::endl;

	struct pollfd pfc;
	pfc.fd = client_fd;
	pfc.events = POLLIN;
	pfc.revents = 0;
	_fds.push_back(pfc);
}

void Server::handle_request(int i) {
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	int bytes_read = recv(_fds[i].fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes_read < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			// Pas encore de données, ce n'est pas une erreur
			return; // ou continue dans la boucle poll
		} else {
			// Erreur réelle
			std::cerr << "Failed to read from client" << std::endl;
			close(_fds[i].fd);
			_fds.erase(_fds.begin() + i);
			return;
		}
	} else if (bytes_read == 0) {
			// Client a fermé la connexion
			std::cout << "Client disconnected" << std::endl;
			close(_fds[i].fd);
			_fds.erase(_fds.begin() + i);
			return;
	} else {
		buffer[bytes_read] = '\0';
		Request req = Request(buffer);
		Reponse res;
		if (is_method_allowed(req.get_method())) {
			// Handle the request
			switch(req.get_method()[0]) {
				case 'G': {
					res = req.handle_get();
					break;
				} case 'P': {
					res = req.handle_post();
					break;
				} case 'D': {
					res = req.handle_delete();
					break;
				} default:
					res = _static_responses[405];
					break;
			}
			res.set_header("Connection", "keep-alive");
			res.set_header("Keep-Alive", "timeout=20, max=100");
			
			std::string response = res.to_string();
			send(_fds[i].fd, response.c_str(), response.size(), 0);
		}
	}
}

bool Server::is_method_allowed(const std::string& method) {
	for (size_t i = 0; i < _sockets[i]._allowedMethods.size(); i++) {
		// std::cout << _sockets[i]._allowedMethods.size() << std::endl;
		if (_sockets[i]._allowedMethods[i] == method)
			return true;
	}
	return false;
}
