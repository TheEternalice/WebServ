/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   File.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gebz <gebz@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 15:59:04 by lde-merc          #+#    #+#             */
/*   Updated: 2025/08/21 13:05:00 by gebz             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <iostream>

class File {
	public:
		File();
		File(char *argv);
		~File();
		File(const File &other);
		
		File &operator=(const File &other);

	private:
		std::string _name;
		char **_oss;

		void openfile();
};
