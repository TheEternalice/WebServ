/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_parsing.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 09:28:12 by lde-merc          #+#    #+#             */
/*   Updated: 2025/10/03 13:23:35 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/HTTP_Server.hpp"

void HTTP_Server::parsing(const std::string &configFile) {
	(void)configFile;
	// Lire le fichier de configuration
	// Valider la syntaxe
	// Extraire les directives globales et les blocs de socket
	// Pour chaque bloc de socket, créer un objet Socket et configurer ses paramètres
	// 		host, name, port, error_pages, max_body_size, path, index, root, autoIndex, return, upload_dir
	// 		allowedMethods, cgiExtensions, errorsResponses
	// Ajouter chaque Socket à la liste des sockets du serveur
	// Gérer les erreurs de configuration en renvoyant une exception ConfigException
}
