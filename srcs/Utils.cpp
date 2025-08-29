/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gebz <gebz@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 01:13:21 by gebz              #+#    #+#             */
/*   Updated: 2025/08/26 17:11:58 by gebz             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <vector>
#include <sstream>

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) 
		return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::vector<std::string> cpp_split(const std::string& line, char c) {
	std::vector<std::string> sentence;
	size_t start = 0;
	size_t end = line.find_first_of(c);;
	
	while (end != std::string::npos){
		if (end != start)
			sentence.push_back(line.substr(start, end - start));
		start = end + 1;
		end = line.find_first_of(c, start);
	}
	if (start < std::string::npos)
			sentence.push_back(line.substr(start, end - start));
	return sentence;
}
