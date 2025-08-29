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

void extract_listen(std::vector<std::string>& tokens, ServerBlock& server) {
	if (tokens.size() < 2)
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

void extract_serverName(std::vector<std::string>& tokens, ServerBlock& server) {
	std::string name = tokens[1];
	if (!name.empty() && name[name.length() - 1] == ';'){
		name = name.substr(0, name.length() - 1);
	}
	server.server_name.push_back(name);
}
void extract_maxBodySyze(std::vector<std::string>& tokens, ServerBlock& server) {
	std::string value = tokens[1];
}
void extract_errorPage(std::vector<std::string>& tokens, ServerBlock& server) {
	std::string value = tokens[1];
}
void extract_location(std::vector<std::string>& tokens, ServerBlock& server) {
	std::string value = tokens[1];
}
