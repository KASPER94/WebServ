/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/03 10:42:03 by skapersk          #+#    #+#             */
/*   Updated: 2024/11/03 14:30:32 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client():
	_request(NULL),
	_response(NULL)
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