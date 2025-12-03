/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reponse.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpichon <gpichon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 12:32:35 by lde-merc          #+#    #+#             */
/*   Updated: 2025/11/28 16:19:00 by gpichon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Reponse.hpp"
#include "../includes/Server.hpp"

Reponse::Reponse() {
	this->_body = "";
	this-> _headers[""] = "";
	this->_status_code = 0;
	this->_status_text = "";
	this->_up = false;
}

Reponse::Reponse(std::string url, std::string root, std::string index, std::string locationPath, bool autoIndex, bool *testing) {
	this->_body = "";
	this->_status_code = 0;
	this->_status_text = "";
	this->_up = false;

	std::string path;
	std::string urlpath = url;

	if (locationPath != "/" && url.size() >= locationPath.size()) {
		if (url.compare(0, locationPath.size(), locationPath) == 0) {
			url = url.substr(locationPath.size());
			if (url.empty() || url[0] != '/')
				url = "/" + url;
		}
	}
	if (!root.empty() && url.size() && url.find('/' + root) == 0) {
		url = url.substr(root.size() + 1);
		if (url.empty() || url[0] != '/')
			url = "/" + url;
	}
	if (urlpath == "/") {
		if (root.empty())
			root = ".";
		if (!index.empty()){
			std::vector<std::string> indexFiles = cpp_split(index, ' ');
			for (size_t i = 0; i < indexFiles.size(); i++) {
				path = root + "/" + indexFiles[i];
				if (access(path.c_str(), F_OK) == 0)
					break;
				path.clear();
			}
		}
		if (path.empty())
			path = root;
	} else {
		if (root.empty())
			path = "." + urlpath;
		else if (autoIndex || *testing)
		{
			path = root + urlpath;
			if (*testing == true)
				*testing = false;
		}
		else
			path = root + urlpath;
	}
	struct stat path_stat;
	if (stat(path.c_str(), &path_stat) != 0) {
		throw std::runtime_error("Can't open file");
	}
	// check on the directory if we have a imdex
	if (S_ISDIR(path_stat.st_mode) && !_up) {
		bool indexFound = false;
		if (!index.empty()) {
			std::vector<std::string> indexFiles = cpp_split(index, ' ');
			for (size_t i = 0; i < indexFiles.size(); i++) {
				std::string indexPath = path + "/" + indexFiles[i];
				if ((access(indexPath.c_str(), F_OK) == 0)) {
					path = indexPath;
					indexFound = true;
					break;
				}
			}
		}

		if (indexFound) {
			// show the index
			struct stat file_stat;
			if (stat(path.c_str(), &file_stat) != 0) {
				throw std::runtime_error("");
			}
			std::ifstream file(path.c_str(), std::ios::binary);
			if (!file || access(path.c_str(), F_OK) != 0) {
				throw std::runtime_error("");
			}
			std::ostringstream oss;
			oss << file.rdbuf();
			_body = oss.str();
			_status_code = 200;
			_status_text = "OK";
			_headers["Content-Type"] = Server::get_content_type(path);
		} else {
			if (autoIndex) {
				std::string displayUrl = url;
				if (displayUrl.empty())
					displayUrl = "/";
				if (displayUrl[displayUrl.size() - 1] != '/')
					displayUrl += "/";
				_body = generateDirectoryListing(path, displayUrl);
				_status_code = 200;
				_status_text = "OK";
				_headers["Content-Type"] = "text/html";
			} else {
				throw std::runtime_error("403");
			}
		}
	} else {
		std::ifstream file(path.c_str(), std::ios::binary);
		if (!file || access(path.c_str(), F_OK) != 0) {
			throw std::runtime_error("");
		}
		std::ostringstream oss;
		oss << file.rdbuf();
		_body = oss.str();
		_status_code = 200;
		_status_text = "OK";
		_headers["Content-Type"] = Server::get_content_type(path);
	}

	std::ostringstream oss_len;
	oss_len << _body.size();
	_headers["Content-Length"] = oss_len.str();
}

Reponse::Reponse(int num, std::string path) {
	_status_code = num;

	switch (_status_code) {
		case 400:
			_status_text = "Bad request";
			break;
		case 403:
			_status_text = "Forbidden";
			break;
		case 404:
			_status_text = "Not Found";
			_headers["Connection"] = "close";
			break;
		case 405:
			_status_text = "Method Not Allowed";
			break;
		case 500:
			_status_text = "Internal Server Error";
			break;
		case 503:
			_status_text = "Service Unavailable";
			break;
		default:
			break;
	}

	_body = "";
	std::ifstream bodyData(path.c_str());
	if (!bodyData.is_open()) {throw std::runtime_error("Can't open bodyData file");}

	char c = 0;
	while (bodyData.get(c)){ std::string s; s.push_back(c); _body += s; }

	_headers["Content-Type"] = Server::get_content_type(path);
	std::ostringstream oss;
	if (_body.size() != 0){
		oss << _body.size();
		_headers["Content-Length"] = oss.str();
	}
	else
		_headers["Content-Length"] = "";
}

Reponse::~Reponse() {}

Reponse::Reponse(const Reponse &other) {
	*this = other;
}

Reponse &Reponse::operator=(const Reponse &other) {
	if (this != &other) {
		this->_status_code = other._status_code;
		this->_status_text = other._status_text;
		this->_headers = other._headers;
		this->_body = other._body;
	}
	return *this;
}

std::map<std::string, std::string> Reponse::get_header() const {
	return _headers;
}

std::string Reponse::to_string() const {
	std::ostringstream oss;
	oss << "HTTP/1.1 " << _status_code << " " << _status_text << "\r\n";
	std::map<std::string, std::string>::const_iterator it;
	for (it = _headers.begin(); it != _headers.end(); ++it) {
		oss << it->first << ": " << it->second << "\r\n";
	}
	oss << "\r\n" << _body;
	return oss.str();
}

std::string Reponse::generateDirectoryListing(const std::string& dirPath, const std::string& url) {
	std::ostringstream html;
	html << "<!DOCTYPE html>\n<html>\n<head>\n";
	html << "<title>Index of " << url << "</title>\n";
	html << "<style>body { font-family: monospace; margin: 40px; }";
	html << "h1 { color: #333; }";
	html << "table { border-collapse: collapse; width: 100%; }";
	html << "th, td { text-align: left; padding: 5px; }";
	html << "th { background-color:rgb(0, 0, 0); color: white; }";
	html << "tr:nth-child(even) { background-color: #f2f2f2; }";
	html << "a { text-decoration: none; color: #0066cc; }";
	html << "a:hover { text-decoration: underline; }</style>\n";
	html << "</head>\n<body>\n";
	html << "<h1>Index of " << url << "</h1>\n";
	html << "<hr>\n<table>\n";
	html << "<tr><th>Name</th><th>Size</th></tr>\n";

	if (url != "/") {
		std::string parentUrl = url;
		if (parentUrl[parentUrl.size() - 1] == '/')
			parentUrl = parentUrl.substr(0, parentUrl.size() - 1);
		size_t lastSlash = parentUrl.find_last_of('/');
		if (lastSlash != std::string::npos)
			parentUrl = parentUrl.substr(0, lastSlash);
		if (parentUrl.empty())
			parentUrl = "/";
		html << "<tr><td><a href=\"" << parentUrl << "\">../</a></td><td>-</td><td>-</td></tr>\n";
	}

	DIR* dir = opendir(dirPath.c_str());
	if (dir != NULL) {
		// DIRENT is mendatory to use READDIR which is allowed by the subject it return a DIRENT*)
		struct dirent* entry;
		while ((entry = readdir(dir)) != NULL) {
			if (entry->d_name[0] == '.')
				continue;

			std::string entryPath = dirPath + "/" + entry->d_name;
			// STAT is mendatory to use S_ISDIR and it's allowed by the subject
			struct stat entryStat;
			if (stat(entryPath.c_str(), &entryStat) != 0)
				continue;

			std::string entryUrl = url;
			if (!entryUrl.empty() && entryUrl[entryUrl.size() - 1] != '/')
				entryUrl += "/";
			entryUrl += entry->d_name;

			std::string displayName = entry->d_name;
			if (S_ISDIR(entryStat.st_mode))
				displayName += "/";

			std::string sizeStr;
			if (S_ISDIR(entryStat.st_mode))
				sizeStr = "-";
			else {
				std::ostringstream sizeOss;
				sizeOss << entryStat.st_size;
				sizeStr = sizeOss.str();
			}

			html << "<tr><td><a href=\"" << entryUrl << "\">" << displayName << "</a></td>";
			html << "<td>" << sizeStr << "</td></tr>\n";
		}
		closedir(dir);
	}

	html << "</table>\n<hr>\n</body>\n</html>\n";
	return html.str();
}

Reponse Reponse::make_500() {
	Reponse r;
	r._status_code = 500;
	r._body = "500 Internal Server Error";
	r._headers["Content-Type"] = "text/plain";
	r._headers["Content-Length"] = r.to_string();
	return r;
}

Reponse Reponse::createDefaultErrorResponse(int errorCode) {
	Reponse r;
	r._status_code = errorCode;
	
	if (errorCode >= 400 && errorCode < 500) {
		switch (errorCode) {
			case 400: r._status_text = "Bad Request"; break;
			case 401: r._status_text = "Unauthorized"; break;
			case 402: r._status_text = "Payment Required"; break;
			case 403: r._status_text = "Forbidden"; break;
			case 404: r._status_text = "Not Found"; r._headers["Connection"] = "close"; break;
			case 405: r._status_text = "Method Not Allowed"; break;
			case 406: r._status_text = "Not Acceptable"; break;
			case 407: r._status_text = "Proxy Authentication Required"; break;
			case 408: r._status_text = "Request Timeout"; break;
			case 409: r._status_text = "Conflict"; break;
			case 410: r._status_text = "Gone"; break;
			case 411: r._status_text = "Length Required"; break;
			case 412: r._status_text = "Precondition Failed"; break;
			case 413: r._status_text = "Payload Too Large"; break;
			case 414: r._status_text = "URI Too Long"; break;
			case 415: r._status_text = "Unsupported Media Type"; break;
			case 416: r._status_text = "Range Not Satisfiable"; break;
			case 417: r._status_text = "Expectation Failed"; break;
			case 418: r._status_text = "I'm a teapot"; break;
			case 421: r._status_text = "Misdirected Request"; break;
			case 422: r._status_text = "Unprocessable Entity"; break;
			case 423: r._status_text = "Locked"; break;
			case 424: r._status_text = "Failed Dependency"; break;
			case 425: r._status_text = "Too Early"; break;
			case 426: r._status_text = "Upgrade Required"; break;
			case 428: r._status_text = "Precondition Required"; break;
			case 429: r._status_text = "Too Many Requests"; break;
			case 431: r._status_text = "Request Header Fields Too Large"; break;
			case 451: r._status_text = "Unavailable For Legal Reasons"; break;
			default: r._status_text = "Client Error"; break;
		}
	} else if (errorCode >= 500 && errorCode < 600) {
		switch (errorCode) {
			case 500: r._status_text = "Internal Server Error"; break;
			case 501: r._status_text = "Not Implemented"; break;
			case 502: r._status_text = "Bad Gateway"; break;
			case 503: r._status_text = "Service Unavailable"; break;
			case 504: r._status_text = "Gateway Timeout"; break;
			case 505: r._status_text = "HTTP Version Not Supported"; break;
			case 506: r._status_text = "Variant Also Negotiates"; break;
			case 507: r._status_text = "Insufficient Storage"; break;
			case 508: r._status_text = "Loop Detected"; break;
			case 510: r._status_text = "Not Extended"; break;
			case 511: r._status_text = "Network Authentication Required"; break;
			default: r._status_text = "Server Error"; break;
		}
	} else {
		r._status_text = "Error";
	}
	
	std::ostringstream html;
	html << "<!DOCTYPE html>\n";
	html << "<html lang=\"fr\">\n";
	html << "<head>\n";
	html << "    <meta charset=\"UTF-8\">\n";
	html << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
	html << "    <title>" << errorCode << " - " << r._status_text << "</title>\n";
	html << "    <style>\n";
	html << "        body {\n";
	html << "            font-family: Arial, sans-serif;\n";
	html << "            background-color: #f8f9fa;\n";
	html << "            margin: 0;\n";
	html << "            padding: 0;\n";
	html << "            display: flex;\n";
	html << "            justify-content: center;\n";
	html << "            align-items: center;\n";
	html << "            min-height: 100vh;\n";
	html << "        }\n";
	html << "        .error-container {\n";
	html << "            text-align: center;\n";
	html << "            background-color: white;\n";
	html << "            padding: 40px;\n";
	html << "            border-radius: 10px;\n";
	html << "            box-shadow: 0 4px 20px rgba(0,0,0,0.1);\n";
	html << "            max-width: 500px;\n";
	html << "        }\n";
	html << "        .error-code {\n";
	html << "            font-size: 120px;\n";
	html << "            font-weight: bold;\n";
	html << "            color: #dc3545;\n";
	html << "            margin: 0;\n";
	html << "            line-height: 1;\n";
	html << "        }\n";
	html << "        .error-message {\n";
	html << "            font-size: 24px;\n";
	html << "            color: #6c757d;\n";
	html << "            margin: 20px 0;\n";
	html << "        }\n";
	html << "        .error-description {\n";
	html << "            color: #6c757d;\n";
	html << "            margin-bottom: 30px;\n";
	html << "        }\n";
	html << "    </style>\n";
	html << "</head>\n";
	html << "<body>\n";
	html << "    <div class=\"error-container\">\n";
	html << "        <h1 class=\"error-code\">" << errorCode << "</h1>\n";
	html << "        <h2 class=\"error-message\">" << r._status_text << "</h2>\n";
	html << "        <p class=\"error-description\">\n";
	
	if (errorCode >= 400 && errorCode < 500) {
		html << "            Une erreur côté client s'est produite.\n";
		html << "            Veuillez vérifier votre requête et réessayer.\n";
	} else if (errorCode >= 500 && errorCode < 600) {
		html << "            Une erreur côté serveur s'est produite.\n";
		html << "            Veuillez réessayer plus tard.\n";
	} else {
		html << "            Une erreur s'est produite.\n";
	}
	
	html << "        </p>\n";
	html << "    </div>\n";
	html << "</body>\n";
	html << "</html>\n";
	
	r._body = html.str();
	r._headers["Content-Type"] = "text/html";
	std::ostringstream oss;
	oss << r._body.size();
	r._headers["Content-Length"] = oss.str();
	
	return r;
}

int Reponse::get_status_code() const {
	return _status_code;
}


std::string Reponse::get_body() const {
	return _body;
}

void Reponse::set_status_code(int code) {
	_status_code = code;
}

void Reponse::set_status_text(const std::string& text) {
	_status_text = text;
}

void Reponse::set_body(const std::string& body) {
	_body = body;
}

void Reponse::set_up() {
	_up = true;
}

bool Reponse::get_up() {
	return _up;
}

void Reponse::set_header(const std::string& key, const std::string& value) {
	_headers[key] = value;
}

static std::string toLower(const std::string &s) {
	std::string out = s;
	for (size_t i = 0; i < out.size(); ++i) out[i] = static_cast<char>(std::tolower(out[i]));
	return out;
}

bool Reponse::isKeepAlive() const {
	// Search header "Connection" by ignoring failures
	std::map<std::string, std::string>::const_iterator it;
	for (it = _headers.begin(); it != _headers.end(); ++it) {
		if (toLower(it->first) == "connection") {
			std::string val = toLower(it->second);
			if (val.find("close") != std::string::npos) return false;
			if (val.find("keep-alive") != std::string::npos) return true;
		}
	}
	return true;
}
