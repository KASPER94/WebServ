/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 16:52:53 by skapersk          #+#    #+#             */
/*   Updated: 2024/12/09 23:48:21 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.h"
#include "Server.hpp"

Server::Server(): websocket(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY), _port(8080), _host("127.0.0.1"), _name("default"), _uri(NULL)  {
    // this->initializeConnection();
}

Server::~Server() {
	cleanUp();
	delete _uri;
	for (std::map<std::string, Location*>::iterator it = _locations.begin(); 
             it != _locations.end(); ++it) {
            delete it->second;
        }
	_locations.clear();
}

Server::Server(const Server &cpy): websocket(AF_INET, SOCK_STREAM, 0, cpy._port, inet_addr(cpy._host.c_str())) {
    _uri = NULL;
    _port = cpy._port;
    _host = cpy._host;
    _name = cpy._name;
    _directoryListing = cpy._directoryListing;
    _root = cpy._root;
    _indexes = cpy._indexes;
    _maxBodySize = cpy._maxBodySize;
    _allowedMethod = cpy._allowedMethod;
    _returnURI = cpy._returnURI;
    _uploadPath = cpy._uploadPath;
    _binPath = cpy._binPath;
    _cgiExtensions = cpy._cgiExtensions;
    _cgiBin = cpy._cgiBin;
    _autoindex = cpy._autoindex;

    // Deep copy URI
    if (cpy._uri) {
        _uri = new std::vector<std::string>(*cpy._uri);
    }

    // Deep copy locations
    for (std::map<std::string, Location *>::const_iterator it = cpy._locations.begin(); 
         it != cpy._locations.end(); ++it) {
        _locations[it->first] = new Location(*it->second);
    }
	// _sock = dup(cpy._sock);
    // if (_sock == -1) {
    //     std::perror("Failed to duplicate socket");
    //     exit(EXIT_FAILURE);
    // }
	_sock = -1;
}

Server &Server::operator=(const Server &rhs) {
    if (this != &rhs) {
		cleanUp();
        // Clean existing data
        delete _uri;
        _uri = NULL;
        for (std::map<std::string, Location *>::iterator it = _locations.begin(); 
             it != _locations.end(); ++it) {
            delete it->second;
        }
        _locations.clear();

        // Copy basic members
        _port = rhs._port;
        _host = rhs._host;
        _name = rhs._name;
        _directoryListing = rhs._directoryListing;
        _root = rhs._root;
        _indexes = rhs._indexes;
        _maxBodySize = rhs._maxBodySize;
        _allowedMethod = rhs._allowedMethod;
        _returnURI = rhs._returnURI;
        _uploadPath = rhs._uploadPath;
        _binPath = rhs._binPath;
        _cgiExtensions = rhs._cgiExtensions;
        _cgiBin = rhs._cgiBin;
        _autoindex = rhs._autoindex;

        // Deep copy URI
        if (rhs._uri) {
            _uri = new std::vector<std::string>(*rhs._uri);
        }

        // Deep copy locations
        for (std::map<std::string, Location *>::const_iterator it = rhs._locations.begin(); 
             it != rhs._locations.end(); ++it) {
            _locations[it->first] = new Location(*it->second);
        }
		// _sock = dup(rhs._sock);
		// if (_sock == -1) {
		// 	std::perror("Failed to duplicate socket");
		// 	exit(EXIT_FAILURE);
		// }
		_sock = -1;
    }
    return *this;
}

int Server::connectToNetwork() {
	setSock();  // Set the socket address structure

	// const int max_retries = 5;  // Maximum retries for binding
	// int retry_count = 0;

	// Create the socket
	this->_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_sock == -1) {
		logMsg(ERROR, "Failed to create socket: " + std::string(strerror(errno)));
		return -1;
	}
	// Set the socket options to reuse the address
	int yes = 1;
	if (setsockopt(this->_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
		logMsg(ERROR, "Failed to set socket options: " + std::string(strerror(errno)));
		// return -1;
		close(this->_sock);
		return -1;
	}

	// Attempt to bind the socket with retry logic
	if (bind(this->_sock, (struct sockaddr *)&this->_address, sizeof(this->_address)) < 0) {
		if (errno == EADDRINUSE) {
			logMsg(ERROR, "Failed to bind socket to port " + toString(this->_port) + ": port already in use");
		} else {
			logMsg(ERROR, "Failed to bind socket to port" + toString(this->_port) + ": " + std::string(strerror(errno)));
		}
		close(this->_sock);
		return -1;
	}

	// Listen on the socket for incoming connections
	if (listen(this->_sock, 10) < 0) {
		logMsg(ERROR, "Failed to listen on port " + toString(this->_port) + ": " + std::string(strerror(errno)));
		close(this->_sock);
		return -1;
	}

	logMsg(INFO, "Server is successfully listening on port " + toString(this->_port));
	return 0;
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

void Server::setAllowedMethods(std::vector<std::string> &methods) {
    // this->_allowedMethod.clear();
    // this->_allowedMethod.insert(_allowedMethod.end(), methods.begin(), methods.end());
	this->_allowedMethod = methods;
}

std::vector<std::string> &Server::getAllowedMethods() {
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
    _port = 0;
    _host.clear();
    
    // Clean up URI
    delete _uri;
    _uri = new std::vector<std::string>;
    
    // Clean up locations
    for (std::map<std::string, Location *>::iterator it = _locations.begin(); 
         it != _locations.end(); ++it) {
        delete it->second;
    }
    _locations.clear();
    
    _name.clear();
    _indexes.clear();
    _errorPages.clear();
    _maxBodySize = 0;
    _allowedMethod.clear();
    _returnURI.clear();
    _root.clear();
    _uploadPath.clear();
    _binPath.clear();
    _cgiExtensions.clear();
    _cgiBin.clear();
    _autoindex = false;
}

void Server::cleanUp() {
	if (_sock != -1) {
		close(_sock);
		_sock = -1;
	}
	// logMsg(INFO, "Resources for server on port " + toString(_port) + " have been cleaned up");
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

std::string Server::getErrorPage(int code) const {
    std::map<int, std::string>::const_iterator it = this->_errorPages.find(code);
    if (it != this->_errorPages.end()) {
        return it->second; // Retourne la page d'erreur configurée au niveau du serveur
    }
    return "";
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
