/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 15:54:02 by lde-merc          #+#    #+#             */
/*   Updated: 2025/09/09 14:28:58 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <stdexcept>
#include "../includes/Server.hpp"

static void print_error(std::string error)
{
	std::cerr << error << std::endl;
}

int main(int argc, char *argv[]) {
	std::string file_name;
	if (argc != 2)
		return (print_error("Bad argument"), -1);
	try {
		file_name = argv[1];
		Server serv;
		
		if (serv.parsing(file_name)) {
			std::cout << "Configuration file parsed successfully!" << std::endl;
			// serv.display_Serv();
		} else {
			std::cerr << "Failed to parse configuration file" << std::endl;
			return -1;
		}

		serv.init();
		serv.run();
	} catch (std::exception& e){
		std::cerr << "Error:" << e.what() << std::endl;
	}
	return 0;
}