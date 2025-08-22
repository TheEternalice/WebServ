/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   File.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gebz <gebz@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 15:59:09 by lde-merc          #+#    #+#             */
/*   Updated: 2025/08/21 13:11:45 by gebz             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "File.hpp"

// Constructeur
File::File() {}

File::File(char *argv) {
	this->_name = std::string(argv[1]);
	
	
}

File::~File() {}

File::File(const File &other) {
    *this = other;
}

File &File::operator=(const File &other) {
    if (this != &other) {
        // copy attributes here
		this->_name = other._name;
		this->_oss = other._oss;
    }
    return *this;
}

File::openfile() {
    std::ifstream file(_name);
    if (!file.is_open){
        throw std::err << "Cannot open config file" + _name << std::endl; 
    }
}