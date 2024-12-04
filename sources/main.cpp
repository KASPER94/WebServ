/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrigny <yrigny@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 19:30:34 by peanut            #+#    #+#             */
<<<<<<< HEAD
/*   Updated: 2024/12/04 16:04:15 by skapersk         ###   ########.fr       */
=======
/*   Updated: 2024/12/04 16:52:16 by yrigny           ###   ########.fr       */
>>>>>>> 1827865 (adding logMsg() function)
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

    try {
        conf config(config_file);
<<<<<<< HEAD
		std::vector<Server> &servers = env()->webserv->getAllServer();
=======
		env()->webserv->initializeSockets();
		// std::cout << "test" << std::endl;
		// it = (env()->webserv->getAllServer()).begin();
		// for (; it != (env()->webserv->getAllServer()).end(); it++){
		// 	methods = it->getAllowedMethods();
		// 	it2 = methods->begin();
		// 	for (; it2 != methods->end(); it2++){
		// 		std::cout << *it2 << std::endl;
		// 	}
			// std::map<int, std::string> errorPages = it->getErrorPage();
>>>>>>> 1827865 (adding logMsg() function)

		// Vérification des conflits de ports
		std::map<int, int> portCounts;
		for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it) {
			int port = it->getPort();
			if (portCounts.find(port) == portCounts.end()) {
				portCounts[port] = 1;
			} else {
				portCounts[port]++;
			}
		}
		for (std::map<int, int>::iterator it = portCounts.begin(); it != portCounts.end(); ++it) {
			if (it->second > 1) {
				std::cerr << "[DEBUG] Conflict detected: "
							<< it->second << " servers are configured to listen on port "
							<< it->first << "." << std::endl;
				throw std::runtime_error("Port conflict error. Check your server configuration.");
			}
		}
		env()->webserv->initializeSockets();
	}
    catch (std::exception &e)
	{
		// std::cerr << e.what() << std::endl;
		logMsg(ERROR, e.what());
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
        std::cout << "\033[31m" << "Usage: ./webserv [configuration file]" << "\033[0m" << std::endl;
        return (1);
    }
    return (0);
}
