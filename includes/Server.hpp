/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 15:47:19 by lde-merc          #+#    #+#             */
/*   Updated: 2025/09/02 16:10:19 by lde-merc         ###   ########.fr       */
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

class Server {
	public:
		Server();
		~Server();
		Server(const Server &other);
		
		Server &operator=(const Server &other);

		void init();
		void run();
		void accept_client();
		void handle_request(int i);
		bool is_method_allowed(const std::string &method);
		
	private:
		int _port;
		int _fd;
		std::vector<struct pollfd> _fds;
		struct sockaddr_in _address;
		socklen_t _addrlen;
		static std::map<int, Request> _static_responses; //Global answers : 404, 405, 500
		std::vector<std::string> _allowedMethods;
		std::vector<std::string> _cgiExtensions;
		
};
