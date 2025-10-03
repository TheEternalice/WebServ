/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 12:26:18 by lde-merc          #+#    #+#             */
/*   Updated: 2025/10/03 15:07:18 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Socket.hpp"

Socket::Socket() { }
Socket::~Socket() { }
Socket::Socket(const Socket &other) { *this = other; }
Socket &Socket::operator=(const Socket &other) {
	if (this != &other) {
		this->_fd = other._fd;
		this->_address = other._address;
		this->_addrlen = other._addrlen;
		this->_port = other._port;
		this->_host = other._host;
		this->_server_name = other._server_name;
		this->_error_pages = other._error_pages;
		this->_max_body_size = other._max_body_size;
		this->_path = other._path;
		this->_index = other._index;
		this->_root = other._root;
		this->_autoIndex = other._autoIndex;
		this->_returnPath = other._returnPath;
		this->_upload_dir = other._upload_dir;
		this->_allowedMethods = other._allowedMethods;
		this->_cgiExtensions = other._cgiExtensions;
		this->_errorsResponses = other._errorsResponses;
	}
	return (*this);
}

void Socket::initialize_socket() {
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd < 0)
		throw SocketException("Failed to create socket");
	
	int flags = fcntl(_fd, F_GETFL, 0);
	if (flags == -1)
		throw SocketException("fcntl F_GETFL");
	if (fcntl(_fd, F_SETFL, flags | O_NONBLOCK) == -1)
		throw SocketException("fcntl F_SETFL");
	
	memset(&_address, 0, sizeof(_address));
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(_port);
	
	int opt = 1;
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		close(_fd);
		throw SocketException("setsockopt failed");
	}
	if (bind(_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0) {
		close(_fd);
		throw SocketException("Failed to bind socket");
	}
	
	if (listen(_fd, 5) < 0) {
		close(_fd);
		throw SocketException("Failed to listen on socket");
	}
	std::cout << "Socket initialized on port " << _port << std::endl;
}

