/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   File.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gebz <gebz@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 15:59:04 by lde-merc          #+#    #+#             */
/*   Updated: 2025/08/25 01:52:39 by gebz             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include "parsing/LocationConfig.hpp"
#include "Utils.hpp"

struct ServerBlock
{
	std::string host; 
	int	port;
	std::vector<std::string> error_pages;
	std::vector<NotPrimarieElement> npe;
};

class File {
	public:
		File();
		File(std::string& file_name);
		~File();
		File(const File &other);
		
		File &operator=(const File &other);

		bool openfile();
		bool check_extention();
		class Badextention: public std::exception {
			public:
				virtual const char* what() const throw();
		};
		class CannotBeOpen: public std::exception {
			public:
				virtual const char* what() const throw();
		};
		
	private:
		std::string _name;
		std::vector<std::string> _servers;
		char **_oss;

		void parsing_serv(std::ifstream& file, ServerBlock& server);
		
};
 