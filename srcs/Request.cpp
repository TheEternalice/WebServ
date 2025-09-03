/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-rese <ade-rese@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 16:26:26 by lde-merc          #+#    #+#             */
/*   Updated: 2025/09/03 23:35:12 by ade-rese         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

// Constructeur
Request::Request() {}

Request::Request(const std::string &request) {parse(request);}

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

const std::string &Request::getMethod() const {
	return (_method);
}

const std::string &Request::getUrl() const {
	return (_url);
}

const std::string &Request::getHttpVersion() const {
	return (_http_version);
}

const std::string &Request::getBody() const {
	return (_body);
}

// "GET /uri.cgi HTTP/1.1\r\n"
// "User-Agent: Mozilla/5.0\r\n"
// "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
// "Host: 127.0.0.1\r\n"
// "\r\n";

void Request::parse(const std::string &buffer) {
	size_t	pos = buffer.rfind("\r\n\r\n");
	if (pos == std::string::npos) {
		throw (std::runtime_error("Invalid HTTP request"));
	}
	std::string header = buffer.substr(0, pos);
	std::string body = buffer.substr(pos + 4);
	std::istringstream stream(header);
	std::string line;
	if (!std::getline(stream, line))
		throw (std::runtime_error("Empty HTTP request"));
	if (line.back() == '\r') line.pop_back();
	std::istringstream first_line(line);
	first_line >> _method >> _url >> _http_version;
	while (std::getline(stream, line)) {
		if (line.back() == '\r') line.pop_back();
		if (line.empty()) break;
		size_t colon = line.find(":");
		if (colon == std::string::npos) continue;
		std::string key = line.substr(0, colon);
		std::string value = line.substr(colon + 1);
		_headers[key] = value;
	}
	if (size_t len = _method.find("POST")) {_body = body.substr(0, len);}
	// géré les chunked body ??????
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
