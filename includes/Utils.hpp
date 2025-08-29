/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gebz <gebz@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 01:14:48 by gebz              #+#    #+#             */
/*   Updated: 2025/08/26 17:03:24 by gebz             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "parsing/File.hpp"

std::string trim(const std::string& str);
std::vector<std::string> cpp_split(const std::string& line, char c);