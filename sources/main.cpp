/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 19:30:34 by peanut            #+#    #+#             */
/*   Updated: 2024/10/25 11:52:02 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "conf.hpp"
#include "webserv.h"
#include <iostream>

bool webserv(char *config_file) {
	std::vector<Server>::iterator	it;

    try {
        conf config(config_file);
		it = (env()->webserv->getAllServer()).begin();
		for (; it != (env()->webserv->getAllServer()).end(); it++){
			std::cout << it->getClientMaxBody() << std::endl;
			// std::map<int, std::string> errorPages = it->getErrorPage();

			// // Utiliser l'itérateur pour parcourir les erreurs
			// for (std::map<int, std::string>::iterator it2 = errorPages.begin(); it2 != errorPages.end(); ++it2) {
			// 	std::cout << it2->first << std::endl;
			// }
		}
    }
    catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
    return (true);
}

int main(int ac, char **av) {
    if (ac == 1) {
        webserv((char *)"./config/default.conf");
    }
    else if (ac == 2) {
        webserv(av[1]);
    }
    else {
        std::cerr << "\033[31m" << "to use Webserv : ./webserv [configuration file]" << "\033[0m" << std::endl;
        return (1);
    }
    return (0);
}
