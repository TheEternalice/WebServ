/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 16:26:29 by lde-merc          #+#    #+#             */
/*   Updated: 2025/09/04 16:32:19 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <map>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>


#include "Reponse.hpp"


class Request {
	public:
		Request();
		Request(const std::string &request);
		~Request();
		Request(const Request &other);
		
		Request &operator=(const Request &other);

		static Request parse(const std::string &buffer);

		std::string get_method() const;
		std::string get_url() const;

		Reponse handle_get();
		Reponse execute_cgi(std::string& path);
		Reponse handle_post();
		Reponse handle_delete();		

	private:
		std::string _method;       // GET, POST, DELETE, etc.
		std::string _url;          // /index.html, /script.py
		std::string _http_version; // HTTP/1.1
		std::map<std::string, std::string> _headers; // "Host" -> "localhost:8080"
		std::string _body;         // Corps de la requête (POST data)

		// Pour le parsing progressif si tu utilises recv non bloquant
		std::string buffer;       // tampon où tu accumules ce que tu lis
		bool complete; 
};
