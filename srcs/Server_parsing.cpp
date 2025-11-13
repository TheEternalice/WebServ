/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_parsing.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpichon <gpichon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 09:28:12 by lde-merc          #+#    #+#             */
/*   Updated: 2025/11/13 13:33:46 by gpichon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

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

void Server::parsing(std::string name) {
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
			ServerSocket serv;
			serv.fd = -1;
			serv._root = "/";
			serv._port = 0;
			serv._max_body_size = 0;
			serv._autoIndex = false;
			serv._host = "";
			serv.returnCode = 0;
			serv._server_name = "";
			serv._path = "";
			serv._root = "";
			serv._returnPath = "";
			serv._upload_dir = "";
			serv._alias = "";
			memset(&serv.address, 0, sizeof(serv.address));
			serv._addrlen = 0;
			serv._allowedMethods["/"] = 0;
			_sockets.push_back(serv);
			parsing_serv(file);
		}
	}
	file.close();
	if (this->_sockets.empty())
		throw std::runtime_error("Empty file");
}


// START OF the extract 1

void Server::extract_listen(std::vector<std::string>& tokens, std::ifstream& file) {
	if (tokens.size() < 2 || !file)
		return;
	std::string value = tokens[1];
	if (!value.empty() && value[value.length() - 1] == ';'){
		value = value.substr(0, value.length() - 1);
	}
	std::vector<std::string> if_host = cpp_split(value, ':');
	if (if_host.size() == 2) {
		_sockets.back()._host = if_host[0];
		_sockets.back()._port = atoi(if_host[1].c_str());
	}
	else {
		_sockets.back()._port = atoi(value.c_str());
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
		_sockets.back()._server_name = name;
	}
}

void Server::extract_root(std::vector<std::string>& tokens, std::ifstream& file) {
	(void)file;
	if (tokens.size() < 2)
		return ;

	std::string root;
	root = tokens[1];
	if (!root.empty() && root[root.length() - 1] == ';'){
		root = root.substr(0, root.length() - 1);
	}

	_sockets.back()._root = root;
}

void Server::extract_index(std::vector<std::string>& tokens, std::ifstream& file) {
	(void)file;
	if (tokens.size() < 2)
		return ;

	std::string index;
	index = tokens[1];
	if (!index.empty() && index[index.length() - 1] == ';'){
		index = index.substr(0, index.length() - 1);
	}

	_sockets.back()._index = index;
}

void Server::extract_maxBodySyze(std::vector<std::string>& tokens, std::ifstream& file) {
	if (!file)
		return ;

	std::string value = tokens[1];
	 if (!value.empty() && value[value.length() - 1] == ';') {
		value = value.substr(0, value.length() - 1);
	}

	_sockets.back()._max_body_size = atoi(value.c_str());
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

	_sockets.back()._error_pages[code] = name_page;
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

	static int c = 0;
	for (int i = 0; i < 7; i++) {
		if (tokens[0] == "listen" || tokens[0] == "server_name") c++;
		if (tokens[0] == instruction[i]) (this->*function[i])(tokens, file);
	}
	if (c < 2) throw(std::runtime_error("No listen found or no server_name"));
}

// START OF the extract 2
void Server::location_root(std::vector<std::string> tokens) {
	if (tokens.size() < 2)
		return ;

	std::string root;
	root = tokens[1];
	if (!root.empty() && root[root.length() - 1] == ';'){
		root = root.substr(0, root.length() - 1);
	}

	_sockets.back()._root = root;
}

void Server::location_methods(std::vector<std::string> tokens){
	if (tokens.size() < 2)
		return ;

	for (size_t i = 1; i < tokens.size(); i++){
		std::string method = tokens[i];
		if (!method.empty() && method[method.length() - 1] == ';'){
			method = method.substr(0, method.length() - 1);
		}
		switch (method[0]) {
			case 'G':
				_sockets.back()._allowedMethods[_sockets.back()._path] += 1;
				break;
			case 'P':
				_sockets.back()._allowedMethods[_sockets.back()._path] += 2;
				break;
			case 'D':
				_sockets.back()._allowedMethods[_sockets.back()._path] += 4;
				break;
		}
	}
}


void Server::location_return(std::vector<std::string> tokens){
	if (tokens.size() < 2)
		return ;

	std::string path;
	path = tokens[1];
	if (!path.empty() && path[path.length() - 1] == ';'){
		path = path.substr(0, path.length() - 1);
	}

	_sockets.back()._returnPath = path;
}

void Server::location_cgi(std::vector<std::string> tokens) {
	if (tokens.size() < 2)
		return ;
	for (size_t i = 1; i < tokens.size(); i++){
		std::string cgi = tokens[i];
		if (!cgi.empty() && cgi[cgi.length() - 1] == ';') cgi = cgi.substr(0, cgi.length() - 1);
		_sockets.back()._cgiExtensions.push_back(cgi);
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
		_sockets.back()._autoIndex = true;
	else if (value == "off")
		_sockets.back()._autoIndex = false;
}

void Server::location_index(std::vector<std::string> tokens){
	if (tokens.size() < 2)
		return ;

	std::string index;
	index = tokens[1];
	if (!index.empty() && index[index.length() - 1] == ';'){
		index = index.substr(0, index.length() - 1);
	}

	_sockets.back()._index = index;
}

void Server::location_max_size(std::vector<std::string> tokens){
	if (tokens.size() < 2)
		return ;

	std::string value = tokens[1];
	if (!value.empty() && value[value.length() - 1] == ';'){
		value = value.substr(0, value.length() - 1);
	}

	_sockets.back()._max_body_size = atoi(value.c_str());
}

void Server::location_upload_dir(std::vector<std::string> tokens){
	if (tokens.size() < 2)
		return ;

	std::string value = tokens[1];
	if (!value.empty() && value[value.length() - 1] == ';'){
		value = value.substr(0, value.length() - 1);
	}

	_sockets.back()._upload_dir = value;
}


void Server::extract_location(std::vector<std::string>& tokens, std::ifstream& file) {
	if (tokens.size() < 2 || !file)
		return ;
	std::string line;

	_sockets.back()._path = tokens[1];
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
