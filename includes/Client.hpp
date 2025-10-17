/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 16:07:09 by lde-merc          #+#    #+#             */
/*   Updated: 2025/10/06 16:42:15 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Utils.hpp"

#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include <cerrno>
#include <map>

class Client {
	public:
		Client();
		Client(int client_fd, struct sockaddr_in addr);
		Client(const Client &other);
		Client &operator=(const Client &other);
		~Client();

		// --- Communication ---
		void	readFromSocket();
		void	writeToSocket();

		// --- Request management ---
		bool	requestComplete() const;
		void	parseRequest();
		bool	hasHeader(const std::string &header) const;
		bool	outputEmpty() const;
		
		// --- Response management ---
		void	setResponse(const std::string &response);
		bool	responseReady() const;
		void	clear();

		// --- Getters ---
		int					getFd() const;
		const std::string	&getRequest() const;
		const std::string	&getMethod() const;
		const std::string	&getPath() const;
		const std::string	&getBody() const;
		const std::string	&getHeader(const std::string &header) const;
	private:
		int					_fd;
		struct sockaddr_in	_adrress;
		std::map<std::string, std::string> _headers;
		std::string			_requestBuffer;
		std::string			_responseBuffer;
		bool				_requestComplete;
		bool				_responseSent;
		std::string			_method;
		std::string			_url;
		std::string			_path;
		std::string			_httpVersion;
		std::string			_body;
};
