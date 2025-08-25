/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   File.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 15:59:09 by lde-merc          #+#    #+#             */
/*   Updated: 2025/08/14 16:40:21 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/File.hpp"

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
