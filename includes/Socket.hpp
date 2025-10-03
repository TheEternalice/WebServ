/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 12:26:30 by lde-merc          #+#    #+#             */
/*   Updated: 2025/10/03 15:04:06 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstring>

#include <poll.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <cerrno>

#include "Reponse.hpp"
#include "Exception.hpp"

class Socket {
	public:
		Socket();
		~Socket();
		Socket(const Socket &other);
		Socket &operator=(const Socket &other);

		void initialize_socket();
		
		
	private:
		int _fd;
		int _port;
		size_t _max_body_size;
		std::string _host;
		std::string _server_name;
		std::string _path;
		std::string _index;
		std::string _root;
		std::string _returnPath;
		std::string _upload_dir;
		bool _autoIndex;
		
		struct sockaddr_in _address;
		socklen_t _addrlen;
		
		std::vector<std::string> _allowedMethods;
		std::vector<std::string> _cgiExtensions;
		std::map<int, std::string> _error_pages;
		std::map<int, Reponse> _errorsResponses;
};
