/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gebz <gebz@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 15:47:12 by lde-merc          #+#    #+#             */
/*   Updated: 2025/09/02 11:32:36 by gebz             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/exec/Server.hpp"
#include <fstream>

// Constructeur
Server::Server() {
}

Server::~Server() {
	// void stop() ?
}

Server::Server(const Server &other) {
    *this = other;
}

Server &Server::operator=(const Server &other) {
    if (this != &other) {
        // copy attributes here
    }
    return *this;
}

// debut
bool Server::check_extention(std::string name){
	size_t pos = name.find_last_of('.');
	if (name.compare(pos, 5, ".conf") == 0)
		return true;
	return false;
}
const char* Server::Badextention::what() const throw() {
	return ("Bad extention of file, need to be a .conf");
}

const char* Server::CannotBeOpen::what() const throw() {
    return ("file Cannot be oppen");
}

void Server::parsing_serv(std::ifstream& file) {
	std::string line;
	while (std::getline(file, line)) {
		trim(line);
		if (line[0] == '}')
			break;
		std::vector<std::string> tokens;
		tokens = cpp_split(line, ' ');
		if (tokens.empty())
			continue;
		lexer_cpp(tokens, file);
	}
}

bool Server::parsing(std::string name) {
    std::ifstream file(name.c_str());
	if (!file.is_open()){
        throw CannotBeOpen();
    }
	if (!check_extention(name)){
		throw(Badextention());
	}

	std::string line;
    while (std::getline(file, line)) {
		line = trim(line);
        if (line.empty() || line.at(0) == '#')
			continue;
		if (line == "server {") {
			Server serv;
			parsing_serv(file);
			_serv.push_back(serv);
		}
	}
	file.close();
	return (!this->_serv.empty());
}


// DEBUT DE l'extract 1



void Server::extract_listen(std::vector<std::string>& tokens, std::ifstream& file) {
	if (tokens.size() < 2 || !file)
		return;
	std::string value = tokens[1];
	if (!value.empty() && value[value.length() - 1] == ';'){
		value = value.substr(0, value.length() - 1);
	}
	//std::cout << value << std::endl;
	std::vector<std::string> if_host = cpp_split(value, ':');
	if (if_host.size() == 2) {
		_host = if_host[0];
		_port = atoi(if_host[1].c_str());
		//std::cout << server.host  << "\n" <<  server.port << std::endl;
	}
	else {
		_port = atoi(value.c_str());
		//std::cout << server.port << std::endl;
	}
}

void Server::extract_serverName(std::vector<std::string>& tokens, std::ifstream& file) {
	if (!file)
		return ;
	for (size_t i = 1; i < tokens.size(); i++){
		std::string name = tokens[i];
		if (!name.empty() && name[name.length() - 1] == ';'){
			name = name.substr(0, name.length() - 1);
		}
		_server_name.push_back(name);
	}
}

void Server::extract_root(std::vector<std::string>& tokens, std::ifstream& file) {
	if (tokens.size() < 2)
		return ;
	_root = tokens[1];
	if (!_root.empty() && _root[_root.length() - 1] == ';'){
		_root = _root.substr(0, _root.length() - 1);
	}
}

void Server::extract_index(std::vector<std::string>& tokens, std::ifstream& file) {
	if (tokens.size() < 2)
		return ;
	_index = tokens[1];
	if (!_index.empty() && _index[_index.length() - 1] == ';'){
		_index = _index.substr(0, _index.length() - 1);
	}
}
void Server::extract_maxBodySyze(std::vector<std::string>& tokens, std::ifstream& file) {
	if (!file)
		return ;
	std::string value = tokens[1];
	 if (!value.empty() && value[value.length() - 1] == ';') {
		value = value.substr(0, value.length() - 1);
	}
	_max_body_size = atoi(value.c_str());
}

void Server::extract_errorPage(std::vector<std::string>& tokens, std::ifstream& file) {
	if (tokens.size() < 3 || !file)
		return ;
	std::string code_str = tokens[1];
	std::string name_page = tokens[2];
	if (!code_str.empty() || name_page.empty()) {
		if (code_str[code_str.length() - 1] == ';')
			code_str = code_str.substr(0, code_str.length() -1);
		if (name_page[name_page.length() -1] == ';')
			name_page = name_page.substr(0, name_page.length() -1);
	}
	int code = atoi(code_str.c_str());
	_error_pages[code] = name_page;
}

void Server::lexer_cpp(std::vector<std::string>& tokens, std::ifstream& file) {
	std::string instruction[7] = {"listen", "server_name", "client_max_body_syze", "root", "index","error_page", "location"};

	void (Server::*function[7])(std::vector<std::string>&, std::ifstream&) = {
		&Server::extract_listen,
		&Server::extract_serverName,
		&Server::extract_maxBodySyze,
		&Server::extract_root,
		&Server::extract_index,
		&Server::extract_errorPage,
		&Server::extract_location
	};

	for (size_t i = 0; i < 5; i++){
		if (tokens[0] == instruction[i])
			(this->*function[i])(tokens, file);
	}
}

// DEBUT DE l'EXTRACT 2
void Server::location_root(std::vector<std::string> tokens) {
	if (tokens.size() < 2)
		return ;
	_root = tokens[1];
	if (!_root.empty() && _root[_root.length() - 1] == ';'){
		_root = _root.substr(0, _root.length() - 1);
	}
}

void Server::location_methods(std::vector<std::string> tokens){
	if (tokens.size() < 2)
		return ;
	for (size_t i = 1; i < tokens.size(); i++){
		std::string method = tokens[i];
		if (!method.empty() && method[method.length() - 1] == ';'){
			method = method.substr(0, method.length() - 1);
		}
		_allowedMethods.push_back(method);
	}
}

void Server::location_return(std::vector<std::string> tokens){
	if (tokens.size() < 2)
		return ;
	_returnPath = tokens[1];
	if (!_returnPath.empty() && _returnPath[_returnPath.length() - 1] == ';'){
		_returnPath = _returnPath.substr(0, _returnPath.length() - 1);
	}
}

void Server::location_cgi(std::vector<std::string> tokens) {
	if (tokens.size() < 2)
		return ;
	for (size_t i = 1; i < tokens.size(); i++){
	std::string cgi = tokens[i];
	if (!cgi.empty() && cgi[cgi.length() - 1] == ';'){
		cgi = cgi.substr(0, cgi.length() - 1);
		}
	_cgiExtensions.push_back(cgi);
	}
}

void Server::location_autoindex(std::vector<std::string> tokens){
	if (tokens.size() < 2)
		return ;
	std::string value = tokens[1];
	if (!value.empty() && value[value.length() - 1] == ';'){
		value = value.substr(0, value.length() - 1);
	}
	if (value == "on")
		_autoIndex = true;
	else if (value == "off")
		_autoIndex = false;
}

void Server::location_index(std::vector<std::string> tokens){
	if (tokens.size() < 2)
		return ;
	_index = tokens[1];
	if (!_index.empty() && _index[_index.length() - 1] == ';'){
		_index = _index.substr(0, _index.length() - 1);
	}
}

void Server::location_max_size(std::vector<std::string> tokens){
	if (tokens.size() < 2)
		return ;
	std::string value = tokens[1];
	if (!value.empty() && value[value.length() - 1] == ';'){
		value = value.substr(0, value.length() - 1);
	}
	_max_body_size = atoi(value.c_str());
}

void Server::location_upload_dir(std::vector<std::string> tokens){
	if (tokens.size() < 2)
		return ;
	std::string value = tokens[1];
	if (!value.empty() && value[value.length() - 1] == ';'){
		value = value.substr(0, value.length() - 1);
	}
	_upload_dir = value;
}


void Server::extract_location(std::vector<std::string>& tokens, std::ifstream& file) {
	if (tokens.size() < 2 || !file)
		return ;
	std::string line;

	_path = tokens[1];
	while (std::getline(file, line)) {
		line = trim(line);

		if (line == "}")
			break;
		else if (line.empty() || line[0] == '#')
			continue;
		std::vector<std::string> tokens;
		tokens = cpp_split(line, ' ');
		if (tokens.empty())
			continue;

		std::string instruction[8] = {"root", "allow_methods", "return", "cgi_extension", "autoindex",
			"client_max_body_size", "index", "upload_dir"};

		void (Server::*function[8])(std::vector<std::string>) = {
			&Server::location_root,
			&Server::location_methods,
			&Server::location_return,
			&Server::location_cgi,
			&Server::location_autoindex,
			&Server::location_max_size,
			&Server::location_index,
			&Server::location_upload_dir
		};

		for (size_t i = 0; i < 8; i++) {
		if (tokens[0] == instruction[i])
			(this->*function[i])(tokens);
		}
	}
}


void Server::init() {

	this->_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_fd < 0) {
		std::cerr << "Failed to create socket" << std::endl;
		exit(EXIT_FAILURE);
	}
	// Set the socket to non-blocking mode
	// This allows the server to handle multiple clients without blocking
	int flags = fcntl(this->_fd, F_GETFL, 0);
	if (flags == -1) { perror("fcntl F_GETFL"); exit(1); }
	if (fcntl(this->_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
		perror("fcntl F_SETFL");
		exit(1);
	}

	memset(this->_address, 0, sizeof(this->_address));
	this->_address->sin_family = AF_INET;
	this->_address->sin_addr.s_addr = INADDR_ANY;
	this->_address->sin_port = htons(this->_port);

	int opt = 1;
	if (setsockopt(this->_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		std::cerr << "setsockopt failed" << std::endl;
		close(this->_fd);
		exit(EXIT_FAILURE); // continue ? (plusieur serv possible dans le config)
	}

	if (bind(this->_fd, (struct sockaddr *)this->_address, sizeof(*this->_address)) < 0) {
		std::cerr << "Failed to bind socket" << std::endl;
		close(this->_fd);
		exit(EXIT_FAILURE); // continue ? (plusieur serv possible dans le config)
	}

	if (listen(this->_fd, 5) < 0) {
		std::cerr << "Failed to listen on socket" << std::endl;
		close(this->_fd);
		exit(EXIT_FAILURE);
	}
}

void Server::display_Serv() {
	if (!_host.empty())
		std::cout << _host << std::endl;
	if (_port)
		std::cout << _port << std::endl;
	if (!_server_name.empty()) {
		for (size_t i = 0; i < _server_name.size(); i++)
			std::cout << _server_name[i] << std::endl;
	}
	if (!_error_pages.empty()) {
		for (size_t j = 0; j < _error_pages.size(); j++)
			std::cout << _error_pages[0] << std::endl;	
	}
	if (!_max_body_size)
		std::cout << _max_body_size << std::endl;
	if (!_path.empty())
		std::cout << _path << std::endl;
	if (!_index.empty())
		std::cout << _index << std::endl;
	if (!_root.empty())
		std::cout << _root << std::endl;
	if (_autoIndex)
		std::cout << _autoIndex << std::endl;
		if (!_allowedMethods.empty()) {
			for (size_t k = 0; k < _allowedMethods.size(); k++)
				std::cout << _allowedMethods[k] << std::endl;
	}
	if (!_cgiExtensions.empty()) {
		for (size_t l = 0; l < _cgiExtensions.size(); l++)
			std::cout << _cgiExtensions[0] << std::endl;
	}
		
	if (!_returnPath.empty())
		std::cout << _returnPath << std::endl;
	if (!_upload_dir.empty())
		std::cout << _upload_dir << std::endl;

}

void Server::run() {

}