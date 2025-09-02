/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 16:26:29 by lde-merc          #+#    #+#             */
/*   Updated: 2025/09/02 12:35:26 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <map>

class Request {
	public:
		Request();
		Request(const std::string &request);
		~Request();
		Request(const Request &other);
		
		Request &operator=(const Request &other);

		std::string get_content_type(const std::string &path);
		static Request parse(const std::string &buffer);
		

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
