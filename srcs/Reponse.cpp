/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reponse.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 12:32:35 by lde-merc          #+#    #+#             */
/*   Updated: 2025/09/02 12:44:13 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Reponse.hpp"

// Constructeur
Reponse::Reponse() {}

Reponse::~Reponse() {}

Reponse::Reponse(const Reponse &other) {
    *this = other;
}

Reponse &Reponse::operator=(const Reponse &other) {
    if (this != &other) {
        // copy attributes here
    }
    return *this;
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

Reponse Reponse::make_404() {
	 Reponse r;
	r._status_code = 404;
	r._status_text = "Not Found";
	r._body = "404 Not Found";
	r._headers["Content-Type"] = "text/plain";
	r._headers["Content-Length"] = std::to_string(r._body.size());
	return r;
}

Reponse Reponse::make_405() {
	Reponse r;
	r._status_code = 405;
	r._status_text = "Method Not Allowed";
	r._body = "405 Method Not Allowed";
	r._headers["Content-Type"] = "text/plain";
	r._headers["Content-Length"] = std::to_string(r._body.size());
	return r;
}

Reponse Reponse::make_500() {
	Reponse r;
	r._status_code = 500;
	r._status_text = "Internal Server Error";
	r._body = "500 Internal Server Error";
	r._headers["Content-Type"] = "text/plain";
	r._headers["Content-Length"] = std::to_string(r._body.size());
	return r;
}

