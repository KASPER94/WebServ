/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/03 10:42:03 by skapersk          #+#    #+#             */
/*   Updated: 2024/12/07 00:29:36 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client():
	_request(NULL),
	_response(NULL)
	{}

// Client::Client(int fd):
// 	_request(NULL),
// 	_response(NULL),
// 	_fd(fd)
// 	{}

Client::Client(int fd, Server *servers):
	_request(NULL),
	_response(NULL),
	 _fd(fd),
	 _error(false),
	 _servers(servers) {
	logMsg(DEBUG, "Client socket " + toString(fd) + " created for communication with port " + toString(servers->getPort()));
}

Client::Client(const Client &cpy) {
	*this = cpy;
}

Client::~Client() {
	if (this->_request)
		delete this->_request;
	if (this->_response)
		delete this->_response;
	logMsg(DEBUG, "Client socket " + toString(_fd) + " destroyed");
}

Client	&Client::operator=(const Client &rhs) {
	this->_request = rhs._request;
	this->_response = rhs._response;
	this->_fd = rhs._fd;
	return (*this);
}

HttpRequest	*Client::getRequest() const {
	return (this->_request);
}

HttpResponse	*Client::getResponse() const {
	return (this->_response);
}

int Client::getFd() const {
    return _fd;
}

void Client::setFd(int fd) {
    if (_fd >= 0)
		close(_fd);
    _fd = fd;
}

// HttpRequest& Client::getRequest() {
//     return *_request;
// }

bool	Client::getError() const {
	return (this->_error);
}

bool	Client::appendRequest(const char *data, int bytes) {
	if (!this->_request)
		this->_request = new HttpRequest(this);
	// this->_creationDate = time(0);
	return (this->_request->appendRequest(data, bytes));
}

Server	*Client::getServer() const {
	// std::cout << this->_servers << std::endl;
	return (this->_servers);
}

void	Client::sendResponse() {
	this->_response = new HttpResponse(this);
	this->_response->sendResponse();
}

std::string	Client::getResponsestr() const {
	return (this->_response->getResponse());
}

void	Client::setError() {
	this->_error = true;
}

void Client::resetForNextRequest() {
	logMsg(DEBUG, "Client using fd " + toString(_fd) + " reset for next request");
    if (_request) {
        delete _request;  // Libère la mémoire allouée à la requête précédente
        _request = NULL;
    }

    if (_response) {
        delete _response;  // Libère la mémoire allouée à la réponse précédente
        _response = NULL;
    }

    _request = new HttpRequest(this);  // Crée une nouvelle requête
    _response = new HttpResponse(this);  // Crée une nouvelle réponse

    // Réinitialise les erreurs et autres indicateurs
    _error = false;
}

