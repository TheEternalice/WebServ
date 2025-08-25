/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 15:47:12 by lde-merc          #+#    #+#             */
/*   Updated: 2025/08/14 16:39:42 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"


std::map<int, Request> Server::static_responses;

// Constructeur
Server::Server() {
	this->_port = 8080;
	this->static_responses[404] = Request::make_404();
	this->static_responses[405] = Request::make_405();
	this->static_responses[500] = Request::make_500();

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

	this->_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_fd < 0) {
		std::cerr << "Failed to create socket" << std::endl;
		exit(EXIT_FAILURE);
	}
	// Set the socket to non-blocking mode
	// This allows the server to handle multiple clients without blocking
	int flags = fcntl(this->_fd, F_GETFL, 0);
	if (flags == -1) { perror("fcntl F_GETFL"); exit(1); }
	if (fcntl(this->_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
		perror("fcntl F_SETFL");
		exit(1);
	}

	memset(&_address, 0, sizeof(_address));
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(_port);

	int opt = 1;
	if (setsockopt(this->_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		std::cerr << "setsockopt failed" << std::endl;
		close(this->_fd);
		exit(EXIT_FAILURE);
	}

	if (bind(this->_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0) {
		std::cerr << "Failed to bind socket" << std::endl;
		close(this->_fd);
		exit(EXIT_FAILURE);
	}

	if (listen(this->_fd, 5) < 0) {
		std::cerr << "Failed to listen on socket" << std::endl;
		close(this->_fd);
		exit(EXIT_FAILURE);
	}
}

void Server::run() {
	
}