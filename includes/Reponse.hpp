/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reponse.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 12:32:28 by lde-merc          #+#    #+#             */
/*   Updated: 2025/10/22 10:49:13 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <map>
#include <sstream>
#include <fstream>
#include <iostream>

class Reponse {
	public:
		Reponse();
		Reponse(std::string url);
		Reponse(int num, std::string path);
		~Reponse();
		Reponse(const Reponse &other);
		
		Reponse &operator=(const Reponse &other);

		std::string to_string() const;

		static Reponse make_200(const std::string& body, const std::string& type);
		static Reponse make_404();
		static Reponse make_405();
		static Reponse make_500();

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
};

