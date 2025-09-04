/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 15:47:12 by lde-merc          #+#    #+#             */
/*   Updated: 2025/09/04 15:03:35 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"


std::map<int, Reponse> Server::_static_responses;
std::map<std::string, std::string> Server::_extensionsToType;

// Constructeur
Server::Server() {
	for (int i = 0; i < 4; i++) {
		ServerSocket s;
		s._port = 8080 + i;
		sockets.push_back(s);
	}
	_static_responses[404] = Reponse::make_404();
	_static_responses[405] = Reponse::make_405();
	_static_responses[500] = Reponse::make_500();
	_allowedMethods.push_back("GET");
	_allowedMethods.push_back("POST");
	_allowedMethods.push_back("DELETE");
	_extensionsToType[".html"] = "text/html";
	_extensionsToType[".css"]  = "text/css";
	_extensionsToType[".js"]   = "application/javascript";
	_extensionsToType[".jpg"]  = "image/jpeg";
	_extensionsToType[".png"]  = "image/png";
}

Server::~Server() {}

Server::Server(const Server &other) {
    *this = other;
}

Server &Server::operator=(const Server &other) {
    if (this != &other) {
        // copy attributes here
    }
    return *this;
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
	for(int i = 0; i < sockets.size(); i++) {
		sockets[i].fd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockets[i].fd < 0) {
			std::cerr << "Failed to create socket" << std::endl;
			exit(EXIT_FAILURE);
		}
		// Set the socket to non-blocking mode
		// This allows the server to handle multiple clients without blocking
		int flags = fcntl(sockets[i].fd, F_GETFL, 0);
		if (flags == -1) { std::cerr << "fcntl F_GETFL" << std::endl; exit(1); }
		if (fcntl(sockets[i].fd, F_SETFL, flags | O_NONBLOCK) == -1) {
			std::cerr << "fcntl F_SETFL" << std::endl;
			exit(1);
		}

		memset(&sockets[i].address, 0, sizeof(sockets[i].address));
		sockets[i].address.sin_family = AF_INET;
		sockets[i].address.sin_addr.s_addr = INADDR_ANY;
		sockets[i].address.sin_port = htons(sockets[i]._port);

		int opt = 1;
		if (setsockopt(sockets[i].fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
			std::cerr << "setsockopt failed" << std::endl;
			close(sockets[i].fd);
			exit(EXIT_FAILURE);
		}

		if (bind(sockets[i].fd, (struct sockaddr *)&sockets[i].address, sizeof(sockets[i].address)) < 0) {
			std::cerr << "Failed to bind socket" << std::endl;
			for(int j = 0; j <= i; j++)
				close(sockets[j].fd);
			exit(EXIT_FAILURE);
		}

		if (listen(sockets[i].fd, 5) < 0) {
			std::cerr << "Failed to listen on socket" << std::endl;
			for(int j = 0; j <= i; j++)
				close(sockets[j].fd);
			exit(EXIT_FAILURE);
		}
	}
}

void Server::run() {
	for(size_t i = 0; i < sockets.size(); i++) {
		struct pollfd pfd;
		pfd = {sockets[i].fd, POLLIN, 0}; // C++98 style, fd, events, revents
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
		
		for(int i = 0; i < sockets.size(); i++){
			if (_fds[i].revents & POLLIN)
				accept_client(sockets[i]);
		}
		for(int i = sockets.size(); i < _fds.size(); i++) {
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
		std::cout << "Received request\n";
		// std::cout << "Received request:\n" << buffer << std::endl;
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
			
			std::string response = res.to_string();
			send(_fds[i].fd, response.c_str(), response.size(), 0);
		}
		close(_fds[i].fd);
	}
}

bool Server::is_method_allowed(const std::string& method) {
	for (size_t i = 0; i < _allowedMethods.size(); i++) {
		if (_allowedMethods[i] == method)
			return true;
	}
	return false;
}