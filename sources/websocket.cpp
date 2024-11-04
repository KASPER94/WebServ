/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   websocket.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 15:44:47 by skapersk          #+#    #+#             */
/*   Updated: 2024/11/04 15:50:54 by skapersk         ###   ########.fr       */
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

}

websocket::websocket(const websocket &cpy) {
	*this = cpy;
}

websocket &websocket::operator=(const websocket &rhs) {
    if (this != &rhs) {
        this->_sock = rhs._sock;
        this->_address = rhs._address;
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

