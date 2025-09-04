/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 15:47:19 by lde-merc          #+#    #+#             */
/*   Updated: 2025/09/04 14:34:52 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <poll.h>
#include <vector>
#include <sys/socket.h>
#include <poll.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <cerrno>
#include <fcntl.h>
#include "Request.hpp"
#include "Reponse.hpp"

struct ServerSocket {
    int fd;
    struct sockaddr_in address;
	socklen_t _addrlen;
	int _port;
};

class Server {
	public:
		Server();
		~Server();
		Server(const Server &other);
		
		Server &operator=(const Server &other);

		void init();
		void run();
		void accept_client(ServerSocket &s);
		void handle_request(int i);
		bool is_method_allowed(const std::string &method);
		static std::string get_content_type(const std::string& path);
		
	private:
		// std::vector<int> _port;
		std::vector<struct pollfd> _fds;
		std::vector<ServerSocket> sockets;
		static std::map<int, Reponse> _static_responses; //Global answers : 404, 405, 500
		std::vector<std::string> _allowedMethods;
		std::vector<std::string> _cgiExtensions;
		static std::map<std::string, std::string> _extensionsToType;
};
