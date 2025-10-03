/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reponse.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 12:32:28 by lde-merc          #+#    #+#             */
/*   Updated: 2025/10/03 13:17:31 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <map>

class Reponse {
	public:
		Reponse();
		~Reponse();
		Reponse(const Reponse &other);
		Reponse &operator=(const Reponse &other);
		
	private:
		int _status_code;
		std::string _status_text;
		std::map<std::string, std::string> _headers;
		std::string _body;
};