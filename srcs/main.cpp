/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gebz <gebz@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 15:54:02 by lde-merc          #+#    #+#             */
/*   Updated: 2025/08/25 17:45:09 by gebz             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <stdexcept>
#include "../includes/parsing/File.hpp"
#include "../includes/exec/Server.hpp"

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
		File config(file_name);
		if (!config.openfile())
			return (print_error("Failed to load Configuration files"), -1);
	//	Server serv = Server();

	//	serv.init();
	//	serv.run();
	} catch (std::exception& e){
		std::cerr << "Error:" << e.what() << std::endl;
	}
	return 0;
}
