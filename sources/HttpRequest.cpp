/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/02 19:24:38 by skapersk          #+#    #+#             */
/*   Updated: 2024/11/05 11:54:52 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

HttpRequest::HttpRequest() {}

// HttpRequest::HttpRequest(const std::string &req) {
//     // parseHttpRequest(req);
// }

HttpRequest::HttpRequest(Client *client): _client(client) {
    // parseHttpRequest(req);
}

HttpRequest::~HttpRequest() {}

enum HttpMethod HttpRequest::getMethod() const {
    return this->_method;
}

std::string HttpRequest::HttpMethodTostring() {
    if (_method == GET) return "GET";
    if (_method == POST) return "POST";
    if (_method == DELETE) return "DELETE";
    return "OTHER";
}

HttpMethod HttpRequest::stringToHttpMethod(const std::string &methodStr) {
    if (methodStr == "GET") return GET;
    if (methodStr == "POST") return POST;
    if (methodStr == "DELETE") return DELETE;
    return OTHER;
}

// void HttpRequest::parseHttpRequest(const std::string& request) {
//     std::istringstream stream(request);
//     std::string line;

//     std::getline(stream, line);
//     std::istringstream requestLine(line);
//     std::string methodStr;

//     requestLine >> methodStr >> this->_path >> this->_version;

//     this->_method = stringToHttpMethod(methodStr);

//     // Lire les en-têtes
//     while (std::getline(stream, line) && line != "\r") {
//         size_t pos = line.find(": ");
//         if (pos != std::string::npos) {
//             std::string headerName = line.substr(0, pos);
//             std::string headerValue = line.substr(pos + 2);
//             // Stocker l'en-tête dans le map `_headers`
//             // this->_headers[headerName] = headerValue;
//         }
//     }
// }

void HttpRequest::parseHttpRequest() {
    std::istringstream stream(_requestData);
    std::string line;

    std::getline(stream, line);
    std::istringstream requestLine(line);
    std::string methodStr;

    requestLine >> methodStr >> this->_path >> this->_version;

    this->_method = stringToHttpMethod(methodStr);

    // Lire les en-têtes
    while (std::getline(stream, line) && line != "\r") {
        size_t pos = line.find(": ");
        if (pos != std::string::npos) {
            std::string headerName = line.substr(0, pos);
            std::string headerValue = line.substr(pos + 2);
            // Stocker l'en-tête dans le map `_headers`
            // this->_headers[headerName] = headerValue;
        }
    }
}

bool HttpRequest::appendRequest(const char* data, int length) {
	_headersParsed = false;
    _requestData.append(data, length);  // Accumule les données reçues dans une std::string
    // Vérifiez si la requête est complète
    if (_requestData.find("\r\n\r\n") != std::string::npos) {  // Fin de l'entête
        // Logique pour vérifier si le corps est complet si Content-Length est spécifié
        if (hasCompleteBody()) {
            return true;
        }
    }
    return false;  // Retourne false si la requête n'est pas encore complète
}

std::string HttpRequest::getFullRequest() const {
    return _requestData;  // Retourne les données accumulées
}

bool HttpRequest::hasCompleteBody() {
    // Si les en-têtes n'ont pas encore été analysés, retournez false
    if (_requestData.find("\r\n\r\n") == std::string::npos) {
        return false;  // En-têtes incomplets
    }

    // Si les en-têtes sont présents mais pas encore analysés
    if (!_headersParsed) {
        parseHeaders();  // Une méthode qui extrait les en-têtes de _requestData
        _headersParsed = true;  // Indiquer que les en-têtes ont été traités
    }

    // Cas 1 : Vérifier Content-Length
    if (_headers.count("Content-Length") > 0) {
		size_t contentLength = std::strtol(_headers["Content-Length"].c_str(), 0, 10);

        size_t headerEndPos = _requestData.find("\r\n\r\n") + 4;
        size_t bodyLength = _requestData.size() - headerEndPos;
        return bodyLength >= contentLength;
    }

    // Cas 2 : Vérifier Transfer-Encoding: chunked
    if (_headers.count("Transfer-Encoding") > 0 && _headers["Transfer-Encoding"] == "chunked") {
        return isChunkedBodyComplete();
    }

    // Si aucune taille de corps spécifiée, considérer comme complète
    return true;
}

void HttpRequest::parseHeaders() {
    std::istringstream stream(_requestData);
    std::string line;

    // Lire la première ligne (request line) et l'ignorer dans ce contexte
    std::getline(stream, line);

    // Lire les en-têtes jusqu'à la ligne vide
    while (std::getline(stream, line) && line != "\r") {
        size_t pos = line.find(": ");
        if (pos != std::string::npos) {
            std::string headerName = line.substr(0, pos);
            std::string headerValue = line.substr(pos + 2);
            _headers[headerName] = headerValue;
        }
    }
}

bool HttpRequest::isChunkedBodyComplete() {
    size_t pos = _requestData.find("\r\n\r\n") + 4;  // Position du début du corps
    while (pos < _requestData.size()) {
        // Trouver la taille du chunk
        size_t nextPos = _requestData.find("\r\n", pos);
        if (nextPos == std::string::npos) return false;

        // Lire la taille en hexadécimal
        std::string chunkSizeStr = _requestData.substr(pos, nextPos - pos);
        int chunkSize = std::strtol(chunkSizeStr.c_str(), 0, 16);

        // Si la taille est zéro, le corps est terminé
        if (chunkSize == 0) return true;

        // Positionner au début du contenu du chunk
        pos = nextPos + 2 + chunkSize;  // Skip \r\n après la taille et avancer de chunkSize

        // Vérifier la fin du chunk
        if (_requestData.substr(pos, 2) != "\r\n") return false;

        pos += 2;  // Skip the \r\n after the chunk content
    }
    return false;  // Corps incomplet si on sort de la boucle sans retour
}

