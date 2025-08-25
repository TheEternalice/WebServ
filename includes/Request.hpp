/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 16:26:29 by lde-merc          #+#    #+#             */
/*   Updated: 2025/08/14 16:28:07 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <map>

class Request {
	public:
		Request();
		~Request();
		Request(const Request &other);
		
		Request &operator=(const Request &other);

		std::string get_content_type(const std::string &path);
		static Request make_404();
		static Request make_405();
		static Request make_500();

	private:
		std::string method;       // GET, POST, DELETE, etc.
		std::string url;          // /index.html, /script.py
		std::string http_version; // HTTP/1.1
		std::map<std::string, std::string> headers; // "Host" -> "localhost:8080"
		std::string body;         // Corps de la requête (POST data)
		
		// Pour la réponse
		std::string response_headers;
		std::string response_body;

		// Pour le parsing progressif si tu utilises recv non bloquant
		std::string buffer;       // tampon où tu accumules ce que tu lis
		bool complete; 
};
