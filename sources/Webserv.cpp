/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 16:38:26 by peanut            #+#    #+#             */
/*   Updated: 2024/11/04 16:18:32 by skapersk         ###   ########.fr       */
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

void Webserv::getRequest(int clientSock) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
	if (_clients.find(clientSock) == _clients.end()) {
        std::cerr << "Client introuvable pour le socket " << clientSock << std::endl;
        return;
    }
	Client &client = this->_clients[clientSock];

    int bytesRead = recv(clientSock, buffer, BUFFER_SIZE - 1, 0);

    if (bytesRead < 0) {
        std::cerr << "Erreur lors de la lecture des données du socket " << clientSock << std::endl;
        return;
    } else if (bytesRead == 0) {
        std::cout << "Le client " << clientSock << " a fermé la connexion." << std::endl;
        close(clientSock);
        return;
    }

    buffer[bytesRead] = '\0'; // Attention ! Guillaume conseil de faire attention au /0 car trop simple pour verifier que la chaîne est terminée
    std::cout << "Requête reçue sur le socket " << clientSock << ": " << buffer << std::endl;

    // traiter la requête, parser l'HTTP, etc.
	std::string request(buffer);
    HttpRequest httpRequest = HttpRequest(request);
	client.setRequest(httpRequest);
	if (!request.empty()) {
        client.setRequest(httpRequest);
        std::cout << "Requête stockée pour le client " << clientSock << std::endl;
    } else {
        std::cerr << "La requête pour le client " << clientSock << " est vide" << std::endl;
        close(clientSock);
        _clients.erase(clientSock);  // Clean up if the request is invalid
        return;
    }

	struct epoll_event clientEvent;
    clientEvent.data.fd = clientSock;
    clientEvent.events = EPOLLOUT;
    epoll_ctl(_epollfd, EPOLL_CTL_MOD, clientSock, &clientEvent);

    // std::string response;
    // if (httpRequest.getMethod() == GET) {
    //     response = "HTTP/1.1 200 OK\r\n"
    //                "Content-Type: text/plain\r\n"
    //                "Content-Length: 13\r\n"
    //                "\r\n"
    //                "Hello, World!";
    // } else {
    //     response = "HTTP/1.1 405 Method Not Allowed\r\n"
    //                "Content-Length: 0\r\n"
    //                "\r\n";
    // }
    // send(clientSock, response.c_str(), response.size(), 0);
    // std::cout << "Réponse envoyée au client." << std::endl;
}


void Webserv::sendResponse(int clientSock) {
    if (_clients.find(clientSock) == _clients.end()) {
        std::cerr << "Client introuvable pour le socket " << clientSock << std::endl;
        return;
    }

    Client &client = _clients[clientSock];
	if (!client.getRequestPtr()) {
        std::cerr << "Aucune requête pour le client " << clientSock << std::endl;
        return;
    }
    HttpRequest &httpRequest = client.getRequest();
    std::string response;

    if (httpRequest.getMethod() == GET) {
        response = "HTTP/1.1 200 OK\r\n"
                   "Content-Type: text/plain\r\n"
                   "Content-Length: 13\r\n"
                   "\r\n"
                   "COUCOU WEBSERV !!!";
    } else {
        response = "HTTP/1.1 405 Method Not Allowed\r\n"
                   "Content-Length: 0\r\n"
                   "\r\n";
    }

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

        int serverSock = servers[i].getSock();
        _serverSockets[serverSock] = servers[i];

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
        int eventCount = epoll_wait(_epollfd, events, MAX_EVENTS, -1);
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
                Client client(clientSock);
                _clients[clientSock] = client;

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



