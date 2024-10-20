/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   websocket.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 15:44:47 by skapersk          #+#    #+#             */
/*   Updated: 2024/10/20 15:59:06 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "websocket.hpp"

websocket(int domain, int service, int protocol, int port, unsigned long interface) : {
	this->_address.sin_family = domain;
	this->_address.sin_addr.s_addr = interface;
	this->_sin_port = htons(port);
	this->_fd = socket(domain, service, protocol);
	this->_connection = connection()
}

~websocket() {

}

websocket(const websocket &cpy) {

}

websocket &operator=(const websocket &rhs) {

}
