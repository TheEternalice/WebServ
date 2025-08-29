/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-rese <ade-rese@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 14:05:31 by ade-rese          #+#    #+#             */
/*   Updated: 2025/08/21 14:05:31 by ade-rese         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include "../Utils.hpp"
#include "File.hpp"
#include "LocationConfig.hpp"

struct ServerBlock
{
	std::string host; 
	int	port;
	std::vector<std::string> server_name;
	std::vector<std::string> error_pages;
	size_t max_body_size;
	std::vector<LocationConfig> find_element_route;
};

typedef void (*ExtractFunction)(std::vector<std::string>& tokens, ServerBlock& server);

void extract_listen(std::vector<std::string>& tokens, ServerBlock& server);
void extract_serverName(std::vector<std::string>& tokens, ServerBlock& server);
void extract_maxBodySyze(std::vector<std::string>& tokens, ServerBlock& server);
void extract_errorPage(std::vector<std::string>& tokens, ServerBlock& server);
void extract_location(std::vector<std::string>& tokens, ServerBlock& server);

