/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   websocket.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mank <ael-mank@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 15:44:47 by skapersk          #+#    #+#             */
/*   Updated: 2024/12/11 13:57:43 by ael-mank         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "websocket.hpp"

websocket::websocket(int domain, int service, int protocol, int port, unsigned long interface) {
	this->_address.sin_family = domain;
	this->_address.sin_addr.s_addr = interface;
	this->_address.sin_port = htons(port);
	(void)protocol;
	(void)service;
	_sock = -1;
}

websocket::~websocket() {
    if (_sock != -1) {
        close(_sock);
    }
}

websocket::websocket(const websocket &cpy) {
	*this = cpy;
}

websocket &websocket::operator=(const websocket &rhs) {
    if (this != &rhs) {
		if (_sock != -1) {
            close(_sock);
        }
        this->_address = rhs._address;
		this->_addrlen = rhs._addrlen;
		_sock = -1;
    }
    return *this;
}

void websocket::testConnection(int item) {
	if (item < 0) {
		std::perror("Failed to connect...");
		exit(EXIT_FAILURE);
	}
}

int	websocket::getSock() const {
	return this->_sock;
}

