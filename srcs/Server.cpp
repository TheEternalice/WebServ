/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gebz <gebz@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 15:47:12 by lde-merc          #+#    #+#             */
/*   Updated: 2025/08/25 01:34:07 by gebz             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

// Constructeur
Server::Server() {}

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

	memset(this->_address, 0, sizeof(this->_address));
	this->_address->sin_family = AF_INET;
	this->_address->sin_addr.s_addr = INADDR_ANY;
	this->_address->sin_port = htons(this->_port);

	int opt = 1;
	if (setsockopt(this->_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		std::cerr << "setsockopt failed" << std::endl;
		close(this->_fd);
		exit(EXIT_FAILURE);
	}

	if (bind(this->_fd, (struct sockaddr *)this->_address, sizeof(*this->_address)) < 0) {
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