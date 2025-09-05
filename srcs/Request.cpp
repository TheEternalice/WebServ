/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 16:26:26 by lde-merc          #+#    #+#             */
/*   Updated: 2025/09/05 16:03:52 by lde-merc         ###   ########.fr       */
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

	size_t headers_end = request.find("\r\n\r\n");
    if (headers_end != std::string::npos) {
        _body = request.substr(headers_end + 4); // everything after headers

        // Optional: enforce Content-Length
        size_t cl_pos = request.find("Content-Length:");
        if (cl_pos != std::string::npos) {
            size_t cl_end = request.find("\r\n", cl_pos);
            std::string cl_str = request.substr(cl_pos + 15, cl_end - cl_pos - 15);
            int content_length = atoi(cl_str.c_str());
            if ((size_t)content_length < _body.size())
                _body = _body.substr(0, content_length);
        }
    } else {
        _body = "";
    }
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

std::string Request::get_body() const {
	return _body;
}

// Handle GET request
/**********************************************
 * Ouvre le fichier demandé
 * Si le fichier est un script CGI, l'exécute
 * Sinon retourne le fichier avec 200
 **********************************************/
Reponse Request::handle_get() {
	Reponse r = execute_cgi_get(_url);

	if (r.get_status_code() == -1) r = Reponse(_method, _url);
	return r;
}

Reponse Request::execute_cgi_get(std::string& url) {
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


// Handle POST request
/*******************************************************
 * Ajoute de la donnee au fichier demandé
 * Interprete la request
	Existence et droit d'ecriture par le client
 * Retourne 200 si ok
 * Retourne 403 si pas le droit
 * Retourne 500 si erreur serveur
********************************************************/
Reponse Request::handle_post() {
	std::string path = "." + _url;
	
	// Check si le fichier est ecrivable
	if (access(path.c_str(), W_OK) != 0) {
		Reponse r;
		r.set_status_code(403); // Pas le droit d'ecriture
		r.set_status_text("No Write Permission");
		r.set_body("403 No Write Permission");
		r.set_header("Content-Type", "text/plain");
		std::ostringstream oss_len;
		oss_len << r.get_body().size();
		r.set_header("Content-Length", oss_len.str());
		return r;
	}
	
	Reponse r;
	// std::cout << "Handling POST for " << _url << " body = " << _body << std::endl;
	r = execute_cgi_post(_url, _body); // On passe le body au CGI
	if (r.get_status_code() == -1) {
		std::ofstream out(path.c_str(), std::ios::binary);
		out << _body;
		r.set_status_code(200);
		r.set_status_text("OK");
		r.set_body("Data saved successfully\n");
		r.set_header("Content-Type", "text/plain");
		std::ostringstream oss;
		oss << r.get_body().size();
		r.set_header("Content-Length", oss.str());
	}
	
	return r;
}

// Execute une cgi, on passe le body en STDIN au script
Reponse Request::execute_cgi_post(std::string& url, std::string& body) {
	std::string path = "." + url;
	int pipe_in[2];  // Envoyer le body au script
	int pipe_out[2]; // Lire la reponse du script
	pipe(pipe_in);
	pipe(pipe_out);
	int flags = fcntl(pipe_out[0], F_GETFL, 0);
	fcntl(pipe_out[0], F_SETFL, flags | O_NONBLOCK);


	pid_t pid = fork();
	if (pid == 0){

		std::cout << "body = " << body << std::endl;
		std::cout << "body size = " << body.size() << " bytes" << std::endl;
		
		close(pipe_in[1]);
		dup2(pipe_in[0], STDIN_FILENO);
		close(pipe_in[0]);
		close(pipe_out[0]);
		dup2(pipe_out[1], STDOUT_FILENO);
		close(pipe_out[1]);
		
		char *args[] = {const_cast<char*>(path.c_str()), NULL};
		// On construit un petit environnement pour le script CGI puisse lire sur stdin
		std::vector<std::string> env;
		env.push_back("REQUEST_METHOD=POST");
		env.push_back("CONTENT_TYPE=application/x-www-form-urlencoded");

		std::ostringstream oss;
		oss << body.size();
		env.push_back("CONTENT_LENGTH=" + oss.str());

		char **envp = new char*[env.size() + 1];
		for (size_t i = 0; i < env.size(); i++)
			envp[i] = strdup(env[i].c_str());
		envp[env.size()] = NULL;

		execve(path.c_str(), args, envp);
		for (size_t i = 0; i < env.size(); i++) free(envp[i]);
		exit(1);
	} else {
		close(pipe_in[0]);
		write(pipe_in[1], body.c_str(), body.size());
		close(pipe_in[1]);
		
		close(pipe_out[1]);
		
		char buffer[4096];
		int bytes_read;
		std::ostringstream oss;
		while((bytes_read = read(pipe_out[0], buffer, sizeof(buffer))) > 0) {
			oss.write(buffer, bytes_read);
		}
		std::cerr << "CGI OUTPUT:\n" << oss.str() << std::endl;
		waitpid(pid, NULL, 0);
		close(pipe_out[0]);
		
		Reponse r;
		r.set_status_code(200);
		r.set_status_text("OK");
		r.set_body(oss.str());
		r.set_header("Content-Type", "text/plain");
		std::ostringstream oss_len;
		oss_len << r.get_body().size();
		r.set_header("Content-Length", oss_len.str());
		return r;
	}
}

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
Reponse Request::handle_delete() {
	
	
	Reponse r;
	
	std::cout << "Handling DELETE for " << _url << std::endl;
	return r;
}
