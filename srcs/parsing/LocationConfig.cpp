/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-rese <ade-rese@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 14:09:26 by ade-rese          #+#    #+#             */
/*   Updated: 2025/08/21 14:09:26 by ade-rese         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/parsing/LocationConfig.hpp"

void location_root(LocationConfig& loc, std::vector<std::string> tokens) {
	if (tokens.size() < 2)
		return ;
	loc.root = tokens[1];
	if (!loc.root.empty() && loc.root[loc.root.length() - 1] == ';'){
		loc.root = loc.root.substr(0, loc.root.length() - 1);
	}
}

void location_methods(LocationConfig& loc, std::vector<std::string> tokens){
	if (tokens.size() < 2)
		return ;
	for (size_t i = 1; i < tokens.size(); i++){
		std::string method = tokens[i];
		if (!method.empty() && method[method.length() - 1] == ';'){
			method = method.substr(0, method.length() - 1);
		}
		loc.allowedMethods.push_back(method);
	}
}

void location_return(LocationConfig& loc, std::vector<std::string> tokens){
	if (tokens.size() < 2)
		return ;
	loc.returnPath = tokens[1];
	if (!loc.returnPath.empty() && loc.returnPath[loc.root.length() - 1] == ';'){
		loc.returnPath = loc.returnPath.substr(0, loc.returnPath.length() - 1);
	}
}

void location_cgi(LocationConfig& loc, std::vector<std::string> tokens) {
	if (tokens.size() < 2)
		return ;
	for (size_t i = 1; i < tokens.size(); i++){
	std::string cgi = tokens[i];
	if (!cgi.empty() && cgi[cgi.length() - 1] == ';'){
		cgi = cgi.substr(0, cgi.length() - 1);
		}
	loc.cgiExtensions.push_back(cgi);
	}
}

void location_autoindex(LocationConfig& loc, std::vector<std::string> tokens){
	if (tokens.size() < 2)
		return ;
	std::string value = tokens[1];
	if (!value.empty() && value[value.length() - 1] == ';'){
		value = value.substr(0, value.length() - 1);
	}
	if (value == "on")
		loc.autoIndex = true;
	else if (value == "off")
		loc.autoIndex = false;
}

void location_index(LocationConfig& loc, std::vector<std::string> tokens){
	if (tokens.size() < 2)
		return ;
	loc.index = tokens[1];
	if (!loc.index.empty() && loc.index[loc.index.length() - 1] == ';'){
		loc.index = loc.index.substr(0, loc.index.length() - 1);
	}
}

void location_max_size(LocationConfig& loc, std::vector<std::string> tokens){
	if (tokens.size() < 2)
		return ;
	std::string value = tokens[1];
	if (!loc.upload_dir.empty() && loc.upload_dir[loc.upload_dir.length() - 1] == ';'){
		loc.upload_dir = loc.upload_dir.substr(0, loc.upload_dir.length() - 1);
	}
	loc.max_body_size = atoi(value.c_str());
}

void location_upload_dir(LocationConfig& loc, std::vector<std::string> tokens){
	if (tokens.size() < 2)
		return ;
	loc.upload_dir = tokens[1];
	if (!loc.upload_dir.empty() && loc.upload_dir[loc.upload_dir.length() - 1] == ';'){
		loc.upload_dir = loc.upload_dir.substr(0, loc.upload_dir.length() - 1);
	}
}
