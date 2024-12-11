/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 16:38:26 by peanut            #+#    #+#             */
/*   Updated: 2024/12/09 15:49:19 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Webserv.hpp"

extern bool	run;

Webserv::Webserv() {}

Webserv::Webserv(std::vector<Server> const &servers): _servers(servers) {

}

Webserv::~Webserv() {

}

std::vector<Server> &Webserv::getAllServer() {
	return (this->_servers);
}

void Webserv::deleteClient(int fd) {
    if (_clients.find(fd) != _clients.end()) {
		Client *client = _clients[fd];
		if (client->getRequest()) {
            delete client->getRequest();
			client->setRequest(NULL); 
        }
        if (client->getResponse()) {
            delete client->getResponse();
			client->setResponse(NULL); 
        }
        if (epoll_ctl(this->_epollfd, EPOLL_CTL_DEL, fd, NULL) == -1) {
			logMsg(DEBUG, "Failed to delete socket " + toString(fd) + " from epoll: " + std::string(strerror(errno)));
        } else {
			logMsg(DEBUG, "Socket " + toString(fd) + " successfully deleted from epoll");
        }
        close(fd);
        delete _clients[fd];
        _clients.erase(fd);
		logMsg(DEBUG, "Client using socket " + toString(fd) + " successfully destroyed");
    }
}

void Webserv::getRequest(int clientSock) {
    char buffer[BUFFER_SIZE + 1];  // +1 pour l'ajout de '\0' à la fin
    int bytesRead;

    // Vérifier que le client existe dans le map _clients
    if (_clients.find(clientSock) == _clients.end()) {
		logMsg(ERROR, "Client using socket " + toString(clientSock) + " not found");
        return;
    }
    Client *client = this->_clients[clientSock];
    // Lire les données de la socket client
    bytesRead = recv(clientSock, buffer, BUFFER_SIZE, 0);
    if (bytesRead <= 0) {
        if (bytesRead == 0) {
			logMsg(INFO, "Client using sock " + toString(clientSock) + " closed the connection");
        } else {
			logMsg(ERROR, "Failed to read data from socket " + toString(clientSock));
        }
        this->deleteClient(clientSock);
        return;
    }

    buffer[bytesRead] = '\0';  // Terminer la chaîne de caractères
	// logMsg(DEBUG, "Fragment of request received on socket " + toString(clientSock));
    // std::cout << buffer << std::endl;
	bool isRequestComplete = client->appendRequest(buffer, bytesRead);
	    // Si la requête est chunked et incomplète, continuer à écouter le socket
    if (!isRequestComplete) {
		// logMsg(DEBUG, "Waiting for more fragments of request on socket " + toString(clientSock));
        struct epoll_event clientEvent;
        clientEvent.data.fd = clientSock;
        clientEvent.events = EPOLLIN | EPOLLET;
        if (epoll_ctl(_epollfd, EPOLL_CTL_MOD, clientSock, &clientEvent) == -1) {
			logMsg(ERROR, "Failed to modify socket " + toString(clientSock) + " in epoll: " + std::string(strerror(errno)));
        }
        return;
    }
    // Accumuler le fragment dans la requête du client
    if (client->getRequest()->getEnd()) {
		// client.getRequest().parseHttpRequest();
        logMsg(DEBUG, "Request received from socket " + toString(clientSock) + " is ready to be proceeded");

        // Préparer le client pour l'envoi de la réponse
        struct epoll_event clientEvent;
        clientEvent.data.fd = clientSock;
        clientEvent.events = EPOLLOUT;
        epoll_ctl(_epollfd, EPOLL_CTL_MOD, clientSock, &clientEvent);
    } else if (client->getError()) {
        // Supprimer le client en cas d'erreur de réception ou de parsing
		logMsg(DEBUG, "Request received from socket " + toString(clientSock) + " is invalid");
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
		logMsg(ERROR, "Client using socket " + toString(clientSock) + " not found");
		return;
	}

	Client *client = _clients[clientSock];
	if (!client->getRequestPtr()) {
		logMsg(ERROR, "Client using socket " + toString(clientSock) + " has no request ready to be proceeded");
		return;
	}
	client->sendResponse();
	std::string response = client->getResponsestr();
	size_t totalBytesSent = 0;
    size_t responseSize = response.size();
	logMsg(DEBUG, "Response to be sent to client socket " + toString(clientSock));
	// std::cout << response << std::endl;

	if (client->getError()) {
		deleteClient(clientSock);
		return;
	}

    while (totalBytesSent < responseSize) {
        int bytesSent = send(clientSock, response.c_str() + totalBytesSent, responseSize - totalBytesSent, 0);
		if (bytesSent <= 0) {
			deleteClient(clientSock);
			return;
		}
        totalBytesSent += bytesSent;
    }
	logMsg(DEBUG, "Response (" + toString(totalBytesSent) + " bytes) successfully sent to client");
	if (client->getRequest()->keepAlive()) {
        // Réinitialiser pour la prochaine requête
        client->resetForNextRequest();

        // Remettre le socket en mode lecture
        struct epoll_event clientEvent;
        clientEvent.data.fd = clientSock;
        clientEvent.events = EPOLLIN | EPOLLET; // Lire les nouvelles requêtes
        epoll_ctl(_epollfd, EPOLL_CTL_MOD, clientSock, &clientEvent);
    } else {
        // Fermer la connexion si `Connection: close`
        this->deleteClient(clientSock);
		return ;
    }

    // Fermer le socket après avoir envoyé la réponse

    // close(clientSock);
    // if (epoll_ctl(this->_epollfd, EPOLL_CTL_DEL, clientSock, NULL) == -1) {
    // std::cerr << "[DEBUG] Erreur lors de la suppression du socket " << clientSock << " de epoll : " << strerror(errno) << std::endl;
	// }
    // _clients.erase(clientSock);



	// std::string response = client->getResponsestr();
	// int ret = send(clientSock, response.c_str(), response.size(), 0);
	// if (ret < 0) {
	// 	std::cerr << "Erreur lors de l'envoi de la réponse" << std::endl;
	// 	close(clientSock);
	// 	_clients.erase(clientSock);
	// } else {
	// 	std::cout << "Réponse envoyée au client." << std::endl;
	// }
}

void Webserv::checkClientTimeouts() {
    std::map<int, Client *>::iterator it = _clients.begin();
    while (it != _clients.end()) {
        Client *client = it->second;
        if (client->isTimeout()) {
            logMsg(INFO, "Client using socket " + toString(client->getFd()) + " timed out due to inactivity");
            int clientFd = client->getFd();
            deleteClient(clientFd);
            it = _clients.begin(); // Recommencer l'itération car le map a été modifié
        } else {
            ++it;
        }
    }
}

void Webserv::initializeSockets() {
	int serverSock;
    std::vector<Server> &servers = this->getAllServer();
    _epollfd = epoll_create1(0);
    if (_epollfd == -1) {
		logMsg(ERROR, "epoll_create() failed to create epoll instance");
        return;
    }

    // Register each server's socket with epoll
    for (size_t i = 0; i < servers.size(); i++) {
        if (servers[i].connectToNetwork() < 0) {
			logMsg(ERROR, "Failed to connect server " + toString(i) + " to network");
    		for (size_t i = 0; i < servers.size(); i++)
				servers[i].cleanUp();
   			close(_epollfd);
			return ;
        }
        serverSock = servers[i].getSock();
        _serverSockets[serverSock] = &servers[i];

        struct epoll_event event;
        event.data.fd = serverSock;
        event.events = EPOLLIN;  // Register for incoming connections
        if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, serverSock, &event) == -1) {
			logMsg(ERROR, "epoll_ctl() failed to add server socket to epoll");
            close(serverSock);
            continue;
        }
    }

    // Main loop to handle events
    while (run == true) {
        struct epoll_event events[MAX_EVENTS];
        int eventCount = epoll_wait(_epollfd, events, MAX_EVENTS, EPOLL_TIMEOUT);
        if (eventCount == -1 && errno != EINTR) {
            logMsg(ERROR, "epoll_wait() failed to fetch events");
            break;
        }

        for (int i = 0; i < eventCount; i++) {
            int sock = events[i].data.fd;

            if (isServerSocket(sock) && (events[i].events & EPOLLIN)) {
                // Accept a new client connection on a server socket
				int clientSock = accept(sock, NULL, NULL);
				if (clientSock == -1) {
					if (errno == EAGAIN || errno == EWOULDBLOCK) {
						logMsg(DEBUG, "No pending client request on server socket " + toString(sock));
					} else {
						logMsg(DEBUG, "accept() failed: " + std::string(strerror(errno)));
					}
					continue;
				}
				logMsg(INFO, "New client connection using socket " + toString(clientSock));

				// Set non-blocking mode for the client socket
				if (setsocknonblock(clientSock) < 0) {
   					 close(clientSock);
   					 continue;
				}
                // Create a new Client instance and add to _clients
				Server *Server =_serverSockets[sock];
				if (Server) {
					Client *new_client = new Client(clientSock, Server);
					_clients[clientSock] = new_client;

					struct epoll_event clientEvent;
					clientEvent.data.fd = clientSock;
					clientEvent.events = EPOLLIN | EPOLLET; // Lecture en mode non-bloquant
					if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, clientSock, &clientEvent) == -1) {
						logMsg(ERROR, "epoll_ctl() failed to add client socket to epoll");
						close(clientSock);
						_clients.erase(clientSock);
					}
				} else {
					// Handle the case where the server pointer is null
					logMsg(ERROR, "Server socket " + toString(sock) + " not found");
					close(clientSock);
				}
				// Server *Server =_serverSockets[sock];
                // Client client(clientSock, Server);
                // _clients[clientSock] = client;

                // Register the client socket for EPOLLIN events to handle incoming requests
                // struct epoll_event clientEvent;
                // clientEvent.data.fd = clientSock;
                // clientEvent.events = EPOLLIN;
                // if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, clientSock, &clientEvent) == -1) {
                //     std::cerr << "Erreur lors de l'ajout du socket client à epoll" << std::endl;
                //     close(clientSock);
                //     _clients.erase(clientSock);
                // }
            } else if (_clients.find(sock) != _clients.end() && (events[i].events & EPOLLIN)) {
                // Handle incoming data for an existing client
                getRequest(sock);
            } else if (_clients.find(sock) != _clients.end() && (events[i].events & EPOLLOUT)) {
                // Send response to client
                sendResponse(sock);

                // Remove the client after sending response
				//     close(sock);
				// 	if (epoll_ctl(this->_epollfd, EPOLL_CTL_DEL, sock, NULL) == -1) {
				// 		std::cerr << "[DEBUG] Erreur lors de la suppression du socket " << sock << " de epoll : " << strerror(errno) << std::endl;
				//     _clients.erase(sock);
				// } else {
				//     std::cerr << "Socket " << sock << " introuvable ou événement inattendu" << std::endl;
            }
        }
		checkClientTimeouts();
    }
    close(_epollfd);
    delete env()->webserv;
}


bool Webserv::isServerSocket(int sock) const {
	for (size_t i = 0; i < _servers.size(); ++i) {
		if (_servers[i].getSock() == sock) {
			return true;
		}
	}
	return false;
}
