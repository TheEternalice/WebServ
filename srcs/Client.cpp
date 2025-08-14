/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 15:47:06 by lde-merc          #+#    #+#             */
/*   Updated: 2025/08/14 15:47:07 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

// Constructeur
Client::Client() {}

Client::~Client() {}

Client::Client(const Client &other) {
    *this = other;
}

Client &Client::operator=(const Client &other) {
    if (this != &other) {
        // copy attributes here
    }
    return *this;
}
