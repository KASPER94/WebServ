/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrigny <yrigny@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 16:52:53 by skapersk          #+#    #+#             */
<<<<<<< HEAD
/*   Updated: 2024/12/05 13:31:38 by skapersk         ###   ########.fr       */
=======
/*   Updated: 2024/12/04 16:59:59 by yrigny           ###   ########.fr       */
>>>>>>> 1827865 (adding logMsg() function)
/*                                                                            */
/* ************************************************************************** */

#include "webserv.h"
#include "Server.hpp"

Server::Server(): websocket(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY), _port(8080), _host("127.0.0.1"), _name("default"), _uri(NULL)  {
    // this->initializeConnection();
}

Server::~Server() {
	// delete _uri;
}

Server::Server(const Server &cpy): websocket(AF_INET, SOCK_STREAM, 0, cpy._port, inet_addr(cpy._host.c_str())) {
    *this = cpy;
}

Server &Server::operator=(const Server &rhs) {
    if (this != &rhs) {
        this->_port = rhs._port;
        this->_host = rhs._host;
        this->_name = rhs._name;
        this->_directoryListing = rhs._directoryListing;
        this->_root = rhs._root;
        this->_indexes = rhs._indexes;
        this->_maxBodySize = rhs._maxBodySize;
        this->_allowedMethod = rhs._allowedMethod;
        this->_returnURI = rhs._returnURI;
        this->_uploadPath = rhs._uploadPath;
        this->_binPath = rhs._binPath;
        this->_cgiExtensions = rhs._cgiExtensions;
        this->_cgiBin = rhs._cgiBin;
		this->_autoindex = rhs._autoindex;


        for (std::map<std::string, Location *>::iterator it = _locations.begin(); it != _locations.end(); ++it) {
            delete it->second;
        }
        _locations.clear();
        for (std::map<std::string, Location *>::const_iterator it = rhs._locations.begin(); it != rhs._locations.end(); ++it) {
            _locations[it->first] = new Location(*it->second);
            // std::cout << (_locations[it->first])->getRoot() << std::endl;
            // std::cout << *(_locations[it->first])->getCgiExtension().begin() << std::endl;
        }

        if (rhs._uri) {
            this->_uri = new std::vector<std::string>(*rhs._uri);
        } else {
            this->_uri = NULL;
        }
    }
    return *this;
}

int Server::connectToNetwork() {
<<<<<<< HEAD
	setSock();  // Set the socket address structure
=======
	this->_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_sock < 0) {
		logMsg(ERROR, "socket() failed");
		return -1;
	}
	this->_address.sin_family = AF_INET;
	this->_address.sin_port = htons(this->_port);
	this->_address.sin_addr.s_addr = inet_addr(this->_host.c_str());
>>>>>>> 1827865 (adding logMsg() function)

	// const int max_retries = 5;  // Maximum retries for binding
	// int retry_count = 0;

	// Create the socket
	this->_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_sock == -1) {
		std::cerr << "[DEBUG] Failed to create socket: " << strerror(errno) << std::endl;
		return -1;
	}
	// Set the socket options to reuse the address
	int yes = 1;
	if (setsockopt(this->_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
		std::cerr << "[DEBUG] Failed to set socket options: " << strerror(errno) << std::endl;
		close(this->_sock);
		return -1;
	}

	// Attempt to bind the socket with retry logic
	if (bind(this->_sock, (struct sockaddr *)&this->_address, sizeof(this->_address)) < 0) {
<<<<<<< HEAD
		if (errno == EADDRINUSE) {
			std::cerr << "[DEBUG] Port " << this->_port
						<< " is already in use. Binding failed." << std::endl;
		} else {
			std::cerr << "[DEBUG] Failed to bind socket on port "
						<< this->_port << ": " << strerror(errno) << std::endl;
		}
		close(this->_sock);
=======
		logMsg(ERROR, "bind() failed");
>>>>>>> 1827865 (adding logMsg() function)
		return -1;
	}

	// Listen on the socket for incoming connections
	if (listen(this->_sock, 10) < 0) {
<<<<<<< HEAD
		std::cerr << "[DEBUG] Failed to listen on port "
					<< this->_port << ": " << strerror(errno) << std::endl;
		close(this->_sock);
		return -1;
	}

	std::cout << "[DEBUG] Server is successfully listening on port: " << this->_port << std::endl;
	return 0;
=======
		logMsg(ERROR, "listen() failed");
		return -1;
	}
	logMsg(INFO, "New server listening to port " + toString(this->_port));
	return (0);
>>>>>>> 1827865 (adding logMsg() function)
}

void Server::setCgiBin(const std::string &cgiBin) { _cgiBin = cgiBin; }
void Server::setCgiExtension(const std::vector<std::string> &cgiExtensions) { _cgiExtensions = cgiExtensions; }
void Server::setUploadPath(const std::string &uploadPath) { _uploadPath = uploadPath; }
void Server::setAutoindex(bool autoindex) { _autoindex = autoindex; }

std::string Server::getCgiBin() const { return _cgiBin; }
std::vector<std::string> Server::getCgiExtension() const { return _cgiExtensions; }
std::string Server::getUploadPath() const { return _uploadPath; }
bool Server::getAutoindex() const { return _autoindex; }

