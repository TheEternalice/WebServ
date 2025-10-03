/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 16:06:59 by lde-merc          #+#    #+#             */
/*   Updated: 2025/10/03 16:40:37 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"

Client::Client() { }
Client::~Client() { }
Client::Client(int fd, sockaddr_in& addr) : _fd(fd), _addr(addr) { }
int Client::getFd() const { return _fd; }
