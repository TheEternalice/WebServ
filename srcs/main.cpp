/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 15:54:02 by lde-merc          #+#    #+#             */
/*   Updated: 2025/08/14 16:39:25 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <stdexcept>
// #include "../includes/File.hpp"
#include "../includes/Server.hpp"



int main(int argc, char *argv[]) {
	try {
		// File conf_file(argv[1]);
		std::cout << "Hello World!" << std::endl;
		Server serv = Server();

		std::cout << "Middle Earth!" << std::endl;

		serv.init();
		std::cout << "End World!" << std::endl;
		// serv.run();
	} catch (std::exception& e){
		std::cout << e.what() << std::endl;
	}
	return 0;
}