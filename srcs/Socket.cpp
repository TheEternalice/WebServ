/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 12:26:18 by lde-merc          #+#    #+#             */
/*   Updated: 2025/10/03 13:26:01 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Socket.hpp"

Socket::Socket() { }
Socket::~Socket() { }
Socket::Socket(const Socket &other) { *this = other; }
Socket &Socket::operator=(const Socket &other) {
	if (this != &other) {
		this->fd = other.fd;
		this->address = other.address;
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
