/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 16:38:26 by peanut            #+#    #+#             */
/*   Updated: 2024/10/30 16:23:34 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Webserv.hpp"

Webserv::Webserv() {}

Webserv::Webserv(std::vector<Server> const &servers): _servers(servers) {

}

Webserv::~Webserv() {

}

std::vector<Server> Webserv::getAllServer() {
	return (this->_servers);
}

void Webserv::initializeSockets() {
	std::vector<Server> servers = this->getAllServer();
	std::vector<struct pollfd> pollFds(servers.size());

	for (size_t i = 0; i < servers.size() - 1; i++) {
		if (servers[i].connectToNetwork() < 0) {
			std::cerr << "Échec de la connexion pour le serveur " << i << std::endl;
			continue ;
		}

		pollFds[i].fd = servers[i].getSock();
		pollFds[i].events = POLLIN;
	}

	while (1) {
		int ret = poll(pollFds.data(), pollFds.size(), -1);

		if (ret < 0) {
			std::cerr << "Erreur lors de l'appel à poll" << std::endl;
			break ;
		}
		for (size_t i = 0; i < pollFds.size(); i++) {
			if (pollFds[i].revents & POLLIN) {
				std::cout << "Prêt à accepter sur le port " << servers[i].getPort() << std::endl;
				int clientSock = accept(pollFds[i].fd, NULL, NULL);
				if (clientSock < 0) {
					std::cerr << "Erreur lors de l'acceptation de la connexion sur le port " << servers[i].getPort() << std::endl;
					continue;
				}
				std::cout << "Nouvelle connexion acceptée sur le socket " << clientSock << std::endl;
			}
		}
	}
}
