/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 14:17:07 by lde-merc          #+#    #+#             */
/*   Updated: 2025/10/27 16:07:17 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"

// Constructeur
Client::Client() { }

Client::~Client() { }

Client::Client(const Client &other) {
    *this = other;
}

Client::Client(int fd): _fd(fd) { }

Client &Client::operator=(const Client &other) {
    if (this != &other) {
        // copy attributes here
    }
    return *this;
}

void Client::readFromSocket() {
	char buffer[4096];
	int bytes_read = recv(_fd, buffer, sizeof(buffer), 0);

	if (bytes_read < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return; // rien de nouveau
		throw std::runtime_error("recv() failed");
	}
	if (bytes_read == 0) {
		_closed = true; // le client a fermé
		return;
	}

	_buffer_in.append(buffer, bytes_read);
}

// bool Client::tryParseRequest() {
// 	size_t pos = _buffer_in.find("\r\n\r\n");
// 	if (pos == std::string::npos)
// 		return false; // pas encore complet
// 	_request = Request(_buffer_in);
// 	_buffer_in.erase(0, pos + 4); // garde ce qui reste
// 	return true;
// }

bool Client::tryParseRequest() {
    if (!_request.hasHeader("Content-Length")) {
        // Si on a déjà reçu le header, on peut parser
        if (_buffer_in.find("\r\n\r\n") != std::string::npos) {
            _request = Request(_buffer_in);
			return true;
		}
    } else {
        int len = to_int(_request.getHeader("Content-Length"));
        size_t bodyStart = _buffer_in.find("\r\n\r\n");
        if (bodyStart != std::string::npos && _buffer_in.size() >= bodyStart + 4 + len) {
            _request = Request(_buffer_in);
			return true;
		}
    }
    return false; // pas encore tout reçu
}

void Client::writeToSocket() {
	if (_buffer_out.empty())
		return;

	int sent = send(_fd, _buffer_out.c_str(), _buffer_out.size(), 0);
	if (sent < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return;
		throw std::runtime_error("send() failed");
	}
	_buffer_out.erase(0, sent);
}

bool Client::outputEmpty() {
	return _buffer_out.empty();
}

Request Client::getRequest() {
	return _request;
}

int	Client::get_fd() {
	return (_fd);	
}

void Client::setResponse(Reponse res) {
	_buffer_out.clear();
	_buffer_out = res.to_string();
	_reponse = res;
}

bool Client::shouldClose() const {
	return _closed;
}

void Client::resetForNextRequest() {
	_request = Request();
	_reponse = Reponse();
	_buffer_in.clear();
	_buffer_out.clear();
	_closed = false;
}

std::string Client::getBufferIn() {
	return _buffer_in;
}

std::string Client::getBufferOut() {
	return _buffer_out;
}

Reponse Client::getReponse() {
	return _reponse;
}