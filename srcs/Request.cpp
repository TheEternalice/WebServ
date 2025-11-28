/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-rese <ade-rese@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 16:26:26 by lde-merc          #+#    #+#             */
/*   Updated: 2025/11/28 14:19:47 by ade-rese         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Request.hpp"
#include "../includes/Server.hpp"
#include <cstring>
extern char **environ;

Request::Request() {}

Request::Request(const std::string &request, size_t bytes_read) {
	parse(request, bytes_read);
}

Request::~Request() {}

Request::Request(const Request& other) {
	*this = other;
}

Request &Request::operator=(const Request& other) {
	if (this != &other) {
		this->_body = other._body;
		this->_headers = other._headers;
		this->_url = other._url;
		this->_method = other._method;
		this->_http_version = other._http_version;
		this->_request_cookies = other._request_cookies;
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

void Request::parse(const std::string &buffer, size_t bytes_read) {
	(void)bytes_read;
	size_t	pos = buffer.find("\r\n\r\n");
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
	pos = _url.find('?');
	if (pos != std::string::npos)
		_url = _url.substr(0, pos);

	while (std::getline(stream, line)) {
		if (line[line.size() - 1] == '\r') line.resize(line.size() - 1);
		if (line.empty()) break;
		size_t colon = line.find(":");
		if (colon == std::string::npos) continue;
		std::string key = line.substr(0, colon);
		std::string value = line.substr(colon + 1);
		_headers[key] = value;
	}
	_headers["Connection"] = "keep-alive";
	parseCookies();
	if (hasHeader("Content-Length")) {
		int len = to_int(getHeader("Content-Length"));
		_body = body.substr(0, len);
	}
	else if (hasHeader("Transfer-Encoding") && getHeader("Transfer-Encoding") == "chunked") {
		_body = parseChunked(body);
	}else {
		_body = body;
	}
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
 * Open the asked file
 * if the file is a CGI script and execute it
 * Else if return the file with code 200
 **********************************************/
Reponse Request::handle_get(std::string root, std::string index, std::string locationPath, bool autoIndex, bool *testing) {
	try {
		Reponse r = execute_cgi_get(_url, testing);
		if (r.get_body() == "")
			r.set_status_code(500);
		return r;
	} catch (std::exception &e) {
		std::string error = static_cast<std::string>(e.what());
		if (error == "file" || error == "empty") {
			Reponse r;
			r.set_status_code(500);
			return r;
		}
		Reponse r = Reponse(_url, root, index, locationPath, autoIndex, testing);
		return r;
	}
}

long now_ms() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

Reponse Request::execute_cgi_get(std::string& url, bool *testing) {
	if (*testing)
		throw std::runtime_error("html");
	std::string path = "." + url;
	struct stat st;
	if (stat(path.c_str(), &st) != 0)
		throw std::runtime_error("file");
		
	if (st.st_size == 0)
		throw std::runtime_error("empty");
		
	if (access(("." + _url).c_str(), X_OK) != 0 || url == "/")
		throw std::runtime_error("html");

	_waitingCgi = true;
	_lastActivity = std::time(NULL);

	// Execute the CGI script
	int pipe_fd[2];
	pipe(pipe_fd);
	pid_t pid = fork();
	Reponse r;
	if (pid < 0) {
		_waitingCgi = false;
		r.set_status_code(500);
		return r;
	} else if (pid == 0) {
		close(pipe_fd[0]);
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[1]);

		std::vector<std::string> env_strings;
		env_strings.push_back("REQUEST_METHOD=GET");
		env_strings.push_back("PATH=/usr/bin:/bin");

		// take the header cookie and change it into a vector in the server
		std::string cookies_header = getHeader("Cookie");
		if (!cookies_header.empty()) {
			env_strings.push_back("HTTP_COOKIE=" + cookies_header);
		}

		std::vector<char*> envp;
		envp.reserve(env_strings.size() + 1);

		for (size_t i = 0; i < env_strings.size(); i++)
			envp.push_back(const_cast<char*>(env_strings[i].c_str()));

		envp.push_back(NULL);

		char *args[] = {const_cast<char*>(path.c_str()), NULL};
		execve(path.c_str(), args, envp.data());
		std::exit(1);

	} else {
		close(pipe_fd[1]);

		fcntl(pipe_fd[0], F_SETFL, O_NONBLOCK);

		char buffer[4096];
		std::ostringstream oss;

		const int TIMEOUT_MS = 5000;
		long start = now_ms();
		int status;
		bool finished = false;


		while(true) {
			ssize_t bytes_read = read(pipe_fd[0], buffer, sizeof(buffer));
			if (bytes_read > 0) {
				oss.write(buffer, bytes_read);
				_lastActivity = std::time(NULL);
			}

			pid_t result = waitpid(pid, &status, WNOHANG);
			if (result == pid) {
				finished = true;
				break;
			}
			if (finished)
			{
				continue;
			}
			long elapsed = now_ms() - start;
			if (elapsed > TIMEOUT_MS) {
				kill(pid, SIGKILL);
				waitpid(pid, NULL, 0);
				close(pipe_fd[0]);
				r.set_status_code(504);
				return r;
			}

			usleep(1);
		}

		close(pipe_fd[0]);

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
			content = body; // no CGI header, everything in the body
		}


		// --- construction of the HTTP reponse ---
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


