/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/03 10:42:03 by skapersk          #+#    #+#             */
/*   Updated: 2024/11/14 11:40:40 by skapersk         ###   ########.fr       */
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
		    std::cout << "Client created at " << this << " for socket " << fd << std::endl;

}

Client::Client(const Client &cpy) {
	*this = cpy;
}

Client::~Client() {
	if (this->_request)
		delete this->_request;
	if (this->_response)
		delete this->_response;
	 std::cout << "Client destroyed at " << this << std::endl;
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

HttpRequest& Client::getRequest() {
    return *_request;
}

bool	Client::error() const {
	return (this->_error);
}

bool	Client::appendRequest(const char *data, int bytes) {
	if (!this->_request)
		this->_request = new HttpRequest(this);
	// this->_creationDate = time(0);
	return (this->_request->appendRequest(data, bytes));
}

Server	*Client::getServer() const {
	std::cout << this->_servers << std::endl;
	return (this->_servers);
}

void	Client::sendResponse() {
	this->_response = new HttpReponse(this);
	this->_reponse->sendResponse();
}
