/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 12:01:56 by peanut            #+#    #+#             */
/*   Updated: 2024/10/20 15:42:58 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/socket.h>
#include <arpa/inet.h>

#include <iostream>
#include <cstring>
#include <unistd.h>

int main() {
    // 1. Créer un socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        std::cerr << "Erreur: Échec de la création du socket" << std::endl;
        return -1;
    }

    // 2. Définir l'adresse du socket (adresse IP et port)
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    address.sin_family = AF_INET;  // Utilisation d'IPv4
    address.sin_addr.s_addr = INADDR_ANY;  // Écoute sur toutes les interfaces
    address.sin_port = htons(8080);  // Écoute sur le port 8080

    // 3. Associer le socket à l'adresse IP et au port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Erreur: Échec de la liaison" << std::endl;
        return -1;
    }

    // 4. Démarrer l'écoute des connexions
    if (listen(server_fd, 10) < 0) {
        std::cerr << "Erreur: Échec de l'écoute" << std::endl;
        return -1;
    }

    std::cout << "Serveur en écoute sur le port 8080..." << std::endl;

    // 5. Accepter une connexion d'un client
    int client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    if (client_fd < 0) {
        std::cerr << "Erreur: Échec de l'acceptation" << std::endl;
        return -1;
    }

    // Le serveur est prêt à gérer une requête
    std::cout << "Client connecté !" << std::endl;
  // 6. Recevoir les données envoyées par le client
    char buffer[1024] = {0};  // Tampon pour stocker les données
    int bytes_read = recv(client_fd, buffer, 1024, 0);  // Lecture de la requête
    if (bytes_read < 0) {
        std::cerr << "Erreur: Échec de la réception de la requête" << std::endl;
        return -1;
    }

    // Afficher la requête reçue
    std::cout << "Requête reçue : " << buffer << std::endl;

    // Vérifier si la requête est une requête GET
    std::string request(buffer);
    if (request.find("GET") == 0) {
        // Le client a fait une requête GET
        std::cout << "Requête GET détectée !" << std::endl;
    }

            // 7. Créer une réponse HTTP
        std::string http_response =
            "HTTP/1.1 200 OK\r\n"  // Ligne de statut HTTP
            "Content-Type: text/html\r\n"  // Type de contenu (HTML)
            "\r\n"  // Fin des en-têtes
            "<html><body><h1>Welcome to My Mother Fucking Webserv</h1></body></html>";  // Corps de la réponse

        // 8. Envoyer la réponse au client
        send(client_fd, http_response.c_str(), http_response.size(), 0);

        std::cout << "Réponse envoyée au client." << std::endl;
    // 9. Fermer les connexions
    close(client_fd);  // Ferme la connexion avec le client
    close(server_fd);  // Ferme le socket du serveur

    return 0;
}