// Handle POST request
/*******************************************************
 * Add some data to the asked file
 * Interpret the request
 * Existence and permission to read per client
 * Return 200 if ok
 * Return 403 if no permissions
 * Return 500 if server error
********************************************************/
Reponse Request::handle_post(std::string uploadDir) {

	// Check if the file is writable
	if (access(uploadDir.c_str(), W_OK) != 0) {
		Reponse r;
		r.set_status_code(403); // No permission to write
		return r;
	}

	Reponse r;
	r = execute_cgi_post(_url, _body); // give the body to the CGI
	if (r.get_status_code() == -1) {
		std::string filePath = uploadDir + "/test.txt";
		if (access(filePath.c_str(), W_OK) != 0) {
			Reponse r;
			r.set_status_code(403); // No permission to write
			return r;
		}
		std::ofstream out(filePath.c_str(), std::ios::binary | std::ios::app);
		if (out.is_open()) {
			std::string body = _body;
			std::string message;
			
			// search message in the body
			size_t msgPos = body.find("message=");
			if (msgPos != std::string::npos) {
				message = body.substr(msgPos + 8);
				// replace + by space
				size_t plusPos;
				while ((plusPos = message.find("+")) != std::string::npos) {
					message.replace(plusPos, 1, " ");
				}
				// search % for get hexadecimal values (ascii table)  
				size_t percentPos;
				while ((percentPos = message.find("%")) != std::string::npos && 
					percentPos + 2 < message.length()) {
					std::string hex = message.substr(percentPos + 1, 2);
					std::istringstream iss(hex);
					unsigned int value;
					if (iss >> std::hex >> value) {
						char decoded = static_cast<char>(value);
						message.replace(percentPos, 3, 1, decoded);
					} else {
						percentPos += 3;
					}
				}
			} else {
				message = body;
			}

			out << message << std::endl;
			out.close();

			r.set_status_code(200);
			r.set_status_text("OK");
			r.set_body("Data saved successfully\n");
			r.set_header("Content-Type", "text/plain");
			std::ostringstream oss;
			oss << r.get_body().size();
			r.set_header("Content-Length", oss.str());
		} else {
			r.set_status_code(500);
			r.set_status_text("Internal Server Error");
			r.set_body("Failed to create file\n");
		}
	}

	return r;
}

// Execute a CGI, and give the body by STDIN to the script
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
		// Child : redirect stdin, stdout, stderr
		close(pipe_in[1]);
		dup2(pipe_in[0], STDIN_FILENO);
		close(pipe_in[0]);
		close(pipe_out[0]);
		dup2(pipe_out[1], STDOUT_FILENO);
		dup2(pipe_out[1], STDERR_FILENO);
		close(pipe_out[1]);

		// Prepare the environnement
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
		std::string cookies_header = getHeader("Cookie");
		if (!cookies_header.empty()) {
			cgi_vars.push_back("HTTP_COOKIE=" + cookies_header);
		}
		std::vector<char*> envp;
		envp.reserve(cgi_vars.size() + 1);

		for (size_t i = 0; i < cgi_vars.size(); i++)
			envp.push_back(const_cast<char*>(cgi_vars[i].c_str()));

		envp.push_back(NULL);
		char *args[] = {const_cast<char*>(path.c_str()), NULL};
		execve(path.c_str(), args, envp.data());
		std::exit(1);
	} else {
		// Parent : write the body, read the output
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
 * Equivalent of rm file but for the client
 * Interpret the request
	 Existence and permission of delete by
		the client with unlink()
	* Return 200 if ok
	* Return 403 if it's a directory
	* Return 500 if server error
*******************************************************/
Reponse Request::handle_delete() {
	std::string path = "." + _url;

	// Check if the file is writable
	if (access(path.c_str(), W_OK) != 0) {
		Reponse r;
		r.set_status_code(403);
		return r;
	}

	// Delete the file
	if (unlink(path.c_str()) != 0) {
		Reponse r;
		r.set_status_code(500);
		return r;
	}

	Reponse r;
	r.set_status_code(200);
	r.set_status_text("OK");
	r.set_body("<p style='color:red;'>File deleted successfully</p>");
	r.set_header("Content-Type", "text/plain");
	std::ostringstream oss;
	oss << r.get_body().size();
	r.set_header("Content-Length", oss.str());
	return r;
}
