/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 16:38:26 by peanut            #+#    #+#             */
/*   Updated: 2024/10/31 11:34:46 by skapersk         ###   ########.fr       */
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

// void Webserv::sendResponse(int clientSock) {
// 	std::cout << "couc";
//     char buffer[1024] = {0};
//     int bytes_read = recv(clientSock, buffer, sizeof(buffer), 0);

//     if (bytes_read < 0) {
//         std::cerr << "Erreur: Échec de la réception de la requête" << std::endl;
//         return;
//     }

//     std::cout << "Requête reçue : " << buffer << std::endl;

//     std::string request(buffer);
//     std::cout << "\033[31m" << request << "\033[0m" << std::endl;

//     if (request.find("GET") == 0) {
//         std::cout << "Requête GET détectée !" << std::endl;
//     }

//     // Créer la réponse HTTP
//     std::string http_response =
//         "HTTP/1.1 200 OK\r\n"
//         "Content-Type: text/html\r\n"
//         "\r\n"
//         "<html><body><h1>Welcome to My Mother Fucking Webserv</h1></body></html>";

//     // Envoyer la réponse
//     send(clientSock, http_response.c_str(), http_response.size(), 0);
//     std::cout << "Réponse envoyée au client." << std::endl;
// }


// void Webserv::initializeSockets() {
// 	std::vector<Server> servers = this->getAllServer();
// 	std::vector<struct pollfd> pollFds(servers.size());

// 	for (size_t i = 0; i < servers.size() - 1; i++) {
// 		if (servers[i].connectToNetwork() < 0) {
// 			std::cerr << "Échec de la connexion pour le serveur " << i << std::endl;
// 			continue ;
// 		}

// 		pollFds[i].fd = servers[i].getSock();
// 		pollFds[i].events = POLLIN;
// 	}

// 	while (1) {
// 		int ret = poll(pollFds.data(), pollFds.size(), -1);

// 		if (ret < 0) {
// 			std::cerr << "Erreur lors de l'appel à poll" << std::endl;
// 			break ;
// 		}
// 		for (size_t i = 0; i < pollFds.size(); i++) {
// 			if (pollFds[i].revents & POLLIN) {
// 				std::cout << "Prêt à accepter sur le port " << servers[i].getPort() << std::endl;
// 				int clientSock = accept(pollFds[i].fd, NULL, NULL);
// 				if (clientSock < 0) {
// 					std::cerr << "Erreur lors de l'acceptation de la connexion sur le port " << servers[i].getPort() << std::endl;
// 					continue;
// 				}
// 				std::cout << "Nouvelle connexion acceptée sur le socket " << clientSock << std::endl;
// 			}
// 			else if (pollFds[i].revents & POLLOUT) // send response to the client
// 				sendResponse(pollFds[i].fd);
// 		}
// 	}
// }

void Webserv::sendResponse(int clientSock) {
	std::string http_response =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: 58\r\n"  // Spécifier la longueur pour éviter des erreurs
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

// void Webserv::initializeSockets() {
	// std::vector<Server> servers = this->getAllServer();
	// std::vector<struct pollfd> pollFds;

	// // Initialisation des sockets des serveurs
	// for (size_t i = 0; i < servers.size() - 1; i++) {
	// 	if (servers[i].connectToNetwork() < 0) {
	// 		std::cerr << "Échec de la connexion pour le serveur " << i << std::endl;
	// 		continue;
	// 	}

	// 	struct pollfd serverPollFd;
	// 	serverPollFd.fd = servers[i].getSock();
	// 	serverPollFd.events = POLLIN;
	// 	pollFds.push_back(serverPollFd);
	// }

	// // Boucle pour gérer les connexions et les réponses
	// while (1) {
	// 	int ret = poll(pollFds.data(), pollFds.size(), -1);
	// 	if (ret < 0) {
	// 		std::cerr << "Erreur lors de l'appel à poll" << std::endl;
	// 		break;
	// 	}

	// 	for (size_t i = 0; i < pollFds.size(); i++) {
	// 		if (pollFds[i].revents & POLLIN) {
	// 			// Si l'événement est prêt pour accepter une nouvelle connexion
	// 			int clientSock = accept(pollFds[i].fd, NULL, NULL);
	// 			if (clientSock < 0) {
	// 				std::cerr << "Erreur lors de l'acceptation de la connexion" << std::endl;
	// 				continue;
	// 			}
	// 			std::cout << "Nouvelle connexion acceptée sur le socket " << clientSock << std::endl;

	// 			// Ajout du client pour le suivi avec POLLOUT pour envoyer une réponse
	// 			struct pollfd clientPollFd;
	// 			clientPollFd.fd = clientSock;
	// 			clientPollFd.events = POLLOUT;
	// 			pollFds.push_back(clientPollFd);
	// 		}
	// 		else if (pollFds[i].revents & POLLOUT) {
	// 			// Envoi de la réponse lorsque le socket client est prêt
	// 			sendResponse(pollFds[i].fd);

	// 			// Fermeture et suppression du client de pollFds
	// 			close(pollFds[i].fd);
	// 			pollFds.erase(pollFds.begin() + i);
	// 			i--; // Ajuste l'indice après la suppression
	// 		}
	// 	}
	// }
// }


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
		struct epoll_event events[10]; // Taille maximale des événements
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
