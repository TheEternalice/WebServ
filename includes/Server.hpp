/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 15:47:19 by lde-merc          #+#    #+#             */
/*   Updated: 2025/10/21 17:14:42 by lde-merc         ###   ########.fr       */
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

#include <cerrno>
#include <fcntl.h>
#include <stdexcept>

#include "Request.hpp"
#include "Reponse.hpp"
#include "Utils.hpp"
#include "Client.hpp"

struct ServerSocket {
	int fd;
	int _port;
	
	std::string _host;
	std::string _server_name;
	std::map<int, std::string> _error_pages;
	size_t _max_body_size;
	std::string _path;
	std::string _index;
	std::string _root;
	std::string _returnPath;
	std::string _upload_dir;
	
	bool _autoIndex;
	
	struct sockaddr_in address;
	socklen_t _addrlen;
	
	std::map<std::string, int> _allowedMethods; // location et methodes en bit
	std::vector<std::string> _cgiExtensions;
	std::map<int, Reponse> _autoResponse;
};

class Server {
	public:
		Server();
		~Server();
		Server(const Server &other);
		
		Server &operator=(const Server &other);

		/******************
		 * Exception
		 ******************/
		class Badextention: public std::exception {
			public:
				virtual const char* what() const throw();
		};
		class CannotBeOpen: public std::exception {
			public:
				virtual const char* what() const throw();
		};

		/******************clientServer;
		 * Parsing methods
		 ******************/

		void parsing(std::string name);
		bool check_extention(std::string name);
		void lexer_cpp(std::vector<std::string>& tokens, std::ifstream& file);
		void parsing_serv(std::ifstream& file);

		void extract_listen(std::vector<std::string>& tokens, std::ifstream& file);
		void extract_serverName(std::vector<std::string>& tokens, std::ifstream& file);
		void extract_maxBodySyze(std::vector<std::string>& tokens, std::ifstream& file);
		void extract_root(std::vector<std::string>& tokens, std::ifstream& file);
		void extract_index(std::vector<std::string>& tokens, std::ifstream& file);
		void extract_errorPage(std::vector<std::string>& tokens, std::ifstream& file);
		void extract_location(std::vector<std::string>& tokens, std::ifstream& file);


		void location_root(std::vector<std::string> tokens);
		void location_methods(std::vector<std::string> tokens);
		void location_return(std::vector<std::string> tokens);
		void location_cgi(std::vector<std::string> tokens);
		void location_autoindex(std::vector<std::string> tokens);
		void location_max_size(std::vector<std::string> tokens);
		void location_index(std::vector<std::string> tokens);
		void location_upload_dir(std::vector<std::string> tokens);
		void display_Serv();
		
		/******************
		 * Server methods
		 ******************/
		void init();
		void run();
		// void accept_client(ServerSocket &s);
		void accept_client(int fd);
		// void handle_request(int i);
		void handle_request(Client& client);
		// bool is_method_allowed(const std::string &method);
		bool is_method_allowed(const std::string &method, Client &client);
		
		static std::string get_content_type(const std::string& path);
		std::vector<ServerSocket> get_Socket();
		void copy_socket(std::vector<ServerSocket> other);
		bool isServerSocket(int fd);
	private:
		static std::map<int, Reponse> _static_responses;
	
		std::vector<struct pollfd> 					_fds;
		std::vector<ServerSocket> 					_sockets;
		static std::map<std::string, std::string> 	_extensionsToType;
		std::map<int, ServerSocket> 				_listeningSockets;
		std::map<int, ServerSocket> 				_clientToSocket;
		std::map<int, Client*> 						_socketToClient;
};
