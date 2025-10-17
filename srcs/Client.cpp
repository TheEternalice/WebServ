/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 16:06:59 by lde-merc          #+#    #+#             */
/*   Updated: 2025/10/06 16:44:55 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"

Client::Client() { }
Client::~Client() { close(_fd); }
Client::Client(int fd, struct sockaddr_in addr): _fd(fd), _adrress(addr) { }
int Client::getFd() const { return _fd; }

bool Client::requestComplete() const { return _requestComplete; }

void Client::readFromSocket() {
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	int bytes_read = recv(_fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes_read < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			// Aucune donnée pour le moment
			return;
		}else{
			std::cerr << "Erreur lors de la lecture du client (fd " << _fd << "): "
					  << strerror(errno) << std::endl;
			close(_fd);
			_fd = -1;
			return;
		}
	}else if (bytes_read == 0){
		// Client a fermé la connexion
		std::cout << "Client déconnecté (fd " << _fd << ")" << std::endl;
		close(_fd);
		_fd = -1;
		return;
	}else{
		_requestBuffer.append(buffer, bytes_read);
		// Vérifie si la requête est complète (terminée par \r\n\r\n)
		if (_requestBuffer.find("\r\n\r\n") != std::string::npos)
			_requestComplete = true;
	}
}

void Client::writeToSocket() {
	if (_responseBuffer.empty()) return;

	int bytes_sent = send(_fd, _responseBuffer.c_str(), _responseBuffer.size(), 0);
	if (bytes_sent < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			// Socket non prête pour l'écriture
			return;
		}else{
			std::cerr << "Erreur lors de l'écriture vers le client (fd " << _fd << "): "
					  << strerror(errno) << std::endl;
			close(_fd);
			_fd = -1;
			return;
		}
	}else{
		_responseBuffer.erase(0, bytes_sent);
		if (_responseBuffer.empty())
			_responseSent = true;
	}
}

void Client::parseRequest() {
	size_t	pos = _requestBuffer.rfind("\r\n\r\n");
	if (pos == std::string::npos) {
		throw (std::runtime_error("Invalid HTTP request"));
	}
	std::string header = _requestBuffer.substr(0, pos);
	std::string body = _requestBuffer.substr(pos + 4);
	std::istringstream stream(header);
	std::string line;
	if (!std::getline(stream, line))
		throw (std::runtime_error("Empty HTTP request"));
	if (line[line.size() - 1] == '\r') line.resize(line.size() - 1);
	std::istringstream first_line(line);
	first_line >> _method >> _url >> _httpVersion;
	while (std::getline(stream, line)) {
		if (line[line.size() - 1] == '\r') line.resize(line.size() - 1);
		if (line.empty()) break;
		size_t colon = line.find(":");
		if (colon == std::string::npos) continue;
		std::string key = line.substr(0, colon);
		std::string value = line.substr(colon + 1);
		_headers[key] = value;
	}
	if (hasHeader("Content-Length")) {
		int len = to_int(getHeader("Content-Length"));
		_body = body.substr(0, len);
	}
	// else if (hasHeader("Transfer-Encoding") && getHeader("Transfer-Encoding") == "chunked") {_body = parseChunked(body);}
	else {_body = body;}
}

bool Client::hasHeader(const std::string &buffer) const {return (_headers.find(buffer) != _headers.end());}
bool Client::outputEmpty() const { return _responseBuffer.empty(); }


const std::string &Client::getHeader(const std::string &buffer) const {
	std::map<std::string, std::string>::const_iterator it = _headers.find(buffer);
	if (it != _headers.end())
		return (it->second);
	throw (std::runtime_error("Header not found: " + buffer));
}