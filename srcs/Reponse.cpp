/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reponse.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 12:32:35 by lde-merc          #+#    #+#             */
/*   Updated: 2025/09/30 16:17:06 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Reponse.hpp"
#include "../includes/Server.hpp"

// Constructeur
Reponse::Reponse() {}

Reponse::Reponse(std::string method, std::string url) {
	(void)method;
	std::string path = "." + url; // exemple : "/style.css" → "./style.css"
	std::ifstream file(path.c_str(), std::ios::binary);

	if (!file) {
		Reponse res404 = make_404();
		_status_code = res404._status_code;
		_status_text = res404._status_text;
		_body = res404._body;
		_headers = res404._headers;
		return;
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
	_headers["Content-Type"] = "text/plain";	
	_headers["Content-Length"] = this->to_string();
}

Reponse::~Reponse() {}

Reponse::Reponse(const Reponse &other) {
    *this = other;
}

Reponse &Reponse::operator=(const Reponse &other) {
    if (this != &other) {
        // copy attributes here
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
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin();
		it != _headers.end(); ++it) {
		oss << it->first << ": " << it->second << "\r\n";
	}
	oss << "\r\n" << _body;
	return oss.str();
}

Reponse Reponse::make_200(const std::string& body, const std::string& type) {
	Reponse r;
	r._status_code = 200;
	r._status_text = "OK";
	r._body = body;
	r._headers["Content-Type"] = type;
	std::ostringstream oss;
	oss << r._body.size();
	r._headers["Content-Length"] = oss.str();
	return r;
}

Reponse Reponse::make_404() {
	 Reponse r;
	r._status_code = 404;
	r._status_text = "Not Found";
	r._body = "404 Not Found";

	// std::map<int, std::string> errorPages = ;
	
	
	// std::ifstream file();
	// if (!file.is_open()){
	// 	throw CannotBeOpen();
	// }
	


	
	r._headers["Content-Type"] = "text/plain";
	r._headers["Content-Length"] = r.to_string();
	return r;
}

Reponse Reponse::make_405() {
	Reponse r;
	r._status_code = 405;
	r._status_text = "Method Not Allowed";
	r._body = "405 Method Not Allowed";
	r._headers["Content-Type"] = "text/plain";
	r._headers["Content-Length"] = r.to_string();
	return r;
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
