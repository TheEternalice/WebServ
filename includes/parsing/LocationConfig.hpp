/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-rese <ade-rese@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 14:14:37 by ade-rese          #+#    #+#             */
/*   Updated: 2025/08/26 17:10:58 by gebz             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>
#include <map>
#include <algorithm>
#include <string>

struct LocationConfig
{
	std::string path;
	std::string index;
	std::string root;
	bool autoIndex;
	std::vector<std::string> allowedMethods;
	std::vector<std::string> cgiExtensions;
	std::string returnPath;
	std::string upload_dir;
	size_t max_body_size;
};

typedef void (*SecondExtractFunction)(LocationConfig& loc, std::vector<std::string> tokens);

void location_root(LocationConfig& loc, std::vector<std::string> tokens);
void location_methods(LocationConfig& loc, std::vector<std::string> tokens);
void location_return(LocationConfig& loc, std::vector<std::string> tokens);
void location_cgi(LocationConfig& loc, std::vector<std::string> tokens);
void location_autoindex(LocationConfig& loc, std::vector<std::string> tokens);
void location_max_size(LocationConfig& loc, std::vector<std::string> tokens);
void location_index(LocationConfig& loc, std::vector<std::string> tokens);
void location_upload_dir(LocationConfig& loc, std::vector<std::string> tokens);
