/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 16:26:29 by lde-merc          #+#    #+#             */
/*   Updated: 2025/08/14 16:28:07 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

class Request {
	public:
		Request();
		~Request();
		Request(const Request &other);
		
		Request &operator=(const Request &other);

		std::string get_content_type(const std::string &path);
		
	private:
		char _method[8];
		char _url[1024];
		std::string _type;
};
