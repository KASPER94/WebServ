/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   confUtils.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: peanut <peanut@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 18:46:09 by peanut            #+#    #+#             */
/*   Updated: 2024/10/22 21:01:20 by peanut           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "conf.hpp"

void    conf::_getListen(std::vector<std::string> line) {
    std::vector<std::string>::iterator it;

    it = line.begin();
    for (; it != line.end(); it++) {
        std::cout << *it << std::endl;
    }
}

void    conf::_getServerName(std::vector<std::string> line) {
    std::vector<std::string>::iterator it;

    it = line.begin();
    for (; it != line.end(); it++) {
        std::cout << *it << std::endl;
    }
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

// void    conf::_getLocation(std::vector<std::string> line) {
//     std::vector<std::string>::iterator it;

//     it = line.begin();
//     for (; it != line.end(); it++) {
//         std::cout << *it << std::endl;
//     }
// }

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
