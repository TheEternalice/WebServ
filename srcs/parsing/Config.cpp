 /* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-rese <ade-rese@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 14:09:17 by ade-rese          #+#    #+#             */
/*   Updated: 2025/08/21 14:09:17 by ade-rese         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/parsing/Config.hpp"
#include <iostream>

void extract_listen(std::vector<std::string>& tokens, ServerBlock& server, std::ifstream& file) {
	if (tokens.size() < 2 || !file)
		return;
	std::string value = tokens[1];
	if (!value.empty() && value[value.length() - 1] == ';'){
		value = value.substr(0, value.length() - 1);
	}
	//std::cout << value << std::endl;
	std::vector<std::string> if_host = cpp_split(value, ':');
	if (if_host.size() == 2) {
		server.host = if_host[0];
		server.port = atoi(if_host[1].c_str());
		//std::cout << server.host  << "\n" <<  server.port << std::endl;
	}
	else {
		server.port = atoi(value.c_str());
		//std::cout << server.port << std::endl;
	}
}

void extract_serverName(std::vector<std::string>& tokens, ServerBlock& server, std::ifstream& file) {
	if (!file)
		return ;
	for (size_t i = 1; i < tokens.size(); i++){
		std::string name = tokens[i];
		if (!name.empty() && name[name.length() - 1] == ';'){
			name = name.substr(0, name.length() - 1);
		}
		server.server_name.push_back(name);
	}
}

void extract_maxBodySyze(std::vector<std::string>& tokens, ServerBlock& server, std::ifstream& file) {
	if (!file)
		return ;
	std::string value = tokens[1];
	 if (!value.empty() && value[value.length() - 1] == ';') {
		value = value.substr(0, value.length() - 1);
	}
	server.max_body_size = atoi(value.c_str());
}

void extract_errorPage(std::vector<std::string>& tokens, ServerBlock& server, std::ifstream& file) {
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
	server.error_pages[code] = name_page;
}

void extract_location(std::vector<std::string>& tokens, ServerBlock& server, std::ifstream& file) {
	if (tokens.size() < 2 || !file)
		return ;
	LocationConfig loc;
	std::string line;

	loc.path = tokens[1];
	while (std::getline(file, line)) {
		line = trim(line);

		if (line == "}")
			break;
		else if (line.empty() || line[0] == '#')
			continue;
		std::vector<std::string> tokens;
		std::vector<std::string> tokens;
		tokens = cpp_split(line, ' ');
		if (tokens.empty())
			continue;
		
		std::string instruction[8] = {"root", "allow_methods", "return", "cgi_extension", "autoindex",
			"client_max_body_size", "index", "upload_dir"};

		SecondExtractFunction function[8] = {
			&location_root,
			&location_methods,
			&location_return,
			&location_cgi,
			&location_autoindex,
			&location_max_size,
			&location_index,
			&location_upload_dir
		};

		for (size_t i = 0; i < 8; i++) {
		if (tokens[0] == instruction[i])
			function[i](loc, tokens);
		}
	}
}
