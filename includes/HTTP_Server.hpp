/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP_Server.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 12:20:34 by lde-merc          #+#    #+#             */
/*   Updated: 2025/10/03 16:48:27 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <fstream>
#include <sstream>


#include "Exception.hpp"
#include "Socket.hpp"
#include "Client.hpp"

class HTTP_Server {
	public:
		HTTP_Server();
		~HTTP_Server();
		HTTP_Server(const HTTP_Server &other);
		HTTP_Server &operator=(const HTTP_Server &other);

		HTTP_Server(const char *configFile);

		void parsing(const std::string &configFile);
		void load_mime_types();

		void initialize_sockets();
		void run();
		void accept_client(int fd);
		Client* getClientByFd(int fd);
		
	private:
		std::vector<Socket> _sockets;
		std::vector<struct pollfd> _pollfds;
		std::map<std::string, std::string> _mimeTypes;
		std::map<int, Socket*> _socketServer; // fd -> Socket*
		std::map<int, Client*> _clientServer; // fd -> Client*
};
