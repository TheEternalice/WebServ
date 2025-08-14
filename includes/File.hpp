/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   File.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 15:59:04 by lde-merc          #+#    #+#             */
/*   Updated: 2025/08/14 16:40:06 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

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
};
