/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/03 10:42:03 by skapersk          #+#    #+#             */
/*   Updated: 2024/11/13 12:52:48 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client():
	_request(NULL),
	_response(NULL)
	{}

// Client::Client(int fd):
// 	_request(NULL),
// 	_response(NULL),
// 	_fd(fd)
// 	{}

Client::Client(int fd, Server *servers):
	_request(NULL),
	_response(NULL),
	 _fd(fd),
	 _error(false),
	 _servers(servers) {
		    std::cout << "Client created at " << this << " for socket " << fd << std::endl;

}

Client::Client(const Client &cpy) {
	*this = cpy;
}

Client::~Client() {
	if (this->_request)
		delete this->_request;
	if (this->_response)
		delete this->_response;
	 std::cout << "Client destroyed at " << this << std::endl;
}

Client	&Client::operator=(const Client &rhs) {
	this->_request = rhs._request;
	this->_response = rhs._response;
	this->_fd = rhs._fd;
	return (*this);
}

HttpRequest	*Client::getRequest() const {
	return (this->_request);
}

HttpRequest	*Client::getResponse() const {
	return (this->_response);
}

int Client::getFd() const {
    return _fd;
}

void Client::setFd(int fd) {
    if (_fd >= 0)
		close(_fd);
    _fd = fd;
}

// void Client::setRequest(HttpRequest& request) {
// 	if (_request) {
//         delete _request;  // Delete the existing request to avoid memory leaks
//     }
//     _request = new HttpRequest(request);
// }

HttpRequest& Client::getRequest() {
    return *_request;
}

bool	Client::error() const {
	return (this->_error);
}

bool	Client::appendRequest(const char *data, int bytes) {
	if (!this->_request)
		this->_request = new HttpRequest(this);
	// this->_creationDate = time(0);
	return (this->_request->appendRequest(data, bytes));
}

Server	*Client::getServer() const {
	std::cout << this->_servers << std::endl;
	return (this->_servers);
}

// bool Client::appendRequest(const char* data, int length) {
// 	_headersParsed = false;
//     _requestData.append(data, length);  // Accumule les données reçues dans une std::string
//     // Vérifiez si la requête est complète
//     if (_requestData.find("\r\n\r\n") != std::string::npos) {  // Fin de l'entête
//         // Logique pour vérifier si le corps est complet si Content-Length est spécifié
//         if (hasCompleteBody()) {
//             return true;
//         }
//     }
//     return false;  // Retourne false si la requête n'est pas encore complète
// }

// std::string Client::getFullRequest() const {
//     return _requestData;  // Retourne les données accumulées
// }

// bool Client::hasCompleteBody() {
//     // Si les en-têtes n'ont pas encore été analysés, retournez false
//     if (_requestData.find("\r\n\r\n") == std::string::npos) {
//         return false;  // En-têtes incomplets
//     }

//     // Si les en-têtes sont présents mais pas encore analysés
//     if (!_headersParsed) {
//         parseHeaders();  // Une méthode qui extrait les en-têtes de _requestData
//         _headersParsed = true;  // Indiquer que les en-têtes ont été traités
//     }

//     // Cas 1 : Vérifier Content-Length
//     if (_headers.count("Content-Length") > 0) {
// 		size_t contentLength = std::strtol(_headers["Content-Length"].c_str(), 0, 10);

//         size_t headerEndPos = _requestData.find("\r\n\r\n") + 4;
//         size_t bodyLength = _requestData.size() - headerEndPos;
//         return bodyLength >= contentLength;
//     }

//     // Cas 2 : Vérifier Transfer-Encoding: chunked
//     if (_headers.count("Transfer-Encoding") > 0 && _headers["Transfer-Encoding"] == "chunked") {
//         return isChunkedBodyComplete();
//     }

//     // Si aucune taille de corps spécifiée, considérer comme complète
//     return true;
// }

// void Client::parseHeaders() {
//     std::istringstream stream(_requestData);
//     std::string line;

//     // Lire la première ligne (request line) et l'ignorer dans ce contexte
//     std::getline(stream, line);

//     // Lire les en-têtes jusqu'à la ligne vide
//     while (std::getline(stream, line) && line != "\r") {
//         size_t pos = line.find(": ");
//         if (pos != std::string::npos) {
//             std::string headerName = line.substr(0, pos);
//             std::string headerValue = line.substr(pos + 2);
//             _headers[headerName] = headerValue;
//         }
//     }
// }

// bool Client::isChunkedBodyComplete() {
//     size_t pos = _requestData.find("\r\n\r\n") + 4;  // Position du début du corps
//     while (pos < _requestData.size()) {
//         // Trouver la taille du chunk
//         size_t nextPos = _requestData.find("\r\n", pos);
//         if (nextPos == std::string::npos) return false;

//         // Lire la taille en hexadécimal
//         std::string chunkSizeStr = _requestData.substr(pos, nextPos - pos);
//         int chunkSize = std::strtol(chunkSizeStr.c_str(), 0, 16);

//         // Si la taille est zéro, le corps est terminé
//         if (chunkSize == 0) return true;

//         // Positionner au début du contenu du chunk
//         pos = nextPos + 2 + chunkSize;  // Skip \r\n après la taille et avancer de chunkSize

//         // Vérifier la fin du chunk
//         if (_requestData.substr(pos, 2) != "\r\n") return false;

//         pos += 2;  // Skip the \r\n after the chunk content
//     }
//     return false;  // Corps incomplet si on sort de la boucle sans retour
// }
