/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/03 10:42:03 by skapersk          #+#    #+#             */
/*   Updated: 2024/11/04 14:26:09 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client():
	_request(NULL),
	_response(NULL)
	{}

Client::Client(int fd):
	_request(NULL),
	_response(NULL), 
	_fd(fd)
	{}

Client::Client(const Client &cpy) {
	*this = cpy;
}

Client::~Client() {
	if (this->_request)
		delete this->_request;
	if (this->_response)
		delete this->_response;
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

HttpRequest	*Client::getResponse() const {
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

void Client::setRequest(HttpRequest& request) {
	if (_request) {
        delete _request;  // Delete the existing request to avoid memory leaks
    }
    _request = new HttpRequest(request);  
}

HttpRequest& Client::getRequest() {
    return *_request;  
}