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
#include <string>

struct LocationConfig
{
	std::string path;
	std::string root;
	bool autoIndex;
	std::vector<std::string> allowedMethods;
	std::vector<std::string> cgiExtensions;
	std::string returnPath;
	size_t max_body_size;
};
