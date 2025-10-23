/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 14:17:13 by lde-merc          #+#    #+#             */
/*   Updated: 2025/10/22 15:03:12 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Reponse.hpp"
#include "Request.hpp"
#include <sys/socket.h>
#include <poll.h>
#include <netinet/in.h>
#include <sys/stat.h>

class Client {
	public:
		Client();
		~Client();
		Client(const Client &other);
		Client(int fd);
		
		Client &operator=(const Client &other);

		void readFromSocket();
		bool tryParseRequest();
		void writeToSocket();
		bool outputEmpty();
		void resetForNextRequest();
		bool shouldClose() const;
		
		Request getRequest();
		std::string getBufferIn() {return _buffer_in;}
		std::string getBufferOut() {return _buffer_out;}
		Reponse getReponse() {return _reponse;}
		int get_fd();
		void setResponse(Reponse res);
		
	private:
		Reponse _reponse;
		Request _request;
		std::string _buffer_in;
		std::string _buffer_out;
		int	_fd;
		bool _closed;
};
