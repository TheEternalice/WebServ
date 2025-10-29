/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 14:17:07 by lde-merc          #+#    #+#             */
/*   Updated: 2025/10/29 10:45:37 by lde-merc         ###   ########.fr       */
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
	char buffer[8192];
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
	_bytes_read += bytes_read;
	_buffer_in.append(buffer, bytes_read);
	// On garde une copie exacte des octets recus
	_raw_buffer.insert(_raw_buffer.end(), buffer, buffer + bytes_read);
}


bool Client::tryParseRequest() {
	size_t headerEnd = _buffer_in.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
		return false; // headers pas encore complets

	// Si les headers sont complets, on peut extraire la longueur du corps
	std::string headersPart = _buffer_in.substr(0, headerEnd);
	size_t contentLengthPos = headersPart.find("Content-Length:");

	if (contentLengthPos != std::string::npos) {
		size_t lineEnd = headersPart.find("\r\n", contentLengthPos);
		std::string value = headersPart.substr(contentLengthPos + 15, lineEnd - (contentLengthPos + 15));
		int len = to_int(value);

		// Vérifie si tout le corps a été reçu
		if (_raw_buffer.size() >= headerEnd + 4 + static_cast<size_t>(len)) {
			_request = Request(std::string(_raw_buffer.begin(), _raw_buffer.end()), this->_bytes_read);
			return true;
		}
	} else {
		// Pas de corps ou petit corps, on peut parser
		_request = Request(std::string(_raw_buffer.begin(), _raw_buffer.end()), this->_bytes_read);
		return true;
	}
	return false;
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
	_raw_buffer.clear();
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