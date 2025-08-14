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

#include "Request.hpp"

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
