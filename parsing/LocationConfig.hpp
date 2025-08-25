/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-rese <ade-rese@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 14:14:37 by ade-rese          #+#    #+#             */
/*   Updated: 2025/08/21 14:14:37 by ade-rese         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>
#include <map>
#include <algorithm>

class LocationConfig
{
	private:
		std::string _path;
		std::string _root;
		bool _autoIndex;
		std::vector<std::string> _allowedMethods;
		std::vector<std::string> _cgiExtensions;
		std::string _returnPath
	public:
		LocationConfig();
		LocationConfig(LocationConfig const &LocationConfig);
		LocationConfig &operator=(LocationConfig const &LocationConfig);
		~LocationConfig();
};
