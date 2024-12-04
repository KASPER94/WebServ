/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 19:30:34 by peanut            #+#    #+#             */
/*   Updated: 2024/12/04 16:04:15 by skapersk         ###   ########.fr       */
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
		std::vector<Server> &servers = env()->webserv->getAllServer();

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
