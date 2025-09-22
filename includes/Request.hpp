/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 16:26:29 by lde-merc          #+#    #+#             */
/*   Updated: 2025/09/22 10:37:36 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <map>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <algorithm>


#include "Reponse.hpp"


class Request {
	public:
		Request();
		Request(const std::string &request);
		~Request();
		Request(const Request &other);
		
		Request &operator=(const Request &other);

		std::string get_method() const;
		std::string get_url() const;
		std::string get_body() const;
		std::string getHeader(const std::string& buffer) const;
		std::map<std::string, std::string> getCookies() const;
		std::string getCookie(const std::string& name) const;

		bool hasHeader(const std::string& buffer) const;
		void parse(const std::string& buffer);
		std::string parseChunked(const std::string& buffer);

		Reponse handle_get();
		Reponse execute_cgi_get(std::string& path);
		Reponse execute_cgi_post(std::string& path, std::string& body);
		Reponse handle_post();
		Reponse handle_delete();
		void parseCookies();

		void print_cookie();

	private:
		std::string _method;       // GET, POST, DELETE, etc.
		std::string _url;          // /index.html, /script.py
		std::string _http_version; // HTTP/1.1
		std::map<std::string, std::string> _headers; // "Host" -> "localhost:8080"
		std::string _body;         // Corps de la requête (POST data)
		std::map<std::string, std::string> _request_cookies;
};
