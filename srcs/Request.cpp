/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-rese <ade-rese@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 16:26:26 by lde-merc          #+#    #+#             */
/*   Updated: 2025/09/07 14:12:18 by ade-rese         ###   ########.fr       */
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
	if (hasHeader("Content-Length")) {
		int len = std::stoi(getHeader("Content-Length"));
		_body = body.substr(0, len);
	}
	else if (hasHeader("Transfer-Encoding") && getHeader("Transfer-Encoding") == "chunked") {_body = parseChunked(body);}
	else {_body = body;}
}

bool Request::hasHeader(const std::string &buffer) const {return (_headers.find(buffer) != _headers.end());}

std::string Request::getHeader(const std::string &buffer) const {
	std::map<std::string, std::string>::const_iterator it = _headers.find(buffer);
	if (it != _headers.end())
		return (it->second);
	return ("");
}

std::string Request::parseChunked(const std::string &buffer) {
	std::string body;
	size_t pos = 0;
	
	while (true) {
		size_t endl = buffer.find("\r\n", pos);
		if (endl == std::string::npos) break;
		std::string str_sub = buffer.substr(pos, endl - pos);
		int chunk_size = std::stoi(str_sub, nullptr, 16);
		if (chunk_size == 0) break;
		pos = endl + 2;
		body += buffer.substr(pos, chunk_size);
		pos += chunk_size + 2;
	}
	return (body);
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
