/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-rese <ade-rese@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 14:14:08 by ade-rese          #+#    #+#             */
/*   Updated: 2025/08/21 14:14:08 by ade-rese         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "LocationConfig.hpp"
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>

class ServerConfig
{
	private:
		size_t _listen;
		std::string _server_name;
		std::string _root;
		std::vector<std::string> _index;
		size_t _client_max_body_size;
		std::map<int, std::string> _error_pages;
		std::vector<LocationConfig> _location;
	public:
		ServerConfig();
		ServerConfig(ServerConfig const &ServerConfig);
		ServerConfig &operator=(ServerConfig const &ServerConfig);
		~ServerConfig();
};
