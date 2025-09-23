/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 15:54:02 by lde-merc          #+#    #+#             */
/*   Updated: 2025/09/23 16:54:09 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <stdexcept>
#include "../includes/Server.hpp"

// static void print_error(std::string error)
// {
// 	std::cerr << error << std::endl;
// }

int main(int argc, char *argv[]) {
	std::string file_name;
	try {
		if (argc != 2)
			throw std::out_of_range("Not good amount of argument");
		file_name = argv[1];
		Server serv;
		
		serv.parsing(file_name);
		std::vector<ServerSocket> so = serv.get_Socket();
		int n = so.size();
		for (int i = 0; i < n - 1; i++) {
			for(int j = i + 1; j < n; j++) {
				if (so[i]._port == so[j]._port)
					throw std::runtime_error("Multiple server with the same port");
			}
		}
		serv.display_Serv();

		serv.init();
		serv.run();
	} catch (std::exception& e){
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}