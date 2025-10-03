/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reponse.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 12:32:35 by lde-merc          #+#    #+#             */
/*   Updated: 2025/10/03 13:17:38 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Reponse.hpp"


Reponse::Reponse() { }
Reponse::~Reponse() { }
Reponse::Reponse(const Reponse &other) { *this = other; }
Reponse &Reponse::operator=(const Reponse &other) {
	if (this != &other) {
		this->_status_code = other._status_code;
		this->_status_text = other._status_text;
		this->_headers = other._headers;
		this->_body = other._body;
	}
	return (*this);
}
