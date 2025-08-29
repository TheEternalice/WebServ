/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   File.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gebz <gebz@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 15:59:04 by lde-merc          #+#    #+#             */
/*   Updated: 2025/08/25 17:43:18 by gebz             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include "Config.hpp"
#include "../Utils.hpp"

struct ServerBlock;

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
		void lexer_cpp(std::vector<std::string>& tokens, ServerBlock& server);
		
	private:
		std::string _name;
		std::vector<ServerBlock> _servers;
		char **_oss;

		void parsing_serv(std::ifstream& file, ServerBlock& server);
		
};
 