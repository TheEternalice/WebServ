/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 15:47:12 by lde-merc          #+#    #+#             */
/*   Updated: 2025/09/09 13:41:16 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"


std::map<int, Reponse> Server::_static_responses;
std::map<std::string, std::string> Server::_extensionsToType;

// Constructeur
Server::Server() {
	// for (int i = 0; i < 4; i++) {
	// 	ServerSocket s;
	// 	s._port = 8080 + i;
	// 	sockets.push_back(s);
	// }
	// _static_responses[404] = Reponse::make_404();
	// _static_responses[405] = Reponse::make_405();
	// _static_responses[500] = Reponse::make_500();
	// _allowedMethods.push_back("GET");
	// _allowedMethods.push_back("POST");
	// _allowedMethods.push_back("DELETE");
	// _extensionsToType[".html"] = "text/html";
	// _extensionsToType[".css"]  = "text/css";
	// _extensionsToType[".js"]   = "application/javascript";
	// _extensionsToType[".jpg"]  = "image/jpeg";
	// _extensionsToType[".png"]  = "image/png";
}

Server::~Server() {}

Server::Server(const Server &other) {
    *this = other;
}

Server &Server::operator=(const Server &other) {
    if (this != &other) {
        // copy attributes here
    }
    return *this;
}

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
			ServerSocket serv;
			_sockets.push_back(serv);
			parsing_serv(file);
		}
	}
	file.close();
	return (!this->_sockets.empty());
}


// DEBUT DE l'extract 1

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

	for (int i = 0; i < 7; i++) {
		if (tokens[0] == instruction[i]) (this->*function[i])(tokens, file);
	}
}

// DEBUT DE l'EXTRACT 2
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
		_sockets.back()._allowedMethods.push_back(method);
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

void Server::display_Serv() {
	for(size_t i = 0; i < _sockets.size(); i++) {
		std::cout << "------- Server " << i + 1 << " -------" << std::endl;
		std::cout << "Host : ";
		if (!_sockets[i]._host.empty())
			std::cout << _sockets[i]._host << std::endl;
		std::cout << "Port : ";
		if (_sockets[i]._port)
			std::cout << _sockets[i]._port << std::endl;
		std::cout << "Server name : ";
		if (!_sockets[i]._server_name.empty())
			std::cout << _sockets[i]._server_name << std::endl;
		std::cout << "Error pages : " << std::endl;
		if (!_sockets[i]._error_pages.empty()) {
			for (size_t j = 0; j < 600; j++){
				if (_sockets[i]._error_pages[j] != "")
					std::cout << "   _ " << _sockets[i]._error_pages[j] << std::endl;	
			}
		}
		std::cout << std::endl;
		std::cout << "Max body size : ";
		if (!_sockets[i]._max_body_size)
			std::cout << _sockets[i]._max_body_size << std::endl;
		std::cout << "Path : ";
		if (!_sockets[i]._path.empty())
			std::cout << _sockets[i]._path << std::endl;
		std::cout << "Index : ";
		if (!_sockets[i]._index.empty())
			std::cout << _sockets[i]._index << std::endl;
		std::cout << "Root : ";
		if (!_sockets[i]._root.empty())
			std::cout << _sockets[i]._root << std::endl;
		std::cout << "Auto index : ";
		if (_sockets[i]._autoIndex)
			std::cout << _sockets[i]._autoIndex << std::endl;
		std::cout << "Allowed Methods : " << std::endl;
		if (!_sockets[i]._allowedMethods.empty()) {
			for (size_t k = 0; k < _sockets[i]._allowedMethods.size(); k++)
				std::cout << "   _ " << _sockets[i]._allowedMethods[k] << std::endl;
		}
		std::cout << std::endl << "CGI Extensions : " << std::endl;
		if (!_sockets[i]._cgiExtensions.empty()) {
			for (size_t l = 0; l < _sockets[i]._cgiExtensions.size(); l++)
				std::cout << "   " << l << " : " << _sockets[i]._cgiExtensions[l] << std::endl;
		}
		std::cout << "Path : " << std::endl;
		if (!_sockets[i]._returnPath.empty())
			std::cout << _sockets[i]._returnPath << std::endl;
		std::cout << "Upload dir : " << std::endl;
		if (!_sockets[i]._upload_dir.empty())
			std::cout << _sockets[i]._upload_dir << std::endl;
		std::cout << "------------------------" << std::endl;
	}
}


std::string Server::get_content_type(const std::string& path) {
	std::string::size_type dot = path.rfind('.');
	if (dot == std::string::npos)
		return "application/octet-stream";

	std::string ext = path.substr(dot);
	std::map<std::string, std::string>::const_iterator it =_extensionsToType.find(ext);

	if (it != _extensionsToType.end())
		return it->second;
	else
		return "application/octet-stream";
}

void Server::init() {
	for(int i = 0; i < (int)_sockets.size(); i++) {
		_sockets[i].fd = socket(AF_INET, SOCK_STREAM, 0);
		if (_sockets[i].fd < 0) {
			std::cerr << "Failed to create socket" << std::endl;
			exit(EXIT_FAILURE);
		}
		// Set the socket to non-blocking mode
		// This allows the server to handle multiple clients without blocking
		int flags = fcntl(_sockets[i].fd, F_GETFL, 0);
		if (flags == -1) { std::cerr << "fcntl F_GETFL" << std::endl; exit(1); }
		if (fcntl(_sockets[i].fd, F_SETFL, flags | O_NONBLOCK) == -1) {
			std::cerr << "fcntl F_SETFL" << std::endl;
			exit(1);
		}

		memset(&_sockets[i].address, 0, sizeof(_sockets[i].address));
		_sockets[i].address.sin_family = AF_INET;
		_sockets[i].address.sin_addr.s_addr = INADDR_ANY;
		_sockets[i].address.sin_port = htons(_sockets[i]._port);

		int opt = 1;
		if (setsockopt(_sockets[i].fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
			std::cerr << "setsockopt failed" << std::endl;
			close(_sockets[i].fd);
			exit(EXIT_FAILURE);
		}

		if (bind(_sockets[i].fd, (struct sockaddr *)&_sockets[i].address, sizeof(_sockets[i].address)) < 0) {
			std::cerr << "Failed to bind socket" << std::endl;
			for(int j = 0; j <= i; j++)
				close(_sockets[j].fd);
			exit(EXIT_FAILURE);
		}

		if (listen(_sockets[i].fd, 5) < 0) {
			std::cerr << "Failed to listen on socket" << std::endl;
			for(int j = 0; j <= i; j++)
				close(_sockets[j].fd);
			exit(EXIT_FAILURE);
		}
	}
}

void Server::run() {
	for(size_t i = 0; i < _sockets.size(); i++) {
		struct pollfd pfd;
		// pfd = {_sockets[i].fd, POLLIN, 0}; // C++98 style, fd, events, revents
		pfd.fd = _sockets[i].fd;
		pfd.events = POLLIN;
		pfd.revents = 0;
		_fds.push_back(pfd);
	}
	
	while (true) {
		int ret = poll(&_fds[0], _fds.size(), 100);
		if (ret < 0) {
			if (errno == EINTR)  continue;
			std::cerr << "Poll error" << std::endl;
			exit(EXIT_FAILURE);
		}
		if (ret == 0) continue;
		
		for(int i = 0; i < (int)_sockets.size(); i++){
			if (_fds[i].revents & POLLIN)
				accept_client(_sockets[i]);
		}
		for(size_t i = _sockets.size(); i < _fds.size(); i++) {
			if (_fds[i].revents & POLLIN) {
				handle_request(i);
				i--; // on erase le client, donc on decremente l'index
			}
		}
	}
}

void Server::accept_client(ServerSocket& s) {
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);

	int client_fd = accept(s.fd, (struct sockaddr *)&client_addr, &client_len);
	if (client_fd < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return; // rien à accepter
		std::cerr << "accept() failed on port " << s._port
				<< ": " << strerror(errno) << std::endl;
		return;
	}

	std::cout << "Client connected on port " << s._port << std::endl;

	struct pollfd pfc;
	pfc.fd = client_fd;
	pfc.events = POLLIN;
	pfc.revents = 0;
	_fds.push_back(pfc);
}

void Server::handle_request(int i) {
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	int bytes_read = recv(_fds[i].fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes_read < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			// Pas encore de données, ce n'est pas une erreur
			return; // ou continue dans la boucle poll
		} else {
			// Erreur réelle
			std::cerr << "Failed to read from client" << std::endl;
			close(_fds[i].fd);
			_fds.erase(_fds.begin() + i);
			return;
		}
	} else if (bytes_read == 0) {
			// Client a fermé la connexion
			std::cout << "Client disconnected" << std::endl;
			close(_fds[i].fd);
			_fds.erase(_fds.begin() + i);
			return;
	} else {
		buffer[bytes_read] = '\0';
		std::cout << "Received request\n";
		// std::cout << "Received request:\n" << buffer << std::endl;
		Request req = Request(buffer);
		// std::cout << "body = " << req.get_body() << std::endl;
		Reponse res;
		if (is_method_allowed(req.get_method())) {
			// Handle the request
			switch(req.get_method()[0]) {
				case 'G': {
					res = req.handle_get();
					break;
				} case 'P': {
					res = req.handle_post();
					break;
				} case 'D': {
					res = req.handle_delete();
					break;
				} default:
					res = _static_responses[405];
					break;
			}
			
			std::string response = res.to_string();
			send(_fds[i].fd, response.c_str(), response.size(), 0);
		}
		// close(_fds[i].fd);
	}
}

bool Server::is_method_allowed(const std::string& method) {
	for (size_t i = 0; i < _sockets[i]._allowedMethods.size(); i++) {
		if (_sockets[i]._allowedMethods[i] == method)
			return true;
	}
	return false;
}