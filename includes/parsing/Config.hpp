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

#include "ServerConfig.hpp"
#include <vector>

class Config
{
	private:
		std::vector<ServerConfig> _servers;
	public:
		Config();
		Config(Config const &Config);
		Config &operator=(Config const &Config);
		~Config();
};
