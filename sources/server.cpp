/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 16:52:53 by skapersk          #+#    #+#             */
/*   Updated: 2024/10/25 11:55:07 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(): websocket(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY), _port(8080), _host("127.0.0.1"), _name("default")  {
    this->initializeConnection();
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
    if (bind(this->_sock, (struct sockaddr *)&this->_address, sizeof(this->_address)) < 0) {
        std::cerr << "Erreur: Échec de la liaison" << std::endl;
        return -1;
    }
    return (0);
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
