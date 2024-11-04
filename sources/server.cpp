/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 16:52:53 by skapersk          #+#    #+#             */
/*   Updated: 2024/11/04 14:51:02 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(): websocket(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY), _port(8080), _host("127.0.0.1"), _name("default")  {
    // this->initializeConnection();
}

Server::~Server() {

}

Server::Server(const Server &cpy): websocket(AF_INET, SOCK_STREAM, 0, cpy._port, inet_addr(cpy._host.c_str())) {
    *this = cpy;
}

Server &Server::operator=(const Server &rhs) {
    this->_port = rhs._port;
	this->_host = rhs._host;
	this->_name = rhs._name;
	this->_directoryListing = rhs._directoryListing;
	this->_root = rhs._root;
	this->_indexes = rhs._indexes;
	this->_maxBodySize = rhs._maxBodySize;
	this->_allowedMethod = rhs._allowedMethod;
	this->_locationBlock = rhs._locationBlock;
	this->_errorPages = rhs._errorPages;
	this->_returnURI = rhs._returnURI;
	this->_uploadPath = rhs._uploadPath;
	this->_binPath = rhs._binPath;
	this->_cgiExtension = rhs._cgiExtension;
	return (*this);
}

int Server::connectToNetwork() {
    setSock();  // Set the socket address structure

    const int max_retries = 5;  // Maximum retries for binding
    int retry_count = 0;

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
    while (bind(this->_sock, (struct sockaddr *)&this->_address, sizeof(this->_address)) < 0) {
        if (++retry_count > max_retries) {
            std::cerr << "Erreur: Échec de la liaison après " << max_retries << " tentatives." << std::endl;
            close(this->_sock);
            return -1;
        }
        std::cerr << "Port " << this->_port << " en cours d'utilisation, tentative avec le port " << (this->_port + 1) << std::endl;
        this->_port++;  // Increment the port
        setSock();  // Update the socket address with the new port
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

void	Server::setPort(int port) {
	this->_port = port;
}

int	Server::getPort() {
	return (this->_port);
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
}

size_t	Server::getClientMaxBody() {
	if (!this->_maxBodySize)
		return (0);
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
