/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 16:26:26 by lde-merc          #+#    #+#             */
/*   Updated: 2025/09/30 13:19:12 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Request.hpp"
#include "../includes/Server.hpp"
#include <cstring>
extern char **environ;

// Constructeur
Request::Request() {}

Request::Request(const std::string &request) { parse(request);}
	// Simple parsing, assumes well-formed request
	// size_t method_end = request.find(' ');
	// if (method_end == std::string::npos) return;
	// _method = request.substr(0, method_end);

	// size_t url_end = request.find(' ', method_end + 1);
	// if (url_end == std::string::npos) return;
	// _url = request.substr(method_end + 1, url_end - method_end - 1);

	// size_t version_end = request.find("\r\n", url_end + 1);
	// if (version_end == std::string::npos) return;
	// _http_version = request.substr(url_end + 1, version_end - url_end - 1);

	// size_t headers_end = request.find("\r\n\r\n");
    // if (headers_end != std::string::npos) {
    //     _body = request.substr(headers_end + 4); // everything after headers

    //     // Optional: enforce Content-Length
    //     size_t cl_pos = request.find("Content-Length:");
    //     if (cl_pos != std::string::npos) {
    //         size_t cl_end = request.find("\r\n", cl_pos);
    //         std::string cl_str = request.substr(cl_pos + 15, cl_end - cl_pos - 15);
    //         int content_length = atoi(cl_str.c_str());
    //         if ((size_t)content_length < _body.size())
    //             _body = _body.substr(0, content_length);
    //     }
    // } else {
    //     _body = "";
    // }
// }

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

std::map<std::string, std::string> Request::getCookies() const { 
	return _request_cookies;
}

void Request::parse(const std::string &buffer) {
	size_t	pos = buffer.rfind("\r\n\r\n");
	if (pos == std::string::npos) {
		throw (std::runtime_error("Invalid HTTP request"));
	}
	std::string header = buffer.substr(0, pos);
	std::string body = buffer.substr(pos + 4);
	std::istringstream stream(header);
	std::string line;
	if (!std::getline(stream, line))
		throw (std::runtime_error("Empty HTTP request"));
	if (line[line.size() - 1] == '\r') line.resize(line.size() - 1);
	std::istringstream first_line(line);
	first_line >> _method >> _url >> _http_version;
	while (std::getline(stream, line)) {
		if (line[line.size() - 1] == '\r') line.resize(line.size() - 1);
		if (line.empty()) break;
		size_t colon = line.find(":");
		if (colon == std::string::npos) continue;
		std::string key = line.substr(0, colon);
		std::string value = line.substr(colon + 1);
		_headers[key] = value;
	}
	parseCookies();
	if (hasHeader("Content-Length")) {
		int len = to_int(getHeader("Content-Length"));
		_body = body.substr(0, len);
	}
	else if (hasHeader("Transfer-Encoding") && getHeader("Transfer-Encoding") == "chunked") {_body = parseChunked(body);}
	else {_body = body;}
}

bool Request::hasHeader(const std::string &buffer) const {return (_headers.find(buffer) != _headers.end());}

std::string Request::getCookie(const std::string& name) const {
	std::map<std::string, std::string>::const_iterator it = _request_cookies.find(name);
	if (it != _request_cookies.end()) {
	    return it->second;
	}
	return ("");
}

std::string Request::getHeader(const std::string &buffer) const {
	std::map<std::string, std::string>::const_iterator it = _headers.find(buffer);
	if (it != _headers.end())
		return (it->second);
	return ("");
}

std::string Request::parseChunked(const std::string &buffer) {
	std::string body;
	size_t pos = 0;
	
	while (true) {
		size_t endl = buffer.find("\r\n", pos);
		if (endl == std::string::npos) break;
		std::string str_sub = buffer.substr(pos, endl - pos);
		int chunk_size = to_int(str_sub);
		if (chunk_size == 0) break;
		pos = endl + 2;
		body += buffer.substr(pos, chunk_size);
		pos += chunk_size + 2;
	}
	return (body);
}

void Request::parseCookies() {
	std::string cookies_header = getHeader("Cookie");
	if (cookies_header.empty())
		return ;
	
	std::vector<std::string> cookie_pair = cpp_split(cookies_header, ';');
	for (size_t i = 0; i < cookie_pair.size(); ++i) {
		std::string pair = trim(cookie_pair[i]);
		size_t pos_equal = pair.find('=');
		
		if (pos_equal != std::string::npos) {
			std::string name = trim(pair.substr(0, pos_equal));
			std::string value = trim(pair.substr(pos_equal + 1));
			_request_cookies[name] = value;
		}
	}
}

// Handle GET request
/**********************************************
 * Ouvre le fichier demandé
 * Si le fichier est un script CGI, l'exécute
 * Sinon retourne le fichier avec 200
 **********************************************/
Reponse Request::handle_get() {
	try {
		Reponse r = execute_cgi_get(_url);
		return r;
	} catch (...) { }

	Reponse r = Reponse(_method, _url);
	return r;	
}

