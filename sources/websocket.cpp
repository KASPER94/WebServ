/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   websocket.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 15:44:47 by skapersk          #+#    #+#             */
/*   Updated: 2024/12/09 12:33:10 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "websocket.hpp"

websocket::websocket(int domain, int service, int protocol, int port, unsigned long interface) {
	this->_address.sin_family = domain;
	this->_address.sin_addr.s_addr = interface;
	this->_address.sin_port = htons(port);
	this->_sock = socket(domain, service, protocol);
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
		_sock = dup(rhs._sock);
        if (_sock == -1) {
            std::perror("Failed to duplicate socket");
            exit(EXIT_FAILURE);
        }
        this->_address = rhs._address;
		this->_addrlen = rhs._addrlen;
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

