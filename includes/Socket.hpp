/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 12:26:30 by lde-merc          #+#    #+#             */
/*   Updated: 2025/10/03 13:25:58 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>
#include <map>
#include <string>

#include <poll.h>
#include <netinet/in.h>
#include <sys/stat.h>

#include "Reponse.hpp"

class Socket {
	public:
		Socket();
		~Socket();
		Socket(const Socket &other);
		Socket &operator=(const Socket &other);
		
	private:
		int fd;
		struct sockaddr_in address;
		socklen_t _addrlen;
		int _port;
		std::string _host;
		std::string _server_name;
		std::map<int, std::string> _error_pages;
		size_t _max_body_size;
		std::string _path;
		std::string _index;
		std::string _root;
		bool _autoIndex;
		std::string _returnPath;
		std::string _upload_dir;

		std::vector<std::string> _allowedMethods;
		std::vector<std::string> _cgiExtensions;
		std::map<int, Reponse> _errorsResponses;
};
