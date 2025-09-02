/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 15:47:12 by lde-merc          #+#    #+#             */
/*   Updated: 2025/09/02 12:06:19 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"


std::map<int, Request> Server::_static_responses;

// Constructeur
Server::Server() {
	_port = 8080;
	_addrlen = sizeof(_address);
	_static_responses[404] = Request::make_404();
	_static_responses[405] = Request::make_405();
	_static_responses[500] = Request::make_500();

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

void Server::init() {

	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd < 0) {
		std::cerr << "Failed to create socket" << std::endl;
		exit(EXIT_FAILURE);
	}
	// Set the socket to non-blocking mode
	// This allows the server to handle multiple clients without blocking
	int flags = fcntl(_fd, F_GETFL, 0);
	if (flags == -1) { perror("fcntl F_GETFL"); exit(1); }
	if (fcntl(_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
		perror("fcntl F_SETFL");
		exit(1);
	}

	memset(&_address, 0, sizeof(_address));
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(_port);

	int opt = 1;
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		std::cerr << "setsockopt failed" << std::endl;
		close(_fd);
		exit(EXIT_FAILURE);
	}

	if (bind(_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0) {
		std::cerr << "Failed to bind socket" << std::endl;
		close(_fd);
		exit(EXIT_FAILURE);
	}

	if (listen(_fd, 5) < 0) {
		std::cerr << "Failed to listen on socket" << std::endl;
		close(_fd);
		exit(EXIT_FAILURE);
	}
}

void Server::run() {
	struct pollfd pfd;
	pfd = {_fd, POLLIN, 0}; // C++98 style, fd, events, revents
	_fds.push_back(pfd);

	while (true) {
		int ret = poll(_fds.data(), _fds.size(), 100);
		if (ret < 0) {
			if (errno == EINTR)  continue;
			std::cerr << "Poll failed" << std::endl;
			close(_fd);
			exit(EXIT_FAILURE);
		}
		if (ret == 0) continue;
		
		if (_fds[0].revents & POLLIN)
			accept_client();
		for(int i = 0; i < _fds.size(); i++) {
			if (_fds[i].revents & POLLIN) {
				handle_request(i);
				i--; // on erase le client, donc on decremente l'index
			}
		}
	}
}

void Server::accept_client() {
	int client_fd;
	
	client_fd = accept(_fd, (struct sockaddr *)&_address, &_addrlen);
	if (client_fd < 0) {
		std::cerr << "Failed to accept connection" << std::endl;
		close(_fd);
		exit(EXIT_FAILURE);
	}
	std::cout << "Client connected" << std::endl;
	struct pollfd pfc;
	pfc = {client_fd, POLLIN, 0}; // C++98 style, fd, events, revents
	_fds.push_back(pfc);
}

void Server::handle_request(int i) {
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	int bytes_read = recv(_fds[i].fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes_read <= 0) {
		if (bytes_read == 0)
			std::cout << "Client disconnected" << std::endl;
		else
			std::cerr << "Failed to read from client" << std::endl;
		close(_fds[i].fd);
		_fds.erase(_fds.begin() + i);
		return;
	}
	buffer[bytes_read] = '\0';
	std::cout << "Received request:\n" << buffer << std::endl;
	Request req = Request(buffer);
	
}
