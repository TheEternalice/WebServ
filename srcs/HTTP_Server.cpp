/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP_Server.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 15:47:12 by lde-merc          #+#    #+#             */
/*   Updated: 2025/10/03 16:50:46 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/HTTP_Server.hpp"

HTTP_Server::HTTP_Server()
{
}
HTTP_Server::~HTTP_Server()
{
}
HTTP_Server::HTTP_Server(const HTTP_Server &other)
{
	*this = other;
}

HTTP_Server &HTTP_Server::operator=(const HTTP_Server &other)
{
	if (this != &other)
	{
		this->_sockets = other._sockets;
		this->_mimeTypes = other._mimeTypes;
		this->_clientServer = other._clientServer;
		this->_pollfds = other._pollfds;
	}
	return (*this);
}

HTTP_Server::HTTP_Server(const char *configFile)
{
	parsing(std::string(configFile));
	load_mime_types();
}

void HTTP_Server::load_mime_types()
{
	std::ifstream file("mime.types");
	if (!file.is_open())
		throw ConfigException("Could not open mime.types");
	std::string line;
	while (std::getline(file, line))
	{
		// Ignore comments et lignes vides
		if (line.empty() || line[0] == '#')
			continue ;
		std::istringstream iss(line);
		std::string mime_type;
		if (!(iss >> mime_type))
			continue ;
		std::string ext;
		while (iss >> ext)
		{
			if (ext[0] != '.')
				ext = "." + ext; // normaliser avec un point
			_mimeTypes[ext] = mime_type;
		}
	}
	file.close();
}

void HTTP_Server::initialize_sockets()
{
	for (size_t i = 0; i < _sockets.size(); i++)
	{
		_sockets[i].initialize_socket();
	}
}

Client* HTTP_Server::getClientByFd(int fd)
{
	std::map<int, Client*>::const_iterator it = _clientServer.find(fd);
	if (it != _clientServer.end())
		return it->second;
	return NULL;
}

void HTTP_Server::run()
{
	int	fd;

	for (size_t i = 0; i < _pollfds.size(); i++)
	{
		fd = _pollfds[i].fd;
		if (_pollfds[i].revents & POLLIN)
		{
			if (_socketServer.count(fd))
			{
				accept_client(fd);
			}
			else if (_clientServer.count(fd))
			{
				Client* cl = _clientServer[fd];
				if (_pollfds[i].revents & POLLIN) {
				cl->readFromSocket();
				if (cl->requestComplete()) {
					cl->parseRequest();
					std::string resp = handle_http(cl);
					cl->setResponse(resp);
					// activer POLLOUT si on a quelque chose à envoyer
					_pollfds[i].events |= POLLOUT;
				}
			}

			if (_pollfds[i].revents & POLLOUT) {
				cl->writeToSocket();
				if (cl->outputEmpty())
					_pollfds[i].events &= ~POLLOUT;
			}
			}
		}
	}
}

void HTTP_Server::accept_client(int fd)
{
	sockaddr_in client_addr;
	socklen_t addr_len = sizeof(client_addr);
	int client_fd = accept(fd, (struct sockaddr *)&client_addr, &addr_len);
	if (client_fd < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return ; // rien à accepter
		std::cerr << "Accept failed: " << strerror(errno) << std::endl;
		return ;
	}

	// Rendre le socket non-bloquant
	int flags = fcntl(client_fd, F_GETFL, 0);
	if (flags == -1 || fcntl(client_fd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		std::cerr << "Failed to set non-blocking mode: " << strerror(errno) << std::endl;
		close(client_fd);
		return ;
	}

	// Ajouter le nouveau client à la liste des clients
	_clientServer[client_fd] = new Client(client_fd, client_addr);

	// Ajouter le nouveau client au tableau pollfd
	struct pollfd pfd;
	pfd.fd = client_fd;
	pfd.events = POLLIN; // Écouter pour lecture
	pfd.revents = 0;
	_pollfds.push_back(pfd);

	std::cout << "New client connected: FD=" << client_fd << std::endl;
}