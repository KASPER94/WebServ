/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 16:38:26 by peanut            #+#    #+#             */
/*   Updated: 2024/11/04 13:39:47 by skapersk         ###   ########.fr       */
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

void Webserv::getRequest(int clientSock) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
	Client &client = this->_clients[clientSock];

	(void)client;

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

    std::string response;
    if (httpRequest.getMethod() == GET) {
        response = "HTTP/1.1 200 OK\r\n"
                   "Content-Type: text/plain\r\n"
                   "Content-Length: 13\r\n"
                   "\r\n"
                   "Hello, World!";
    } else {
        response = "HTTP/1.1 405 Method Not Allowed\r\n"
                   "Content-Length: 0\r\n"
                   "\r\n";
    }
    send(clientSock, response.c_str(), response.size(), 0);
    std::cout << "Réponse envoyée au client." << std::endl;
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
        close(clientSock);
    } else {
        std::cout << "Réponse envoyée au client." << std::endl;
    
    }
}

void Webserv::initializeSockets() {
    std::vector<Server> servers = this->getAllServer();
    int epollFd = epoll_create1(0);
    if (epollFd == -1) {
        std::cerr << "Erreur lors de la création de l'instance epoll" << std::endl;
        return;
    }

    // Loop through each server and set up its socket in epoll
    for (size_t i = 0; i < servers.size(); i++) {
        if (servers[i].connectToNetwork() < 0) {
            std::cerr << "Échec de la connexion pour le serveur " << i << std::endl;
            continue;
        }

        struct epoll_event event;
        event.data.fd = servers[i].getSock();  // Track each server's socket
        event.events = EPOLLIN; // Register for EPOLLIN to accept connections
        if (epoll_ctl(epollFd, EPOLL_CTL_ADD, servers[i].getSock(), &event) == -1) {
            std::cerr << "Erreur lors de l'ajout du socket serveur à epoll" << std::endl;
            close(servers[i].getSock());
            continue;
        }
    }

    // Main loop to handle epoll events
    while (true) {
        struct epoll_event events[MAX_EVENTS];
        int eventCount = epoll_wait(epollFd, events, MAX_EVENTS, -1); // Wait for events
        if (eventCount == -1) {
            std::cerr << "Erreur lors de l'appel à epoll_wait" << std::endl;
            break;
        }

        for (int i = 0; i < eventCount; i++) {
            if (events[i].events & EPOLLIN) {
                // New incoming connection on a server socket
                int serverSock = events[i].data.fd;
                int clientSock = accept(serverSock, NULL, NULL);  // Accept client on server socket
                if (clientSock == -1) {
                    std::cerr << "Erreur lors de l'acceptation de la connexion" << std::endl;
                    continue;
                }
                std::cout << "Nouvelle connexion acceptée sur le socket " << clientSock << " via le port serveur " << serverSock << std::endl;
                
				// Create a new Client instance and set its file descriptor
                Client client(clientSock);
                getRequest(client.getFd());  // Handles reading and processing the request
                setsocknonblock(client.getFd());  // Make client socket non-blocking

                struct epoll_event clientEvent;
                clientEvent.data.fd = client.getFd();
                clientEvent.events = EPOLLOUT;  // Monitor client socket for EPOLLOUT
                if (epoll_ctl(epollFd, EPOLL_CTL_ADD, client.getFd(), &clientEvent) == -1) {
                    std::cerr << "Erreur lors de l'ajout du socket client à epoll" << std::endl;
                    close(client.getFd());
                }
            } else if (events[i].events & EPOLLOUT) {
                // Ready to send a response
                sendResponse(events[i].data.fd);  // Sends response on client socket

                // Close client socket after sending response
                close(events[i].data.fd);
                epoll_ctl(epollFd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
            }
        }
    }
    close(epollFd); // Close the epoll instance after exiting loop
}

