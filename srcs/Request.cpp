/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 16:26:26 by lde-merc          #+#    #+#             */
/*   Updated: 2025/09/04 17:24:30 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Request.hpp"
#include "../includes/Server.hpp"

// Constructeur
Request::Request() {}

Request::Request(const std::string &request) {
	// Simple parsing, assumes well-formed request
	size_t method_end = request.find(' ');
	if (method_end == std::string::npos) return;
	_method = request.substr(0, method_end);

	size_t url_end = request.find(' ', method_end + 1);
	if (url_end == std::string::npos) return;
	_url = request.substr(method_end + 1, url_end - method_end - 1);

	size_t version_end = request.find("\r\n", url_end + 1);
	if (version_end == std::string::npos) return;
	_http_version = request.substr(url_end + 1, version_end - url_end - 1);
}

Request::~Request() {}

Request::Request(const Request& other) {
    *this = other;
}

Request &Request::operator=(const Request& other) {
    if (this != &other) {
        // copy attributes here
    }
    return *this;
}

std::string Request::get_method() const {
	return _method;
}
std::string Request::get_url() const {
	return _url;
}

Reponse Request::handle_get() {
	// Handle GET request
	/**********************************************
	 * Ouvre le fichier demandé
	 * Si le fichier n'existe pas, retourne 404
	 * Si le fichier est un script CGI, l'exécute
	 * Sinon retourne le fichier avec 200
	 **********************************************/
	
	Reponse r = execute_cgi(_url);

	if (r.get_status_code() == -1) r = Reponse(_method, _url);
	return r;
}

Reponse Request::execute_cgi(std::string& url) {
	std::string path = "." + url;
	
	// Check si le fichier est executable
	if (access(path.c_str(), X_OK) != 0) {
		Reponse r;
		r.set_status_code(-1); // Pas un script cgi
		return r;
	}
	
	// Execute le script CGI
	int pipe_fd[2];
	pipe(pipe_fd);
	Reponse r;
	pid_t pid = fork();
	if (pid < 0) {
		r = Reponse::make_500();
		return r;
	} else if (pid == 0) {
		close(pipe_fd[0]);
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[1]);
		char *args[] = {const_cast<char*>(path.c_str()), NULL};
		extern char **environ;
		execve(path.c_str(), args, environ);
		exit(1);
	} else {		
		close(pipe_fd[1]);
		
		char buffer[4096];
        ssize_t bytes_read;
        std::ostringstream oss;
		
        while ((bytes_read = read(pipe_fd[0], buffer, sizeof(buffer))) > 0) {
			oss.write(buffer, bytes_read);
        }
		
		close(pipe_fd[0]);
		waitpid(pid, NULL, 0);
		
		std::string body = oss.str();
		
		std::string::size_type pos = body.find("\r\n\r\n");
		if (pos == std::string::npos)
			pos = body.find("\n\n");

		std::map<std::string, std::string> headers;
		std::string content;
		if (pos != std::string::npos) {
			std::string raw_headers = body.substr(0, pos);
			content = body.substr(pos + 2);

			std::istringstream hs(raw_headers);
			std::string line;
			while (std::getline(hs, line)) {
				std::string::size_type sep = line.find(':');
				if (sep != std::string::npos) {
					std::string key = line.substr(0, sep);
					std::string value = line.substr(sep + 1);
					headers[key] = value;
				}
			}
		} else {
			content = body; // pas d’en-têtes CGI, tout est du body
		}

		// --- construction de la réponse HTTP ---
		Reponse r;
		r.set_status_code(200);
		r.set_status_text("OK");
		r.set_body(content);

		if (headers.find("Content-Type") != headers.end())
			r.set_header("Content-Type", headers["Content-Type"]);
		else
			r.set_header("Content-Type", "text/plain"); // default, affiche a l'ecran

		std::ostringstream oss_len;
		oss_len << content.size();
		r.set_header("Content-Length", oss_len.str());
		
		return r;
	}		
}

Reponse Request::handle_post() {
	// Handle POST request
	Reponse r;
	std::cout << "Handling POST for " << _url << std::endl;
	return r;
}

Reponse Request::handle_delete() {
	// Handle DELETE request
	/*******************************************************
	 * Equivalent du rm fichier mais pour le client
	 * Interprete la request
		 Existence et droit de supprimer par le client
	 * Retourne 200 si ok
	 * Retourne 404 si le fichier n'existe pas
	 * Retourne 403 si pas le droit
	 * Retourne 500 si erreur serveur
	*******************************************************/
	
	Reponse r;
	
	std::cout << "Handling DELETE for " << _url << std::endl;
	return r;
}

