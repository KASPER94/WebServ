/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mank <ael-mank@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/03 10:42:03 by skapersk          #+#    #+#             */
/*   Updated: 2024/12/09 20:05:10 by ael-mank         ###   ########.fr       */
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
	_lastActivityTime = time(NULL) ;
	// cgiEnv = env()->envp;
	cgiEnv = NULL;
	logMsg(DEBUG, "Client socket " + toString(fd) + " created for communication with port " + toString(servers->getPort()));
}

Client::Client(const Client &cpy) {
	*this = cpy;
}

Client::~Client() {
    if (this->cgiEnv) {
        for (int i = 0; this->cgiEnv[i]; i++) {
            free(this->cgiEnv[i]);
        }
        delete[] this->cgiEnv;
        this->cgiEnv = NULL;
    }
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
	this->cgiEnv = rhs.cgiEnv;
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
    _lastActivityTime = time(NULL); 
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
    _lastActivityTime = time(NULL);
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
    if (_request) {
        delete _request;
        _request = NULL;
    }
    if (_response) {
        delete _response;
        _response = NULL;
    }
    if (cgiEnv) {
        for (int i = 0; cgiEnv[i]; i++) {
            free(cgiEnv[i]);
        }
        delete[] cgiEnv;
        cgiEnv = NULL;
    }
    _lastActivityTime = time(NULL); 
    _error = false;
}

bool Client::isTimeout() {
    return difftime(time(NULL), _lastActivityTime) > _timeout;
}

time_t Client::getLastActivityTime() const {
    return _lastActivityTime;
}

int Client::getTimeout() const {
    return _timeout;
}

void Client::setCgiEnv(char **mergedEnv) {
    if (this->cgiEnv) {
        for (int i = 0; this->cgiEnv[i]; i++) {
            free(this->cgiEnv[i]);
        }
        delete[] this->cgiEnv;
    }
    this->cgiEnv = mergedEnv;
}

char **Client::getCgiEnv() {
	return (cgiEnv);
}
