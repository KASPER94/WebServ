/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   confUtils.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 18:46:09 by peanut            #+#    #+#             */
/*   Updated: 2024/10/24 17:32:08 by skapersk         ###   ########.fr       */
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

bool allDigit(std::string &seg) {
	if (seg.empty())
		return false;
	for (size_t i = 0; i < seg.size(); ++i) {
		if (!isdigit(seg[i])) {
			return false;
		}
	}
	int num = atoi(seg.c_str());
	return (num >= 0 && num <= 255);
}

bool isValidIpAdd(std::string &ip) {
	int	numDots = 0;
	std::string seg;

	for (size_t i = 0; i < ip.length(); i++){
		if (ip[i] == '.') {
			numDots++;
			if (seg.empty() || !allDigit(seg) || atoi(seg.c_str()) > 255) {
				return (false);
			}
			seg.clear();
		}
		else if (isdigit(ip[i])) {
			seg += ip[i];
		}
		else
			return (false);
	}
	if (seg.empty() || !allDigit(seg) || atoi(seg.c_str()) > 255)
		return (false);
	return (numDots == 3);
}

bool isValidHostname(std::string &host) {
	if (host.empty() || host.length() > 63)
		return (false);
	for (size_t i = 0; i < host.length(); i++) {
		if (!isalnum(host[i]) && host[i] != '-')
			return false;
		if ((i == 0 || i == host.length() - 1) && host[i] == '-')
		return (false);
	}
	return (true);
}

std::string    conf::_getHost(std::vector<std::string> line) {
	if (line.size() != 2)
		throw std::runtime_error("Error: host directive requires exactly one argument (host)");
	if (line[1][line[1].size() - 1] != ';')
		throw std::runtime_error("Error: missing ';'");
	std::string host = line[1].substr(0, line[1].size() - 1);
	if (!isValidIpAdd(host)) {
		if (!isValidHostname(host))
			throw std::runtime_error("Error: invalid host name (must be a valid IP or hostname)");
	}
	return host;
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
