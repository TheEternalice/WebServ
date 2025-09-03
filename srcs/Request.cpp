/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 16:26:26 by lde-merc          #+#    #+#             */
/*   Updated: 2025/09/03 14:20:45 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Request.hpp"

// Constructeur
Request::Request() {}

Request::Request(const std::string &request) {
	// Simple parsing, assumes well-formed request
	size_t method_end = request.find(' ');
	if (method_end == std::string::npos) return;
	_method = request.substr(0, method_end);

	size_t url_end = request.find(' ', method_end + 1);
	if (url_end == std::string::npos) return;
	_url = request.substr(method_end + 1, url_end - method_end - 1);

	size_t version_end = request.find("\r\n", url_end + 1);
	if (version_end == std::string::npos) return;
	_http_version = request.substr(url_end + 1, version_end - url_end - 1);

	// Headers and body parsing can be added here
}

Request::~Request() {}

Request::Request(const Request &other) {
    *this = other;
}

Request &Request::operator=(const Request &other) {
    if (this != &other) {
        // copy attributes here
    }
    return *this;
}

std::string Request::get_method() const {
	return _method;
}
std::string Request::get_url() const {
	return _url;
}
