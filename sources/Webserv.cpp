/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 16:38:26 by peanut            #+#    #+#             */
/*   Updated: 2024/10/31 18:22:58 by skapersk         ###   ########.fr       */
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

void Webserv::sendResponse(int clientSock) {
	std::string http_response =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: 58\r\n"  // Spécifier la longueur pour éviter des erreurs
		"Connection: keep-alive\r\n" 
		"\r\n"
		"<html><body><h1>Welcome to My Mother Fucking Webserv</h1></body></html>";

    int ret = send(clientSock, http_response.c_str(), http_response.size(), 0);
    if (ret < 0) {
        std::cerr << "Erreur lors de l'envoi de la réponse" << std::endl;
        close(clientSock);  // Fermer en cas d'erreur d'envoi
    } else {
        std::cout << "Réponse envoyée au client." << std::endl;
        // Le socket reste ouvert ici pour de nouvelles requêtes éventuelles.
    }
}

void Webserv::initializeSockets() {
	std::vector<Server> servers = this->getAllServer();
	int epollFd = epoll_create1(0); // Crée l'instance epoll
	if (epollFd == -1) {
		std::cerr << "Erreur lors de la création de l'instance epoll" << std::endl;
		return;
	}

	// Initialisation des sockets des serveurs et ajout à epoll
	for (size_t i = 0; i < servers.size() - 1; i++) {
		if (servers[i].connectToNetwork() < 0) {
			std::cerr << "Échec de la connexion pour le serveur " << i << std::endl;
			continue;
		}

		struct epoll_event event;
		event.data.fd = servers[i].getSock();
		event.events = EPOLLIN; // Enregistre pour EPOLLIN pour accepter les connexions
		if (epoll_ctl(epollFd, EPOLL_CTL_ADD, servers[i].getSock(), &event) == -1) {
			std::cerr << "Erreur lors de l'ajout du socket serveur à epoll" << std::endl;
			close(servers[i].getSock());
			continue;
		}
	}

	// Boucle principale pour gérer les événements
	while (1) {
		struct epoll_event events[MAX_EVENTS]; // Taille maximale des événements
		int eventCount = epoll_wait(epollFd, events, 10, -1);
		if (eventCount == -1) {
			std::cerr << "Erreur lors de l'appel à epoll_wait" << std::endl;
			break;
		}

		for (int i = 0; i < eventCount; i++) {
			if (events[i].events & EPOLLIN) {
				// Nouvelle connexion entrante
				int clientSock = accept(events[i].data.fd, NULL, NULL);
				if (clientSock == -1) {
					std::cerr << "Erreur lors de l'acceptation de la connexion" << std::endl;
					continue;
				}
				std::cout << "Nouvelle connexion acceptée sur le socket " << clientSock << std::endl;
				setsocknonblock(clientSock);
				struct epoll_event clientEvent;
				clientEvent.data.fd = clientSock;
				clientEvent.events = EPOLLOUT; // Surveille le socket client pour EPOLLOUT
				if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientSock, &clientEvent) == -1) {
					std::cerr << "Erreur lors de l'ajout du socket client à epoll" << std::endl;
					close(clientSock);
				}
			}
			else if (events[i].events & EPOLLOUT) {
				// Prêt à envoyer une réponse
				sendResponse(events[i].data.fd);

				// Ferme le socket client et le retire de epoll
				close(events[i].data.fd);
				epoll_ctl(epollFd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
			}
		}
	}
	close(epollFd); // Ferme l'instance epoll
}