Reponse Request::execute_cgi_get(std::string& url) {
	std::string path = "." + url;
	
	// Check si le fichier est executable
	if (access(path.c_str(), X_OK) != 0) {
		throw std::runtime_error("");
	}
	
	// Execute le script CGI
	int pipe_fd[2];
	pipe(pipe_fd);
	pid_t pid = fork();
	Reponse r;
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
	if (access(path.c_str(), X_OK) != 0) {
		Reponse r;
		r.set_status_code(-1);
		return r;
	}

	int pipe_in[2];
	int pipe_out[2];
	if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1) {
		Reponse r;
		r.set_status_code(500);
		r.set_status_text("Pipe error");
		return r;
	}

	pid_t pid = fork();
	if (pid < 0) {
		Reponse r;
		r.set_status_code(500);
		r.set_status_text("Fork error");
		return r;
	}
	if (pid == 0) {
		// Fils : redirige stdin, stdout, stderr
		close(pipe_in[1]);
		dup2(pipe_in[0], STDIN_FILENO);
		close(pipe_in[0]);
		close(pipe_out[0]);
		dup2(pipe_out[1], STDOUT_FILENO);
		dup2(pipe_out[1], STDERR_FILENO);
		close(pipe_out[1]);

		// Prépare l'environnement
		std::vector<std::string> env_vec;
		for (char **env = environ; *env != 0; env++) {
			env_vec.push_back(std::string(*env));
		}
		std::ostringstream oss;
		oss << body.size();
		std::vector<std::string> cgi_vars;
		cgi_vars.push_back("REQUEST_METHOD=POST");
		cgi_vars.push_back("CONTENT_TYPE=application/x-www-form-urlencoded");
		cgi_vars.push_back("CONTENT_LENGTH=" + oss.str());
		cgi_vars.push_back("PATH=/usr/bin:/bin");
		for (size_t i = 0; i < cgi_vars.size(); ++i) {
			std::string key = cgi_vars[i].substr(0, cgi_vars[i].find('='));
			bool found = false;
			for (size_t j = 0; j < env_vec.size(); ++j) {
				if (env_vec[j].find(key + "=") == 0) {
					env_vec[j] = cgi_vars[i];
					found = true;
					break;
				}
			}
			if (!found)
				env_vec.push_back(cgi_vars[i]);
		}
		char **envp = new char*[env_vec.size() + 1];
		for (size_t i = 0; i < env_vec.size(); i++)
			envp[i] = strdup(env_vec[i].c_str());
		envp[env_vec.size()] = NULL;

		char *args[] = {const_cast<char*>(path.c_str()), NULL};
		execve(path.c_str(), args, envp);
		perror("execve failed");
		for (size_t i = 0; i < env_vec.size(); i++) free(envp[i]);
		delete[] envp;
		exit(1);
	} else {
		// Parent : écrit le body, lit la sortie
		close(pipe_in[0]);
		ssize_t written = 0;
		while (written < (ssize_t)body.size()) {
			ssize_t w = write(pipe_in[1], body.c_str() + written, body.size() - written);
			if (w <= 0) break;
			written += w;
		}
		close(pipe_in[1]);
		close(pipe_out[1]);

		char buffer[4096];
		std::ostringstream oss;
		ssize_t bytes_read;
		while ((bytes_read = read(pipe_out[0], buffer, sizeof(buffer))) > 0) {
			oss.write(buffer, bytes_read);
		}
		close(pipe_out[0]);
		waitpid(pid, NULL, 0);

		std::string cgi_output = oss.str();
		std::string::size_type pos = cgi_output.find("\r\n\r\n");
		if (pos == std::string::npos)
			pos = cgi_output.find("\n\n");

		std::map<std::string, std::string> headers;
		std::string content;
		if (pos != std::string::npos) {
			std::string raw_headers = cgi_output.substr(0, pos);
			content = cgi_output.substr(pos + 2);
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
			content = cgi_output;
		}

		Reponse r;
		r.set_status_code(200);
		r.set_status_text("OK");
		r.set_body(content);
		if (headers.find("Content-Type") != headers.end())
			r.set_header("Content-Type", headers["Content-Type"]);
		else
			r.set_header("Content-Type", "text/plain");
		std::ostringstream oss_len;
		oss_len << content.size();
		r.set_header("Content-Length", oss_len.str());
		return r;
	}
}


// Handle DELETE request
/*******************************************************
 * Equivalent du rm fichier mais pour le client
 * Interprete la request
	 Existence et droit de supprimer par le client avec unlink()
	* Retourne 200 si ok
	* Retourne 403 si c'est un repertoire
	* Retourne 500 si erreur serveur
*******************************************************/
Reponse Request::handle_delete() {
	std::cout << "Handling DELETE for " << _url << std::endl;
	
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
	
	// Supprime le fichier
	if (unlink(path.c_str()) != 0) {
		Reponse r;
		r.set_status_code(500);
		r.set_status_text("Internal Server Error");
		r.set_body("500 Internal Server Error");
		r.set_header("Content-Type", "text/plain");
		std::ostringstream oss_len;
		oss_len << r.get_body().size();
		r.set_header("Content-Length", oss_len.str());
		return r;
	}

	Reponse r;
	r.set_status_code(200);
	r.set_status_text("OK");
	r.set_body("File deleted successfully\n");
	r.set_header("Content-Type", "text/plain");
	std::ostringstream oss;
	oss << r.get_body().size();
	r.set_header("Content-Length", oss.str());
	return r;
}