void	Server::setPort(int port) {
	this->_port = port;
}

int	Server::getPort() {
	return (this->_port);
}

void	Server::setIndexes(std::vector<std::string> indexes){
	_indexes = indexes;
}

std::vector<std::string> Server::getIndexes() {
	return (_indexes);
}

void	Server::setRoot(std::string root) {
	this->_root = root;
}

std::string	Server::getRoot() {
	return (this->_root);
}

void	Server::setServerName(std::string name) {
	this->_name = name;
}

std::string		Server::getServerName(){
	return (this->_name);
}

void	Server::setHostname(std::string host) {
	this->_host = host;
}

std::string		Server::getHostname(){
	return (this->_host);
}

void	Server::setErrorPage(std::map<int, std::string> Error) {
	this->_errorPages = Error;
}

std::map<int, std::string> Server::getErrorPage() {
	return (this->_errorPages);
}

void	Server::setClientMaxBody(size_t body) {
	this->_maxBodySize = body;
	// std::cout << "....000 " << _maxBodySize << std::endl;
}

size_t	Server::getClientMaxBody() {
	// std::cout << ".....  ... "<< this->_maxBodySize << std::endl;
	// if (!this->_maxBodySize)
	// 	return (0);
	return (this->_maxBodySize);
}

void Server::setAllowedMethods(std::vector<std::string> *methods) {
    // this->_allowedMethod.clear();
    // this->_allowedMethod.insert(_allowedMethod.end(), methods.begin(), methods.end());
	this->_allowedMethod = methods;
}

std::vector<std::string> *Server::getAllowedMethods() {
    return _allowedMethod;
}

void	Server::setSock() {
	this->_address.sin_family = AF_INET;
	this->_address.sin_port = htons(this->_port);
	this->_address.sin_addr.s_addr = inet_addr(this->_host.c_str());
}

void Server::setUri(std::string uri) {
	if (!_uri) {
		_uri = new std::vector<std::string>;
	}
	_uri->push_back(uri);
}

std::vector<std::string> *Server::getUri() {
	return (_uri);
}

void Server::setReturnUri(std::map<int, std::string> returnuri) {
	// if (!_returnURI) {
	// 	_returnURI = new std::map<int, std::string>;
	// }
	_returnURI = returnuri;
}

std::map<int, std::string> Server::getReturnUri() const {
	return (_returnURI);
}

void Server::addLocation(const std::string &uri, const Location &location) {
	if (_locations.find(uri) != _locations.end()) {
        delete _locations[uri];
    }
    _locations[uri] = new Location(location);
	setUri(uri);
}

void Server::reset() {
    // Réinitialise tous les membres de l'objet
    this->_port = 0;
    this->_host.clear();
    if (this->_uri) {
        this->_uri->clear(); // Efface les éléments existants
    }
	this->_uri = new std::vector<std::string>; // Crée une nouvelle instance si NULL
    this->_name.clear();
    this->_indexes.clear();
    this->_errorPages.clear();
    this->_locations.clear();
    this->_maxBodySize = 0;
    // this->_allowedMethod->clear();
	//REVENIR SUR LE RESET DU POINTEUR ALLOWED MTHODE
    this->_returnURI.clear();
    this->_root.clear();
    // Réinitialise d'autres champs si nécessaire
}

void Server::cleanUp() {
	if (_sock != -1) {
		close(_sock);
		_sock = -1;
	}
	std::cout << "[INFO] Resources for server on port " << _port
				<< " have been cleaned up." << std::endl;
}

Location *Server::getLocation(const std::string &uri) const {
	std::map<std::string, Location*>::const_iterator it = _locations.find(uri);
	std::cout << (it == _locations.end()) << std::endl;

	if (it != _locations.end()) {
		return it->second;
	}
	return (NULL);
	// throw std::runtime_error("Location not found for URI: " + uri);
}

std::map<std::string, Location*> Server::returnLoc() {
	return (_locations);
}

std::ostream	&operator<<(std::ostream &o, Server &server) {
	o << server.getServerName() << " → ";
	o << server.getHostname() << ":" << server.getPort();
	o << "\n";
    o << "URIs:\n";
    std::map<std::string, Location*> locations = server.returnLoc();
    for (std::map<std::string, Location*>::const_iterator it = locations.begin(); it != locations.end(); ++it) {
		std::string tmp = (it->second)->getRoot();
        o << " - " << tmp << "\n";
        const std::vector<std::string> &cgiExtensions = (it->second)->getCgiExtension();
        if (!cgiExtensions.empty()) {
            std::string tmp2 = cgiExtensions.front();
            o << " - " << tmp2 << "\n";
        } else {
            o << " - No CGI Extensions\n";
        }
		std::string indexes = (it->second)->getIndex();
        if (!indexes.empty()) {
            o << " - Indexes: ";
			o << indexes;
            o << "\n";
        } else {
            o << " - No Indexes Defined\n";
        }
    }
	o << " (fd: " << server.getSock() << ")";
	o << " ok -----> " << server.getCgiBin();
	return(o);
}
