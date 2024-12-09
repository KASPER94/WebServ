/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 19:30:34 by peanut            #+#    #+#             */
/*   Updated: 2024/12/09 15:25:08 by skapersk         ###   ########.fr       */
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
		std::cout << std::endl;
		logMsg(INFO, "Received SIGINT");
		run = false;
        if (env()->webserv) {
            logMsg(INFO, "Cleaning up clients...");

            std::map<int, Client*>::iterator it = env()->webserv->getClients().begin();
            while (it != env()->webserv->getClients().end()) {
                env()->webserv->deleteClient(it->first); 
                it = env()->webserv->getClients().begin();
            }
            logMsg(INFO, "All clients cleaned up.");
        }
		// freeEnv(env()->envp);
	}
}

bool webserv(char *config_file, char **envi) {
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
				std::string msg = "Conflict detected: " + toString(it->second) + " servers are configured to listen on port " + toString(it->first) + ".";
				logMsg(DEBUG, msg);
				throw std::runtime_error("Port conflict error. Check your server configuration.");
			}
		}
		env()->envp = envi;
		env()->webserv->initializeSockets();
	}
    catch (std::exception &e)
	{
		logMsg(ERROR, e.what());
	}
    return (true);
}

int main(int ac, char **av, char **envp) {
	signal(SIGINT, signalHandler);
    if (ac == 1) {
        webserv((char *)"./config/default.conf", envp);
    }
    else if (ac == 2) {
        webserv(av[1], envp);
    }
    else {
        std::cout << "\033[31m" << "Usage: ./webserv [configuration file]" << "\033[0m" << std::endl;
        return (1);
    }
    return (0);
}
