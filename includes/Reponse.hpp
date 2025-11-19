/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reponse.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpichon <gpichon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 12:32:28 by lde-merc          #+#    #+#             */
/*   Updated: 2025/11/18 13:08:51 by gpichon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <map>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <sys/stat.h>
#include <dirent.h>
#include <ctime>

class Reponse {
	public:
		Reponse();
		Reponse(std::string url,std::string root, std::string index, std::string locationPath, bool autoIndex);
		Reponse(int num, std::string path);
		~Reponse();
		Reponse(const Reponse &other);

		Reponse &operator=(const Reponse &other);

		std::string to_string() const;

		static Reponse make_500();
		bool isKeepAlive() const;

		int get_status_code() const;
		std::string get_body() const;
		std::map<std::string, std::string> get_header() const;

		void set_status_code(int code);
		void set_status_text(const std::string& text);
		void set_body(const std::string& body);
		void set_header(const std::string& key, const std::string& value);

	private:
		int _status_code;
		std::string _status_text;
		std::map<std::string, std::string> _headers;
		std::string _body;
		std::string generateDirectoryListing(const std::string& dirPath, const std::string& url);
};
