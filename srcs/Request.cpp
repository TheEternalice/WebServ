/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 16:26:26 by lde-merc          #+#    #+#             */
/*   Updated: 2025/08/14 16:28:18 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Request.hpp"

// Constructeur
Request::Request() {}

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


std::string Request::get_content_type(const std::string &path) {
	if (path.size() >= 5 && path.substr(path.size()-5) == ".html") return "text/html";
	if (path.size() >= 4 && path.substr(path.size()-4) == ".txt")  return "text/plain";
	if (path.size() >= 4 && path.substr(path.size()-4) == ".css")  return "text/css";
	if (path.size() >= 3 && path.substr(path.size()-3) == ".js")   return "application/javascript";
	if (path.size() >= 4 && path.substr(path.size()-4) == ".jpg")  return "image/jpeg";
	if (path.size() >= 4 && path.substr(path.size()-4) == ".png")  return "image/png";
	return "application/octet-stream";
}

Request Request::make_404() {
	Request r;
	r.response_body = "404 Not Found";
	r.response_headers =
		"HTTP/1.1 404 Not Found\r\n"
		"Content-Type: text/plain\r\n"
		"Content-Length: " + std::to_string(r.response_body.size()) + "\r\n"
		"\r\n";
	return r;
}

Request Request::make_405() {
	Request r;
	r.response_body = "405 Method Not Allowed";
	r.response_headers =
		"HTTP/1.1 405 Method Not Allowed\r\n"
		"Content-Type: text/plain\r\n"
		"Content-Length: " + std::to_string(r.response_body.size()) + "\r\n"
		"\r\n";
	return r;
}

Request Request::make_500() {
	Request r;
	r.response_body = "500 Internal Server Error";
	r.response_headers =
		"HTTP/1.1 500 Internal Server Error\r\n"
		"Content-Type: text/plain\r\n"
		"Content-Length: " + std::to_string(r.response_body.size()) + "\r\n"
		"\r\n";
	return r;
}
