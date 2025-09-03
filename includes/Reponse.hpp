/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reponse.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 12:32:28 by lde-merc          #+#    #+#             */
/*   Updated: 2025/09/03 13:26:20 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <map>
#include <sstream>

class Reponse {
	public:
		Reponse();
		Reponse(std::string method, std::string url);
		~Reponse();
		Reponse(const Reponse &other);
		
		Reponse &operator=(const Reponse &other);

		std::string to_string() const;

		static Reponse make_200(const std::string &body, const std::string &type="text/html");
		static Reponse make_404();
		static Reponse make_405();
		static Reponse make_500();

	private:
		int _status_code;
		std::string _status_text;
		std::map<std::string, std::string> _headers;
		std::string _body;
};

