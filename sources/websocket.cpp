/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   websocket.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: peanut <peanut@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 15:44:47 by skapersk          #+#    #+#             */
/*   Updated: 2024/10/23 14:28:14 by peanut           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "websocket.hpp"

websocket::websocket(int domain, int service, int protocol, int port, unsigned long interface) {
	this->_address.sin_family = domain;
	this->_address.sin_addr.s_addr = interface;
	this->_address.sin_port = htons(port);
	this->_sock = socket(domain, service, protocol);
	// this->_connection = connectToNetwork(this->_sock, this->_address);
}

websocket::~websocket() {

}

websocket::websocket(const websocket &cpy) {
	*this = cpy;
}

websocket &websocket::operator=(const websocket &rhs) {
    if (this != &rhs) {
        this->_sock = rhs._sock;
        this->_connection = rhs._connection;
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

int	websocket::getSock() {
	return this->_sock;
}

int	websocket::getConnect() {
	return this->_connection;
}

int websocket::connectToNetwork() {
    // Implémentation par défaut (peut être redéfinie dans les classes dérivées)
    if (bind(this->_sock, (struct sockaddr *)&this->_address, sizeof(this->_address)) < 0) {
        std::cerr << "Erreur: Échec de la liaison" << std::endl;
        return -1;
    }
    return 0;
}


void websocket::initializeConnection() {
    this->_connection = connectToNetwork();
}
