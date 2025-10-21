/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 14:17:13 by lde-merc          #+#    #+#             */
/*   Updated: 2025/10/20 16:08:06 by lde-merc         ###   ########.fr       */
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
		void setResponse(std::string res);
		
	private:
		Reponse _reponse;
		Request _request;
		std::string _buffer_in;
		std::string _buffer_out;
};
