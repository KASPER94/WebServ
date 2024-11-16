/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 16:38:26 by peanut            #+#    #+#             */
/*   Updated: 2024/11/16 10:11:45 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Webserv.hpp"

Webserv::Webserv() {}

Webserv::Webserv(std::vector<Server> const &servers): _servers(servers) {

}

Webserv::~Webserv() {

}

std::vector<Server> &Webserv::getAllServer() {
	return (this->_servers);
}

void	Webserv::deleteClient(int fd) {
    if (_clients.find(fd) != _clients.end()) {
        epoll_ctl(this->_epollfd, EPOLL_CTL_DEL, fd, NULL);
        close(fd);
        delete _clients[fd];
        _clients.erase(fd);
    }
}

void Webserv::getRequest(int clientSock) {
    char buffer[BUFFER_SIZE + 1];  // +1 pour l'ajout de '\0' à la fin
    int bytesRead;

    // Vérifier que le client existe dans le map _clients
    if (_clients.find(clientSock) == _clients.end()) {
        std::cerr << "Client introuvable pour le socket " << clientSock << std::endl;
        return;
    }
    Client *client = this->_clients[clientSock];
    // Lire les données de la socket client
    bytesRead = recv(clientSock, buffer, BUFFER_SIZE, 0);
    if (bytesRead <= 0) {
        if (bytesRead == 0) {
            std::cout << "Le client " << clientSock << " a fermé la connexion." << std::endl;
        } else {
            std::cerr << "Erreur lors de la lecture des données du socket " << clientSock << std::endl;
        }
        this->deleteClient(clientSock);
        return;
    }

    buffer[bytesRead] = '\0';  // Terminer la chaîne de caractères
    std::cout << "Fragment de requête reçu sur le socket " << clientSock << ": " << buffer << std::endl;
	bool isRequestComplete = client->appendRequest(buffer, bytesRead);
	    // Si la requête est chunked et incomplète, continuer à écouter le socket
    if (!isRequestComplete) {
        std::cout << "Attente de fragments supplémentaires pour le client " << clientSock << std::endl;
        return;
    }
    // Accumuler le fragment dans la requête du client
    if (client->getRequest()->getEnd()) {
		// client.getRequest().parseHttpRequest();
        std::cout << "Requête stockée et prête pour le traitement pour le client " << clientSock << std::endl;

        // Préparer le client pour l'envoi de la réponse
        struct epoll_event clientEvent;
        clientEvent.data.fd = clientSock;
        clientEvent.events = EPOLLOUT;
        epoll_ctl(_epollfd, EPOLL_CTL_MOD, clientSock, &clientEvent);
    } else if (client->getError()) {
        // Supprimer le client en cas d'erreur de réception ou de parsing
        std::cerr << "Erreur de requête détectée pour le client " << clientSock << std::endl;
        this->deleteClient(clientSock);
    }
}



// void Webserv::sendResponse(int clientSock) {
// 	if (_clients.find(clientSock) == _clients.end()) {
// 		std::cerr << "Client introuvable pour le socket " << clientSock << std::endl;
// 		return;
// 	}

// 	Client *client = _clients[clientSock];
// 	if (!client->getRequestPtr()) {
// 		std::cerr << "Aucune requête pour le client " << clientSock << std::endl;
// 		return;
// 	}
// 	HttpRequest &httpRequest = client->getRequest();
// 	std::string response;
// 	std::cout << "Méthode HTTP reçue : " << httpRequest.HttpMethodTostring() << std::endl;

// 	if (httpRequest.getMethod() == GET) {
// 		response = "HTTP/1.1 200 OK\r\n"
// 					"Content-Type: text/plain\r\n"
// 					"Content-Length: 13\r\n"
// 					"\r\n"
// 					"COUCOU WEBSERV !!!";
// 	} else {
// 		response = "HTTP/1.1 405 Method Not Allowed\r\n"
// 					"Content-Length: 0\r\n"
// 					"\r\n";
// 	}

// 	int ret = send(clientSock, response.c_str(), response.size(), 0);
// 	if (ret < 0) {
// 		std::cerr << "Erreur lors de l'envoi de la réponse" << std::endl;
// 		close(clientSock);
// 		_clients.erase(clientSock);
// 	} else {
// 		std::cout << "Réponse envoyée au client." << std::endl;
// 	}
// }

void Webserv::sendResponse(int clientSock) {
	if (_clients.find(clientSock) == _clients.end()) {
		std::cerr << "Client introuvable pour le socket " << clientSock << std::endl;
		return;
	}

	Client *client = _clients[clientSock];
	if (!client->getRequestPtr()) {
		std::cerr << "Aucune requête pour le client " << clientSock << std::endl;
		return;
	}
	client->sendResponse();
	if (client->getError()) {
		epoll_ctl(this->_epollfd, EPOLL_CTL_DEL, clientSock, NULL);
		close(clientSock);
		_clients.erase(clientSock);
		return ;
	}
	std::string response = client->getResponsestr();
	int ret = send(clientSock, response.c_str(), response.size(), 0);
	if (ret < 0) {
		std::cerr << "Erreur lors de l'envoi de la réponse" << std::endl;
		close(clientSock);
		_clients.erase(clientSock);
	} else {
		std::cout << "Réponse envoyée au client." << std::endl;
	}
}

void Webserv::initializeSockets() {
	int serverSock;
    std::vector<Server> &servers = this->getAllServer();
    _epollfd = epoll_create1(0);
    if (_epollfd == -1) {
        std::cerr << "Erreur lors de la création de l'instance epoll" << std::endl;
        return;
    }

    // Register each server's socket with epoll
    for (size_t i = 0; i < servers.size(); i++) {
        if (servers[i].connectToNetwork() < 0) {
            std::cerr << "Échec de la connexion pour le serveur " << i << std::endl;
            continue;
        }
		std::cout << "..... " << servers[i].getReturnUri().begin()->second << " ... end"<< std::endl;
        serverSock = servers[i].getSock();
        _serverSockets[serverSock] = &servers[i];

        struct epoll_event event;
        event.data.fd = serverSock;
        event.events = EPOLLIN;  // Register for incoming connections
        if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, serverSock, &event) == -1) {
            std::cerr << "Erreur lors de l'ajout du socket serveur à epoll" << std::endl;
            close(serverSock);
            continue;
        }
    }

    // Main loop to handle events
    while (true) {
        struct epoll_event events[MAX_EVENTS];
        int eventCount = epoll_wait(_epollfd, events, MAX_EVENTS, EPOLL_TIMEOUT);
        if (eventCount == -1) {
            std::cerr << "Erreur lors de l'appel à epoll_wait" << std::endl;
            break;
        }

        for (int i = 0; i < eventCount; i++) {
            int sock = events[i].data.fd;

            if (isServerSocket(sock) && (events[i].events & EPOLLIN)) {
                // Accept a new client connection on a server socket
                int clientSock = accept(sock, NULL, NULL);
                if (clientSock == -1) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        // No pending connections; not an error in non-blocking mode
                        continue;
                    } else {
                        std::cerr << "Erreur lors de l'acceptation de la connexion: " << strerror(errno) << std::endl;
                        continue;
                    }
                }

                // Create a new Client instance and add to _clients
				Server *Server =_serverSockets[sock];
				if (Server) {
					Client *new_client = new Client(clientSock, Server);
					_clients[clientSock] = new_client;
				} else {
					// Handle the case where the server pointer is null
					std::cerr << "Server not found for socket " << sock << std::endl;
					close(clientSock);
				}
				// Server *Server =_serverSockets[sock];
                // Client client(clientSock, Server);
                // _clients[clientSock] = client;

                // Set non-blocking mode for the client socket
                setsocknonblock(clientSock);

                // Register the client socket for EPOLLIN events to handle incoming requests
                struct epoll_event clientEvent;
                clientEvent.data.fd = clientSock;
                clientEvent.events = EPOLLIN;
                if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, clientSock, &clientEvent) == -1) {
                    std::cerr << "Erreur lors de l'ajout du socket client à epoll" << std::endl;
                    close(clientSock);
                    _clients.erase(clientSock);
                }
            } else if (_clients.find(sock) != _clients.end() && (events[i].events & EPOLLIN)) {
                // Handle incoming data for an existing client
                getRequest(sock);
            } else if (_clients.find(sock) != _clients.end() && (events[i].events & EPOLLOUT)) {
                // Send response to client
                sendResponse(sock);

                // Remove the client after sending response
                close(sock);
                epoll_ctl(_epollfd, EPOLL_CTL_DEL, sock, NULL);
                _clients.erase(sock);
            } else {
                std::cerr << "Socket " << sock << " introuvable ou événement inattendu" << std::endl;
            }
        }
    }
    close(_epollfd);
}


bool Webserv::isServerSocket(int sock) const {
	for (size_t i = 0; i < _servers.size(); ++i) {
		if (_servers[i].getSock() == sock) {
			return true;
		}
	}
	return false;
}



