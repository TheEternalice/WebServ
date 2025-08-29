/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   File.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gebz <gebz@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 15:59:09 by lde-merc          #+#    #+#             */
/*   Updated: 2025/08/26 15:17:12 by gebz             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/parsing/File.hpp"

// Constructeur
File::File() {}

File::File(std::string& file_name) {
	this->_name = file_name;
    this->_servers.clear();
}

File::~File() {}

File::File(const File &other) {
    *this = other;
}

File &File::operator=(const File &other) {
    if (this != &other) {
        // copy attributes here
		this->_name = other._name;
		this->_servers = other._servers;
		this->_oss = other._oss;
    }
    return *this;
}

bool File::check_extention(){
	size_t pos = this->_name.find_last_of('.');
	if (this->_name.compare(pos, 5, ".conf") == 0)
		return true;
	return false;
}
const char* File::Badextention::what() const throw() {
	return ("Bad extention of file, need to be a .conf");
}

const char* File::CannotBeOpen::what() const throw() {
    return ("file Cannot be oppen");
}

bool File::openfile() {
    std::ifstream file(_name.c_str());
	if (!file.is_open()){
        throw CannotBeOpen();
    }
	if (!check_extention()){
		throw(Badextention());
	}
	
	std::string line;
    while (std::getline(file, line))
    {
		line = trim(line);
        if (line.empty() || line.at(0) == '#')
			continue;
		if (line == "server {")
		{
			ServerBlock serv;
			parsing_serv(file, serv);
			_servers.push_back(serv);		}
    }
	file.close();
	return !this->_servers.empty();
}

void File::lexer_cpp(std::vector<std::string>& tokens, ServerBlock& server, std::ifstream& file) {
	std::string instruction[5] = {"listen", "server_name", "client_max_body_syze", "error_page", "location"};
		
	ExtractFunction function[5] = {
		&extract_listen,
		&extract_serverName,
		&extract_maxBodySyze,
		&extract_errorPage,
		&extract_location
	};
	
	for (size_t i = 0; i < 5; i++){
		if (tokens[0] == instruction[i])
			function[i](tokens, server, file);
	}
}

void File::parsing_serv(std::ifstream& file, ServerBlock& server) {
	std::string line;
	while (std::getline(file, line))
	{
		trim(line);
		if (line[0] == '}')
			break;
		std::vector<std::string> tokens;
		tokens = cpp_split(line, ' ');
		if (tokens.empty())
			continue;
		lexer_cpp(tokens, server, file);
	}
}

std::ostream& operator<<(std::ostream& os, const File& sparam) {
	(void)sparam;
	os << "Some configuration Text";
	return os;
}