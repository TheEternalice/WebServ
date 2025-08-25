/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 15:47:19 by lde-merc          #+#    #+#             */
/*   Updated: 2025/08/14 16:39:35 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
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
		
	private:
		int _port;
		int _fd;
		struct sockaddr_in _address;

		static std::map<int, Request> static_responses; //Global answers : 404, 405, 500
		
};
