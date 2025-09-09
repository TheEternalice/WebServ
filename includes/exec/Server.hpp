/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gebz <gebz@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 15:47:19 by lde-merc          #+#    #+#             */
/*   Updated: 2025/09/09 13:19:39 by gebz             ###   ########.fr       */
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
#include <map>
#include "../Utils.hpp"


struct LocationConfig
{
	std::string location_root;
	std::vector<std::string> allowedMethods;
	std::string returnPath;
	std::vector<std::string> cgiExtensions;
	bool autoIndex;
	size_t struct_max_body_size;
	std::string path;
	std::string location_index;
	std::string upload_dir;
};

class Server {
	public:
		Server();
		~Server();
		Server(const Server &other);

		Server &operator=(const Server &other);

		class Badextention: public std::exception {
			public:
				virtual const char* what() const throw();
		};
		class CannotBeOpen: public std::exception {
			public:
				virtual const char* what() const throw();
		};

		bool parsing(std::string name);
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


		void location_root(std::vector<std::string> tokens, LocationConfig& loc);
		void location_methods(std::vector<std::string> tokens, LocationConfig& loc);
		void location_return(std::vector<std::string> tokens, LocationConfig& loc);
		void location_cgi(std::vector<std::string> tokens, LocationConfig& loc);
		void location_autoindex(std::vector<std::string> tokens, LocationConfig& loc);
		void location_max_size(std::vector<std::string> tokens, LocationConfig& loc);
		void location_index(std::vector<std::string> tokens, LocationConfig& loc);
		void location_upload_dir(std::vector<std::string> tokens, LocationConfig& loc);
		void display_Serv();

		void init();
		void run();

	private:
		std::vector<Server> _serv;
		std::string _host;
		int	_port;
		std::vector<std::string> _server_name;
		size_t _max_body_size;
		std::string _root;
		std::string _index;
		std::map<int, std::string> _error_pages;
		std::vector<LocationConfig> find_element_route;

		int _fd;
		struct sockaddr_in* _address;

};
