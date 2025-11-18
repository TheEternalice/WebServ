/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reponse.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-rese <ade-rese@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 12:32:35 by lde-merc          #+#    #+#             */
/*   Updated: 2025/10/29 14:46:37 by ade-rese         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Reponse.hpp"
#include "../includes/Server.hpp"

Reponse::Reponse() {}

Reponse::Reponse(std::string url, std::string root, std::string index, std::string locationPath) {
	std::string path;
	
	if (locationPath != "/" && url.size() >= locationPath.size()) {
		if (url.compare(0, locationPath.size(), locationPath) == 0) {
			url = url.substr(locationPath.size());
			if (url.empty() || url[0] != '/')
				url = "/" + url;
		}
	}
	if (!root.empty() && url.size() && url.find('/' + root) == 0) {
		url = url.substr(root.size() + 1);
		if (url.empty() || url[0] != '/')
			url = "/" + url;
	}
	if (url == "/") {
		if (root.empty())
			root = ".";
		if (!index.empty()){
			std::vector<std::string> indexFiles = cpp_split(index, ' ');
			for (size_t i = 0; i < indexFiles.size(); i++) {
				path = root + "/" + indexFiles[i];
				if (access(path.c_str(), F_OK) == 0)
					break;
				path.clear();
			}
		}
		if (path.empty())
			path = root;
	} else {
		if (root.empty())
			path = "." + url;
		else
			path = root + url;
	}
	std::ifstream file(path.c_str(), std::ios::binary);

	if (!file || access(path.c_str(), F_OK) != 0) {
		throw (std::runtime_error(""));
		
	} else {
		std::ostringstream oss;
		oss << file.rdbuf();
		_body = oss.str();
		_status_code = 200;
		_status_text = "OK";
		_headers["Content-Type"] = Server::get_content_type(path); // text/html, text/css, etc.
	}

	std::ostringstream oss_len;
	oss_len << _body.size();
	_headers["Content-Length"] = oss_len.str();
}

Reponse::Reponse(int num, std::string path) {
	_status_code = num;
	
	switch (_status_code) {
		case 400:
			_status_text = "Bad request";
			break;
		case 403:
			_status_text = "Forbidden";
			break;
		case 404:
			_status_text = "Not Found";
			_headers["Connection"] = "close";
			break;
		case 405: 
			_status_text = "Method Not Allowed";
			break;
		case 500:
			_status_text = "Internal Server Error";
			break;
		case 503:
			_status_text = "Service Unavailable";
			break;
		default:
			break;
	}
	
	_body = "";
	std::ifstream bodyData(path.c_str());
	if (!bodyData.is_open()) {throw std::runtime_error("Can't open bodyData file");}
	
	char c = 0;
	while (bodyData.get(c)){ std::string s; s.push_back(c); _body += s; }
	

	if (_body.empty()) std::cout << "body empty in constructor" << std::endl;
	_headers["Content-Type"] = Server::get_content_type(path);	
	std::ostringstream oss;
	oss << _body.size();
	_headers["Content-Length"] = oss.str();
}

Reponse::~Reponse() {}

Reponse::Reponse(const Reponse &other) {
	*this = other;
}

Reponse &Reponse::operator=(const Reponse &other) {
	if (this != &other) {
		this->_status_code = other._status_code;
		this->_status_text = other._status_text;
		this->_headers = other._headers;
		this->_body = other._body;
	}
	return *this;
}

std::map<std::string, std::string> Reponse::get_header() const {
	return _headers;
}

std::string Reponse::to_string() const {
	std::ostringstream oss;
	oss << "HTTP/1.1 " << _status_code << " " << _status_text << "\r\n";
	std::map<std::string, std::string>::const_iterator it;
	for (it = _headers.begin(); it != _headers.end(); ++it) {
		oss << it->first << ": " << it->second << "\r\n";
	}
	oss << "\r\n" << _body;
	return oss.str();
}

Reponse Reponse::make_500() {
	Reponse r;
	r._status_code = 500;
	r._body = "500 Internal Server Error";
	r._headers["Content-Type"] = "text/plain";
	r._headers["Content-Length"] = r.to_string();
	return r;
}

int Reponse::get_status_code() const {
	return _status_code;
}


std::string Reponse::get_body() const {
	return _body;
}

void Reponse::set_status_code(int code) {
	_status_code = code;
}

void Reponse::set_status_text(const std::string& text) {
	_status_text = text;
}

void Reponse::set_body(const std::string& body) {
	_body = body;
}

void Reponse::set_header(const std::string& key, const std::string& value) {
	_headers[key] = value;
}

static std::string toLower(const std::string &s) {
	std::string out = s;
	for (size_t i = 0; i < out.size(); ++i) out[i] = static_cast<char>(std::tolower(out[i]));
	return out;
}

bool Reponse::isKeepAlive() const {
	// Search header "Connection" by ignoring failures
	std::map<std::string, std::string>::const_iterator it;
	for (it = _headers.begin(); it != _headers.end(); ++it) {
		if (toLower(it->first) == "connection") {
			std::string val = toLower(it->second);
			if (val.find("close") != std::string::npos) return false;
			if (val.find("keep-alive") != std::string::npos) return true;
		}
	}
	return true;
}
