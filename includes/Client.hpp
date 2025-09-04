/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 15:47:16 by lde-merc          #+#    #+#             */
/*   Updated: 2025/09/04 09:07:58 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <ctime>
#include <string>

#include "Request.hpp"

class Client {
	public:
		Client();
		Client(int fd_) : _fd(fd_), _file_fd(-1), _file_offset(0), _file_size(0), _sending(false), _last_activity(time(NULL)) {}
		~Client();
		Client(const Client& other);
		
		Client &operator=(const Client& other);

	private:
		int _fd;
		int _file_fd;
		off_t _file_offset;
		off_t _file_size;
		bool _sending;
		std::string _header;
		time_t _last_activity;
		Request _curretnReq;
};
