/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 16:52:53 by skapersk          #+#    #+#             */
/*   Updated: 2024/11/20 17:20:33 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

        for (std::map<std::string, Location *>::iterator it = _locations.begin(); it != _locations.end(); ++it) {
            delete it->second;
        }
        _locations.clear();
        for (std::map<std::string, Location *>::const_iterator it = rhs._locations.begin(); it != rhs._locations.end(); ++it) {
            _locations[it->first] = new Location(*it->second);
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
    setSock();  // Set the socket address structure

    // const int max_retries = 5;  // Maximum retries for binding
    // int retry_count = 0;

    // Create the socket
    this->_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (this->_sock == -1) {
        std::cerr << "Erreur: Échec de la création du socket" << std::endl;
        return -1;
    }

    // Set the socket options to reuse the address
    int yes = 1;
    if (setsockopt(this->_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
        perror("Erreur: setsockopt()");
        close(this->_sock);
        return -1;
    }

    // Attempt to bind the socket with retry logic
    if (bind(this->_sock, (struct sockaddr *)&this->_address, sizeof(this->_address)) < 0) {
        // if (++retry_count > max_retries) {
        //     std::cerr << "Erreur: Échec de la liaison après " << max_retries << " tentatives." << std::endl;
        //     close(this->_sock);
        //     return -1;
        // }
        std::cerr << "Port " << this->_port << " en cours d'utilisation, tentative avec le port " << (this->_port + 1) << std::endl;
        // this->_port++;  // Increment the port
        // setSock();  // Update the socket address with the new port
    }

    // Listen on the socket for incoming connections
    if (listen(this->_sock, 10) < 0) {
        std::cerr << "Erreur: Échec de l'écoute" << std::endl;
        close(this->_sock);
        return -1;
    }

    std::cout << "Serveur en écoute sur le port: " << this->_port << std::endl;
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
	std::cout << "URIs actuels : ";
    for (size_t i = 0; i < _uri->size(); ++i) {
        std::cout << _uri->at(i) << " ";
    }
    std::cout << std::endl;
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
    }
	o << " (fd: " << server.getSock() << ")";
	return(o);
}