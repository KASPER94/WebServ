/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   confUtils.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 18:46:09 by peanut            #+#    #+#             */
/*   Updated: 2024/10/24 15:34:53 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "conf.hpp"

int    conf::_getListen(std::vector<std::string> line) {
	if (line.size() != 2)
		throw std::runtime_error("Error: listen directive requires exactly one argument (port)");

	const std::string& portStr = line[1];
	if (portStr[portStr.size() - 1] != ';')
			throw std::runtime_error("Error: missing ';'");
	for (size_t i = 0; i < portStr.size()- 1; ++i) {
		if (!std::isdigit(portStr[i])) {
			throw std::runtime_error("Error: listen directive requires a valid port number");
		}
	}

	std::istringstream nb(portStr);
	int port;
	nb >> port;
	if (nb.fail() || port < 0 || port > 65535) {
		throw std::runtime_error("Error: listen directive requires a port number between 0 and 65535");
	}
	return port;
}

std::string    conf::_getServerName(std::vector<std::string> line) {
	if (line.size() != 2)
		throw std::runtime_error("Error: server_name directive requires exactly one argument (server_name)");
	if (line[1][line[1].size() - 1] != ';')
		throw std::runtime_error("Error: missing ';'");
	std::string serverName = line[1].substr(0, line[1].size() - 1);
	if (serverName.empty())
		throw std::runtime_error("Error: missing server_name");

	return serverName;
}

void    conf::_getHost(std::vector<std::string> line) {
    std::vector<std::string>::iterator it;

    it = line.begin();
    for (; it != line.end(); it++) {
        std::cout << *it << std::endl;
    }
}

void    conf::_getIndex(std::vector<std::string> line) {
    std::vector<std::string>::iterator it;

    it = line.begin();
    for (; it != line.end(); it++) {
        std::cout << *it << std::endl;
    }
}

void    conf::_getErrorPage(std::vector<std::string> line) {
    std::vector<std::string>::iterator it;

    it = line.begin();
    for (; it != line.end(); it++) {
        std::cout << *it << std::endl;
    }
}

void    conf::_getClientMaxBodySize(std::vector<std::string> line) {
    std::vector<std::string>::iterator it;

    it = line.begin();
    for (; it != line.end(); it++) {
        std::cout << *it << std::endl;
    }
}

void    conf::_getAllowedMethods(std::vector<std::string> line) {
    std::vector<std::string>::iterator it;

    it = line.begin();
    for (; it != line.end(); it++) {
        std::cout << *it << std::endl;
    }
}

void    conf::_getAutoindex(std::vector<std::string> line) {
    std::vector<std::string>::iterator it;

    it = line.begin();
    for (; it != line.end(); it++) {
        std::cout << *it << std::endl;
    }
}

void    conf::_getUploadPath(std::vector<std::string> line) {
    std::vector<std::string>::iterator it;

    it = line.begin();
    for (; it != line.end(); it++) {
        std::cout << *it << std::endl;
    }
}

void    conf::_getRoot(std::vector<std::string> line) {
    std::vector<std::string>::iterator it;

    it = line.begin();
    for (; it != line.end(); it++) {
        std::cout << *it << std::endl;
    }
}

void    conf::_getRedirection(std::vector<std::string> line) {
    std::vector<std::string>::iterator it;

    it = line.begin();
    for (; it != line.end(); it++) {
        std::cout << *it << std::endl;
    }
}

void    conf::_getCgiExtension(std::vector<std::string> line) {
    std::vector<std::string>::iterator it;

    it = line.begin();
    for (; it != line.end(); it++) {
        std::cout << *it << std::endl;
    }
}

void    conf::_getCgiPath(std::vector<std::string> line) {
    std::vector<std::string>::iterator it;

    it = line.begin();
    for (; it != line.end(); it++) {
        std::cout << *it << std::endl;
    }
}

void    conf::_getCgiBin(std::vector<std::string> line) {
    std::vector<std::string>::iterator it;

    it = line.begin();
    for (; it != line.end(); it++) {
        std::cout << *it << std::endl;
    }
}
