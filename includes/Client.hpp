/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-rese <ade-rese@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 14:17:13 by lde-merc          #+#    #+#             */
/*   Updated: 2025/10/21 16:37:54 by ade-rese         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Reponse.hpp"
#include "Request.hpp"

class Client {
	public:
		Client();
		~Client();
		Client(const Client &other);
		
		Client &operator=(const Client &other);

		void readFromSocket();
		bool tryParseRequest();
		void writeToSocket();
		bool outputEmpty();
		
		Request getRequest();
		int get_fd();
		void setResponse(std::string res);
		
	private:
		Reponse _reponse;
		Request _request;
		std::string _buffer_in;
		std::string _buffer_out;
		int	_fd;
};
