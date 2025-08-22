/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gebz <gebz@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 15:54:02 by lde-merc          #+#    #+#             */
/*   Updated: 2025/08/19 15:30:15 by gebz             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <stdexcept>
#include "File.hpp"
#include "Server.hpp"

int main(int argc, char *argv[]) {
	try {
		File conf_file(argv[1]);
		Server serv = Server();

		serv.init();
		serv.run();
	} catch (std::exception& e){
		std::cout << e.what() << std::endl;
	}
	return 0;
}
