/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 15:47:12 by lde-merc          #+#    #+#             */
/*   Updated: 2025/11/27 13:55:26 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
volatile bool g_running(true);
Server* g_server = NULL;

std::map<std::string, std::string> Server::_extensionsToType;

void handle_sigint(int signum) {
	(void)signum;
	std::cout << "\nSignal reçu, arrêt du serveur..." << std::endl;
	g_running = false;
	g_server->cleanup();
}

void Server::cleanup() {
	std::cout << "Nettoyage des connexions..." << std::endl;

	for (size_t i = 0; i < _fds.size(); ++i) {
		int fd = _fds[i].fd;
		// if it's not a server socket
		if (!isServerSocket(fd)) {
			std::map<int, Client*>::iterator it = _socketToClient.find(fd);
			if (it != _socketToClient.end()) {
				if (it->second) {
					delete it->second; // free the client
					it->second = NULL;
				}
				close(fd);
			}
		} else {
			close(fd);
		}
	}
	_socketToClient.clear();
	_fds.clear();
	std::cout << "Serveur arrêté proprement." << std::endl;
}

Server::Server() {
	std::ifstream file("mime.types");
	if (!file.is_open())
		throw std::runtime_error("Could not open mime.types");

	std::string line;
	while (std::getline(file, line)) {
		// Ignore comments and empty lines
		if (line.empty() || line[0] == '#')
			continue;

		std::istringstream iss(line);
		std::string mime_type;
		if (!(iss >> mime_type))
			continue;

		std::string ext;
		while (iss >> ext) {
			if (ext[0] != '.')
				ext = "." + ext; // normalize with a dot
			_extensionsToType[ext] = mime_type;
		}
	}
	file.close();
}

Server::~Server() {}

Server::Server(const Server &other) { *this = other; }

Server &Server::operator=(const Server &other) {
	if (this != &other) {
		_fds = other._fds;
		_sockets = other._sockets;
		_extensionsToType = other._extensionsToType;
	}
	return *this;
}

void Server::copy_socket(std::vector<ServerSocket> other) {
	this->_sockets = other;
}

std::vector<ServerSocket> Server::get_Socket() {
	return _sockets;
}

void Server::display_Serv() {
	std::string msg = "";
	for (size_t i = 0; i < _sockets.size(); i++){
		std::cout << "Port-> ";
		if (_sockets[i]._port)
			std::cout << _sockets[i]._port << std::endl;
		else
			msg = "No port found";
		std::cout << "Server name-> ";
		if (!_sockets[i]._server_name.empty())
			std::cout << _sockets[i]._server_name << std::endl;
		else
			msg = "No server name found";
		std::cout << "------------------------" << std::endl;
		if (!msg.empty())
			throw(std::runtime_error(msg));
	}
}

std::string Server::get_content_type(const std::string &path) {
	std::string::size_type dot = path.rfind('.');
	if (dot == std::string::npos)
		return "application/octet-stream";

	std::string ext = path.substr(dot);
	std::map<std::string, std::string>::const_iterator it = _extensionsToType.find(ext);

	if (it != _extensionsToType.end())
		return it->second;
	else
		return "application/octet-stream";
}

void Server::init() {
	for (int i = 0; i < (int)_sockets.size(); i++) {
		_sockets[i].fd = socket(AF_INET, SOCK_STREAM, 0);
		if (_sockets[i].fd < 0)
			throw std::runtime_error("Failed to create socket");
		_listeningSockets[_sockets[i].fd] = _sockets[i];
		// Set the socket to non-blocking mode
		// This allows the server to handle multiple clients without blocking
		int flags = fcntl(_sockets[i].fd, F_GETFL, 0);
		if (flags == -1) {
			std::cerr << "fcntl F_GETFL" << std::endl;
			exit(1);
		}
		if (fcntl(_sockets[i].fd, F_SETFL, flags | O_NONBLOCK) == -1)
			throw std::runtime_error("fcntl F_SETFL");

		memset(&_sockets[i].address, 0, sizeof(_sockets[i].address));
		_sockets[i].address.sin_family = AF_INET;
		_sockets[i].address.sin_addr.s_addr = INADDR_ANY;
		_sockets[i].address.sin_port = htons(_sockets[i]._port);

		int opt = 1;
		if (setsockopt(_sockets[i].fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
			close(_sockets[i].fd);
			throw std::runtime_error("setsockopt failed");
		}

		if (bind(_sockets[i].fd, (struct sockaddr *)&_sockets[i].address, sizeof(_sockets[i].address)) < 0) {
			for (int j = 0; j <= i; j++)
				close(_sockets[j].fd);
			throw std::runtime_error("Failed to bind socket");
		}

		if (listen(_sockets[i].fd, 5) < 0) {
			for (int j = 0; j <= i; j++)
				close(_sockets[j].fd);
			throw std::runtime_error("Failed to listen on socket");
		}
	}
}

void Server::run() {
	for (size_t i = 0; i < _sockets.size(); i++) {
		struct pollfd pfd;
		pfd.fd = _sockets[i].fd;
		pfd.events = POLLIN;
		pfd.revents = 0;
		_fds.push_back(pfd);
	}

	while (g_running) {
		int ret = poll(_fds.data(), _fds.size(), 100);
		if (ret < 0) {
			if (errno == EINTR)
				continue;
			std::cerr << "Poll error" << std::endl;
			exit(EXIT_FAILURE);
		}
		if (ret == 0) continue;

		for (size_t i = 0; i < _fds.size(); ++i) {
			int fd = _fds[i].fd;
			int re = _fds[i].revents;

			if (re & POLLIN) {
				if (isServerSocket(fd)) {
					accept_client(fd);
				} else {
					Client *cl = _socketToClient[fd];
					if (!cl) continue;
					cl->readFromSocket();
					if (cl->shouldClose()) {
						disconnectClient(cl->get_fd());
						continue;
					}

					if (cl->tryParseRequest()) {
						handle_request(*cl);
						_fds[i].events = POLLOUT;
					}
				}
			}

			if (re & POLLOUT) {
				std::map<int, Client*>::iterator it = _socketToClient.find(fd);
				if (it == _socketToClient.end())
					continue;

				Client *cl = it->second;
				cl->writeToSocket();
				if (cl->shouldClose()){
					disconnectClient(cl->get_fd());
					continue;
				}
				if (cl->outputEmpty()) {
					if (cl->getReponse().isKeepAlive()) {
						cl->resetForNextRequest();
						_fds[i].events = POLLIN;
					} else {
						disconnectClient(fd);
					}
				}
			}
		}
	}
}

bool Server::isServerSocket(int fd) {
	for (size_t i = 0; i < _sockets.size(); i++)
		if (fd == _sockets[i].fd)
			return true;
	return false;
}

void Server::accept_client(int fd) {
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);

	int client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_len);
	if (client_fd < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return; // Nothing to accept
		std::cerr << "accept() failed on fd " << fd << ": "
				  << strerror(errno) << std::endl;
		return;
	}

	// make the client non-blocking
	int flags = fcntl(client_fd, F_GETFL, 0);
	if (flags == -1 || fcntl(client_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
		std::cerr << "fcntl() failed: " << strerror(errno) << std::endl;
		close(client_fd);
		return;
	}

	ServerSocket &server = _listeningSockets[fd];

    // create a new client and link it to the server
    Client* client = new Client(client_fd);
    _socketToClient[client_fd] = client;
    _clientToSocket[client_fd] = server;

	if (_listeningSockets.count(fd) == 0) {
        std::cerr << "Unknown listening socket fd: " << fd << std::endl;
        close(client_fd);
        return;
    }

	// add to poll()
	struct pollfd pfd;
	pfd.fd = client_fd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_fds.push_back(pfd);
	std::cout << "New client accepted on fd : " << client_fd << std::endl;
}

void Server::handle_request(Client &client) {
	const std::string method = client.getRequest().get_method();
	Reponse res;
	ServerSocket* server = &_clientToSocket[client.get_fd()];

	try {
		std::string url = client.getRequest().get_url();
		std::string locationPath;

		if (url == "/")
			locationPath = "/";
		else {
			size_t loca;
			loca = url.find_last_of("/");
			if (loca != std::string::npos) {
				locationPath = url.substr(loca);
			} else {
				locationPath = "/";
			}
		}
		std::string root = server->_root;
		std::string index = server->_index;

		std::map<std::string, std::string>::const_iterator rootIt = server->_locationRoots.find(locationPath);
		if (rootIt != server->_locationRoots.end() && !rootIt->second.empty()) {
			root = rootIt->second;
		}

		std::map<std::string, std::string>::const_iterator indexIt = server->_locationIndexes.find(locationPath);
		if (indexIt != server->_locationIndexes.end() && !indexIt->second.empty()) {
			index = indexIt->second;
		}

		size_t maxBodySize = server->_max_body_size;
		std::map<std::string, size_t>::const_iterator maxBodySizeIt = server->_locationMaxBodySizes.find(locationPath);
		if (maxBodySizeIt != server->_locationMaxBodySizes.end()) {
			maxBodySize = maxBodySizeIt->second;
		}

		std::map<std::string, std::string>::const_iterator returnIt = server->_returnPaths.find(locationPath);
		if (returnIt != server->_returnPaths.end() && !returnIt->second.empty()) {
			int redirectCode = 301;
			std::map<std::string, int>::const_iterator codeIt = server->_returnCodes.find(locationPath);
			if (codeIt != server->_returnCodes.end()) {
				redirectCode = codeIt->second;
			}
			res.set_status_code(redirectCode);
			if (redirectCode == 301)
			{
				res.set_status_text("Moved Permanently");
				server->_test = true;
			}
			else if (redirectCode == 302)
				res.set_status_text("Found");
			else if (redirectCode == 307)
				res.set_status_text("Temporary Redirect");
			else if (redirectCode == 308)
				res.set_status_text("Permanent Redirect");
			else
				res.set_status_text("Redirect");
			res.set_header("Location", returnIt->second);
			res.set_body("");
			res.set_header("Content-Length", "0");
			client.setResponse(res);
			server->_returnCode = redirectCode;
			return;
		}
		if (maxBodySize > 0) {
			size_t bodySize = 0;
			if (client.getRequest().hasHeader("Content-Length")) {
				std::string clHeader = client.getRequest().getHeader("Content-Length");
				size_t first = clHeader.find_first_not_of(" \t");
				if (first != std::string::npos) {
					clHeader = clHeader.substr(first);
					size_t last = clHeader.find_last_not_of(" \t");
					if (last != std::string::npos)
						clHeader = clHeader.substr(0, last + 1);
				}
				bodySize = static_cast<size_t>(atoi(clHeader.c_str()));
			} else {
				bodySize = client.getRequest().get_body().size();
			}
			if (bodySize > maxBodySize) {
				std::map<int, Reponse>::iterator it = server->_autoResponse.find(413);
				if (it != server->_autoResponse.end()) {
					res = it->second;
				} else {
					res.set_status_code(413);
					res.set_status_text("Payload Too Large");
					res.set_body("413 Payload Too Large\n");
					res.set_header("Content-Type", "text/plain");
					std::ostringstream oss;
					oss << res.get_body().size();
					res.set_header("Content-Length", oss.str());
				}
				client.setResponse(res);
				return;
			}
		}
		bool autoIndex = server->_autoIndex;
		std::map<std::string, bool>::const_iterator autoIndexIt = server->_locationAutoIndex.find(locationPath);
		if (autoIndexIt != server->_locationAutoIndex.end()) {
			autoIndex = autoIndexIt->second;
		}
		// gros problemme de merde a corriger sur les root, changer la root ici pour verif la method correctement notamment avec is_method_allowed

		if (!is_method_allowed(method, client, locationPath)) {
			res = server->_autoResponse[405];
		}
		if (method == "GET") {
			res = client.getRequest().handle_get(root, index, locationPath, autoIndex, &server->_test);
			if (res.get_status_code() == 500)
				res = _clientToSocket[client.get_fd()]._autoResponse[500];
			if (res.get_status_code() == 504)
				res = _clientToSocket[client.get_fd()]._autoResponse[500]; // 504 a envoyer sur page automatique
		} else if (method == "POST") {
			Request req = client.getRequest();
			std::string contentType = req.getHeader("Content-Type");
			std::string uploadDir = server->_upload_dir;
				std::map<std::string, std::string>::const_iterator uploadDirIt = server->_locationUploadDirs.find(locationPath);
				if (uploadDirIt != server->_locationUploadDirs.end() && !uploadDirIt->second.empty()) {
					uploadDir = uploadDirIt->second;
				}
				if (uploadDir.empty()) {
					uploadDir = root + locationPath;
				} else {
					if (uploadDir.size() >= 2 && uploadDir.substr(0, 2) == "./") {
						if (!root.empty()) {
							uploadDir = root + "/" + uploadDir.substr(2);
						} else {
							uploadDir = uploadDir.substr(2);
						}
					} else if (uploadDir[0] != '/') {
						if (!root.empty()) {
							uploadDir = root + "/" + uploadDir;
						}
					}
				}
			if (contentType.find("multipart/form-data") != std::string::npos) {
				if (handleFileUpload(req.get_body(), contentType, uploadDir)) {
					res.set_status_code(200);
					res.set_status_text("OK");
					res.set_header("Content-Type", "text/html");
					std::string boby ="<p style='color:green;'>Upload réussi !</p>";
					res.set_body(boby);
					std::ostringstream oss_len;
					oss_len << boby.size();
					res.set_header("Content-Length", oss_len.str());
				} else {
					res = server->_autoResponse[403];
				}
			} else {;
				res = client.getRequest().handle_post(uploadDir);
				if(res.get_status_code() == 500)
					res = _clientToSocket[client.get_fd()]._autoResponse[500];
				if(res.get_status_code() == 403)
					res = _clientToSocket[client.get_fd()]._autoResponse[403];
			}
		} else if (method == "DELETE") {
			res = client.getRequest().handle_delete();
			if (res.get_status_code() == 500)
				res = _clientToSocket[client.get_fd()]._autoResponse[500];
			if (res.get_status_code() == 403)
				res = _clientToSocket[client.get_fd()]._autoResponse[403];
		} else {
			res = server->_autoResponse[400];
		}
	} catch (std::runtime_error &e) {
		res = server->_autoResponse[404];
	}

	if (client.getRequest().getHeader("Connection") == "keep-alive") {
		res.set_header("Connection", "keep-alive");
		res.set_header("Keep-Alive", "timeout=20, max=100");
	} else {
		res.set_header("Connection", "close");
		client.setClosed();
	}
	client.setResponse(res);
}

bool Server::is_method_allowed(const std::string &method, Client &client, const std::string &locationPath) {
	ServerSocket *server = &_clientToSocket[client.get_fd()];
	int nummethode = 0;
	switch (method[0]) {
		case 'G':
			nummethode = 1;
			break;
		case 'P':
			nummethode = 2;
			break;
		case 'D':
			nummethode = 4;
			break;
		default:
			return false;
	}
	if ((server->_allowedMethods[locationPath] & nummethode) != 0)
		return true;
	return false;
}

void Server::disconnectClient(int fd) {
	std::map<int, Client*>::iterator it = _socketToClient.find(fd);
	if (it != _socketToClient.end()) {
		if (it->second)
			delete it->second;
		_socketToClient.erase(it);
	}
	close(fd);
	removeFdFromPoll(fd);
	std::cout << "Client (fd=" << fd << ") disconnected." << std::endl;
}

void Server::removeFdFromPoll(int fd) {
	for (std::vector<struct pollfd>::iterator it = _fds.begin(); it != _fds.end(); ++it) {
		if (it->fd == fd) {
			_fds.erase(it);
			return;
		}
	}
}

bool Server::handleFileUpload(const std::string& body,
							const std::string& contentType,
							std::string uploadDir)
{
	std::string boundaryKey = "boundary=";
	size_t bpos = contentType.find(boundaryKey);
	if (bpos == std::string::npos)
		return false;

	std::string boundary = "--" + contentType.substr(bpos + boundaryKey.size());
	size_t start = body.find(boundary);
	if (start == std::string::npos)
		return false;

	size_t headerEnd = body.find("\r\n\r\n", start);
	if (headerEnd == std::string::npos)
		return false;
	headerEnd += 4;

	size_t fnStart = body.find("filename=\"", start);
	if (fnStart == std::string::npos)
		return false;
	fnStart += 10;
	size_t fnEnd = body.find("\"", fnStart);
	if (fnEnd == std::string::npos)
		return false;

	std::string filename = body.substr(fnStart, fnEnd - fnStart);
	if (filename.empty())
		return false;

	size_t dataEnd = body.find(boundary, headerEnd);
	if (dataEnd == std::string::npos)
		return false;

	size_t fileSize = dataEnd - headerEnd - 2;
	struct stat dirStat;
	if (stat(uploadDir.c_str(), &dirStat) != 0) {    
		if (mkdir(uploadDir.c_str(), 0755) != 0) {
			std::cerr << "Failed to create upload directory: " << uploadDir << std::endl;
			return false;
		}
	} else if (!S_ISDIR(dirStat.st_mode)) {
		std::cerr << "Upload path is not a directory: " << uploadDir << std::endl;
		return false;
	}
	
	if (access(uploadDir.c_str(), W_OK) != 0) {
		std::cerr << "No write permission for upload directory: " << uploadDir << std::endl;
		return false;
	}
	
	std::ofstream file((uploadDir + "/" + filename).c_str(), std::ios::binary);
	if (!file.is_open())
		return false;

	file.write(&body[headerEnd], fileSize);
	file.close();

	return true;
}
