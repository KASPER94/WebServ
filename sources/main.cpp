/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 19:30:34 by peanut            #+#    #+#             */
/*   Updated: 2024/11/26 13:16:48 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "conf.hpp"
#include "webserv.h"
#include <iostream>
#include <csignal>

bool	run = true;

void	signalHandler(int signum) {
	if (signum == SIGINT) {
		(void)signum;
		std::cout << "\nReceived SIGINT" << std::endl;
		run = false;
	}
}

bool webserv(char *config_file) {
	// std::vector<Server>::iterator	it;
	// std::vector<std::string>::iterator	it2;
	// std::vector<std::string> *methods;

    try {
        conf config(config_file);
		env()->webserv->initializeSockets();
		// it = (env()->webserv->getAllServer()).begin();
		// for (; it != (env()->webserv->getAllServer()).end(); it++){
		// 	methods = it->getAllowedMethods();
		// 	it2 = methods->begin();
		// 	for (; it2 != methods->end(); it2++){
		// 		std::cout << *it2 << std::endl;
		// 	}
			// std::map<int, std::string> errorPages = it->getErrorPage();

			// // Utiliser l'itérateur pour parcourir les erreurs
			// for (std::map<int, std::string>::iterator it2 = errorPages.begin(); it2 != errorPages.end(); ++it2) {
			// 	std::cout << it2->first << std::endl;
			// }
	}
    catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
    return (true);
}

int main(int ac, char **av) {
	signal(SIGINT, signalHandler);
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
