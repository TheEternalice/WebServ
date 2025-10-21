/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-rese <ade-rese@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 14:17:07 by lde-merc          #+#    #+#             */
/*   Updated: 2025/10/21 16:42:42 by ade-rese         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

// Constructeur
Client::Client() {}

Client::~Client() {}

Client::Client(const Client &other) {
    *this = other;
}

Client &Client::operator=(const Client &other) {
    if (this != &other) {
        // copy attributes here
    }
    return *this;
}

void Client::readFromSocket() {
	char buffer[1024];
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

bool Client::tryParseRequest() {
	size_t pos = _buffer_in.find("\r\n\r\n");
	if (pos == std::string::npos)
		return false; // pas encore complet

	_request = Request(_buffer_in.substr(0, pos + 4));
	_buffer_in.erase(0, pos + 4); // garde ce qui reste
	return true;
}


void HTTP_Server::handleRequest(Client& client) {
	const std::string& method = client.getRequest().get_method();
	Reponse res;

	if (!is_method_allowed(method, client)) {
		res = _static_responses[405];
	} else if (method == "GET") {
		res = client.getRequest().handle_get();
	} else if (method == "POST") {
		res = client.getRequest().handle_post();
	} else if (method == "DELETE") {
		res = client.getRequest().handle_delete();
	} else {
		res = _static_responses[400];
	}

	res.set_header("Connection", "keep-alive");
	res.set_header("Keep-Alive", "timeout=20, max=100");
	client.setResponse(res.to_string());
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

void Client::setResponse(std::string res) {
	_buffer_out.clear();
	_buffer_out = res;
}
