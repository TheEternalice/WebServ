/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 16:07:09 by lde-merc          #+#    #+#             */
/*   Updated: 2025/10/03 16:40:34 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <queue>
#include <netinet/in.h>

class Request;
class Response;

class Client {
	public:
		Client();
		~Client();

		Client(int fd, sockaddr_in& addr);
		int getFd() const;

		// Lecture/écriture
		bool readData();   // lit dans le buffer d'entrée
		bool writeData();  // écrit ce qu'il y a dans le buffer de sortie

		// Gestion des requêtes/réponses
		bool hasRequestReady() const;
		Request* popRequest();
		void pushResponse(Response* resp);

	private:
		int              _fd;
		struct sockaddr_in      _addr;

		std::string      _inBuffer;   // données reçues en attente de parsing
		std::string      _outBuffer;  // données prêtes à être envoyées

		std::queue<Request*>  _requests;   // requêtes décodées
		std::queue<Response*> _responses;  // réponses prêtes
};
