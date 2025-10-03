/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP_Server.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 15:47:12 by lde-merc          #+#    #+#             */
/*   Updated: 2025/10/03 14:33:15 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/HTTP_Server.hpp"

HTTP_Server::HTTP_Server() { }
HTTP_Server::~HTTP_Server() { }
HTTP_Server::HTTP_Server(const HTTP_Server &other) { *this = other; }

HTTP_Server &HTTP_Server::operator=(const HTTP_Server &other) {
	if (this != &other) {
		this->_sockets = other._sockets;
		this->_mimeTypes = other._mimeTypes;
		this->_clientServer = other._clientServer;
		this->_pollfds = other._pollfds;
	}
	return (*this);
}

HTTP_Server::HTTP_Server(const char *configFile) {
	parsing(std::string(configFile));

	load_mime_types();
}

void HTTP_Server::load_mime_types() {
	std::ifstream file("mime.types");
	if (!file.is_open()) throw ConfigException("Could not open mime.types");

	std::string line;
	while (std::getline(file, line)) {
		// Ignore comments et lignes vides
		if (line.empty() || line[0] == '#')
			continue;

		std::istringstream iss(line);
		std::string mime_type;
		if (!(iss >> mime_type))
			continue;

		std::string ext;
		while (iss >> ext) {
			if (ext[0] != '.')
				ext = "." + ext; // normaliser avec un point
			_mimeTypes[ext] = mime_type;
		}
	}
	file.close();
}