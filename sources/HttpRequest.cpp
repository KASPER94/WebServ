/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/02 19:24:38 by skapersk          #+#    #+#             */
/*   Updated: 2024/11/08 11:49:24 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

HttpRequest::HttpRequest() {}

HttpRequest::HttpRequest(Client *client): _client(client) {
    std::cout << "HttpRequest created at " << this << std::endl;
}

HttpRequest::~HttpRequest() {
	 std::cout << "HttpRequest destroyed at " << this << std::endl;
}

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

void HttpRequest::setQuery(std::string query) {
	_query.strquery = query;
}

std::string decodeURIComponent(const std::string& encoded) {
    std::string decoded;
    char hex[3] = {0};
    for (size_t i = 0; i < encoded.length(); ++i) {
        if (encoded[i] == '%' && i + 2 < encoded.length()) {
            hex[0] = encoded[i + 1];
            hex[1] = encoded[i + 2];
            decoded += static_cast<char>(std::strtol(hex, NULL, 16));
            i += 2;
        } else if (encoded[i] == '+') {
            decoded += ' ';
        } else {
            decoded += encoded[i];
        }
    }
    return decoded;
}

void HttpRequest::parseUrn(std::string queryString) {
	setQuery(queryString);

    std::istringstream stream(queryString);
    std::string token;

    while (std::getline(stream, token, '&')) {
        std::string::size_type pos = token.find('=');
        if (pos != std::string::npos) {
            std::string key = decodeURIComponent(token.substr(0, pos));
            std::string value = decodeURIComponent(token.substr(pos + 1));
            _query.params[key] = value;
        }
    }
}

void	HttpRequest::getUri() {
	size_t queryPos = this->_path.find("?");
	size_t fragPos = this->_path.find("#");

    if (queryPos != std::string::npos) {
		std::string queryString;
		std::string tmp;
		if (fragPos != std::string::npos) {
			std::string fragString;
        	queryString = this->_path.substr(queryPos + 1, fragPos - 1);
			fragString = this->_path.substr(fragPos + 1);
		}
        else
			queryString = this->_path.substr(queryPos + 1);
        tmp = this->_path.substr(0, queryPos);
		this->_path.erase();
		this->_path = tmp;
        parseUrn(queryString);
    }
}

void HttpRequest::parseAcceptedMimes(std::string &line) {
    std::istringstream stream(line);
    std::string mime;
    while (std::getline(stream, mime, ',')) {
        _acceptedMimes.push_back(mime);
    }
}

void HttpRequest::parseConnection(std::string &line) {
    _keepAliveConnection = (line == "keep-alive");
}

void HttpRequest::parseUserAgent(std::string &line) {
    _userAgent = line;
}

void HttpRequest::parseContentType(std::string &line) {
    _contentType = line;
    if (_contentType.find("multipart/form-data") != std::string::npos) {
        size_t pos = _contentType.find("boundary=");
        if (pos != std::string::npos) {
            _boundary = _contentType.substr(pos + 9);
        }
    }
	std::cout << "++++  " << _boundary << " ++++" << std::endl;
}

void HttpRequest::parseHost(std::string &line) {
    _host = line;
}

void HttpRequest::initializeHeaderParsers() {
	parseAcceptedMimes(_headers["Accept"]);
	parseConnection(_headers["Connection"]);
	parseUserAgent(_headers["User-Agent"]);
	parseContentType(_headers["Content-Type"]);
	parseHost(_headers["Host"]);
}

void HttpRequest::parseHttpRequest() {
    std::istringstream stream(_requestData);
    std::string line;

    // Étape 1 : Lecture de la ligne de requête
    std::getline(stream, line);
    std::istringstream requestLine(line);
    std::string methodStr;

    // Extraction de la méthode, du chemin (URI) et de la version HTTP
    requestLine >> methodStr >> this->_path >> this->_version;
    this->_method = stringToHttpMethod(methodStr);
    // Si l'URI contient une chaîne de requête, extraire et stocker les paramètres
	this->getUri();

    // Étape 2 : traitement des en-tête parser précédemment.
	this->initializeHeaderParsers();
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
	parseHttpRequest();
}

bool HttpRequest::isChunkedBodyComplete() {
    size_t pos = _requestData.find("\r\n\r\n") + 4;  // Position du début du corps
    while (pos < _requestData.size()) {
        // Trouver la taille du chunk
        size_t nextPos = _requestData.find("\r\n", pos);
        if (nextPos == std::string::npos)
			return false;

        // Lire la taille en hexadécimal
        std::string chunkSizeStr = _requestData.substr(pos, nextPos - pos);
        int chunkSize = std::strtol(chunkSizeStr.c_str(), 0, 16);

        // Si la taille est zéro, le corps est terminé
        if (chunkSize == 0)
			return true;

        // Positionner au début du contenu du chunk
        pos = nextPos + 2 + chunkSize;  // Skip \r\n après la taille et avancer de chunkSize

        // Vérifier la fin du chunk
        if (_requestData.substr(pos, 2) != "\r\n")
			return false;

        pos += 2;  // Skip the \r\n after the chunk content
    }
    return false;  // Corps incomplet si on sort de la boucle sans retour
}

Client	*HttpRequest::getClient() const {
	return (this->_client);
}

std::string 	HttpRequest::getHeader(std::string find) {
	return (_headers[find]);
}

