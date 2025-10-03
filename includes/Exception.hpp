/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exception.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 12:41:51 by lde-merc          #+#    #+#             */
/*   Updated: 2025/10/03 13:09:32 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <stdexcept>
#include <string>

// Exception générique pour le serveur
class ServerException : public std::runtime_error {
public:
    explicit ServerException(const std::string& msg)
        : std::runtime_error("Server error: " + msg) {}
};

// Exception liée aux sockets
class SocketException : public std::runtime_error {
public:
    explicit SocketException(const std::string& msg)
        : std::runtime_error("Socket error: " + msg) {}
};

// Exception liée aux fichiers de config
class ConfigException : public std::runtime_error {
public:
    explicit ConfigException(const std::string& msg)
        : std::runtime_error("Config error: " + msg) {}
};

// Exception liée aux requêtes HTTP
class HttpRequestException : public std::runtime_error {
public:
    explicit HttpRequestException(const std::string& msg)
        : std::runtime_error("HTTP request error: " + msg) {}
};
