/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-rese <ade-rese@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 15:54:02 by lde-merc          #+#    #+#             */
/*   Updated: 2025/10/27 13:14:33 by ade-rese         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include <iostream>
#include <stdexcept>
#include <unistd.h>

extern Server* g_server;

int	main(int argc, char *argv[])
{
	std::string file_name;
	try {
		if (argc != 2)
			file_name = "./conf_file/valid_file/maximal_valid.conf";
		else
			file_name = argv[1];
		Server serv;
		g_server = &serv;

		std::signal(SIGINT, handle_sigint);

		serv.parsing(file_name);
		std::vector<ServerSocket> so = serv.get_Socket();
		int n = so.size();
		for (int i = 0; i < n; i++) {
			if (i < n - 1){
				for (int j = i + 1; j < n; j++) {
					if (so[i]._port == so[j]._port)
						throw std::runtime_error("Multiple server with the same port");
					}
			}
			std::map<int, std::string>::const_iterator it;
			for (it = so[i]._error_pages.begin(); it != so[i]._error_pages.end(); ++it) {
				if (access(it->second.c_str(), F_OK) == 0) {
					try {
						so[i]._autoResponse[it->first] = Reponse(it->first, it->second);
					} catch (std::exception &e) {
						so[i]._autoResponse[it->first] = Reponse::createDefaultErrorResponse(it->first);
					}
				} else {
					so[i]._autoResponse[it->first] = Reponse::createDefaultErrorResponse(it->first);
				}
			}
			for (int errorCode = 400; errorCode < 600; errorCode++) {
				if (so[i]._autoResponse.find(errorCode) == so[i]._autoResponse.end()) {
					so[i]._autoResponse[errorCode] = Reponse::createDefaultErrorResponse(errorCode);
				}
			}
		}

		serv.copy_socket(so);
		serv.display_Serv();
		serv.init();
		serv.run();
	} catch (std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}
	return (0);
}
