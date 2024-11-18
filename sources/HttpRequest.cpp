/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/02 19:24:38 by skapersk          #+#    #+#             */
/*   Updated: 2024/11/18 18:31:41 by skapersk         ###   ########.fr       */
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
    _contentType = line.substr();
    if (line.find("multipart/form-data") != std::string::npos) {
        size_t pos = _contentType.find("boundary=");
        if (pos != std::string::npos) {
            _boundary = "--" + _contentType.substr(pos + 9);
        }
		_contentType = "multipart/form-data";
    }
	else if (line.find("text/plain") != std::string::npos) {
		_contentType = "text/plain";
	}
	else if (line.find("application/x-www-form-urlencoded") != std::string::npos) {
		_contentType = "application/x-www-form-urlencoded";
	}

	// std::cout << "++++  " << _boundary << " ++++" << std::endl;
	// std::cout << "++++  " << _contentType << " ++++" << std::endl;
}

void HttpRequest::parseHost(std::string &line) {
    _host = line;
}

void HttpRequest::parseCookie(std::string &line) {
    _cookie = line;
}

void HttpRequest::initializeHeaderParsers() {
	parseAcceptedMimes(_headers["Accept"]);
	parseConnection(_headers["Connection"]);
	parseUserAgent(_headers["User-Agent"]);
	parseContentType(_headers["Content-Type"]);
	parseHost(_headers["Host"]);
	parseCookie(_headers["cookie"]);
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
	_endRequested = false;
	_tooLarge = false;
	_headersParsed = false;
	_isGood = true;
	_receivedBodySize += length;

	size_t maxBody = this->_client->getServer()->getClientMaxBody();
	// std::cout << "ClientMaxBody (avant conversion): " << maxBody << std::endl;

	// Vérifiez si maxBody est raisonnable et différent de zéro
	// if (maxBody == 0) {
	// 	std::cerr << "Erreur : ClientMaxBody est à zéro ou non défini !" << std::endl;
	// 	return false;
	// }

	size_t confBodySize = maxBody ;
	std::cout << "ClientMaxBody après conversion en bytes (confBodySize): " << confBodySize << std::endl;

    _requestData.append(data, length);  // Accumule les données reçues dans une std::string
    // Vérifiez si la requête est complète
    if (_requestData.find("\r\n\r\n") != std::string::npos) {  // Fin de l'entête
        // Logique pour vérifier si le corps est complet si Content-Length est spécifié
		if (hasCompleteBody()) {
			_endRequested = true;
            return (_endRequested);
        }
		if (confBodySize && _receivedBodySize >= confBodySize) {
			std::cerr << "Erreur : La taille du corps de la requête dépasse la limite maximale autorisée." << std::endl;
			_tooLarge = true;
            return (_tooLarge);
		}
    }
    return (_endRequested);  // Retourne false si la requête n'est pas encore complète
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

	if (!_completed) {
		// if (_requestData.find(_boundary + "--")) {
		// 	std::cout << "OK    ++++ " << std::endl;
		// }
		this->processMultipartData();
		return (true);
	}

	// Cas 1 : Vérifier Content-Length
    if (_headers.count("Content-Length") > 0) {
		size_t contentLength = std::strtol(_headers["Content-Length"].c_str(), 0, 10);

        size_t headerEndPos = _requestData.find("\r\n\r\n") + 4;
        size_t bodyLength = _requestData.size() - headerEndPos;
		if (bodyLength >= contentLength)
        	return (true);
    }
    // Cas 2 : Vérifier Transfer-Encoding: chunked
    if (_headers.count("Transfer-Encoding") > 0 && _headers["Transfer-Encoding"] == "chunked") {
        return isChunkedBodyComplete();
    }

    // Si aucune taille de corps spécifiée, considérer comme complète
    return (true);
}

void HttpRequest::setForm(std::string form) {
	_form.strform = form;
}

void	HttpRequest::getForm(std::string &str) {
	setForm(str);

    std::istringstream stream(str);
    std::string token;

    while (std::getline(stream, token, '&')) {
        std::string::size_type pos = token.find('=');
        if (pos != std::string::npos) {
            std::string key = decodeURIComponent(token.substr(0, pos));
            std::string value = decodeURIComponent(token.substr(pos + 1));
            _form.form[key] = value;
			// std::cout << value << std::endl;
        }
    }
}

void	HttpRequest::decodeUrl() {
	size_t pos = _requestData.find("\r\n\r\n");
	if (pos != std::string::npos) {
		pos += 4;
		std::string tmp = _requestData.substr(pos);
		getForm(tmp);
	}
}

void	HttpRequest::processMultipartData() {
	if (_contentType == "multipart/form-data") {
		this->decodeFormData();
	}
	else if (_contentType == "application/x-www-form-urlencoded") {
		this->decodeUrl();
	}
	else if (_contentType == "text/plain") {
		std::string plainTextContent = _requestData.substr(_requestData.find("\r\n\r\n") + 4);
		// std::cout << "Contenu text/plain reçu : " << plainTextContent << std::endl;
		_plainTextBody = plainTextContent;
	}
	// else
		// this->_isGood = false;
}

bool	HttpRequest::isGood() const {
	return (this->_isGood);
}

// void	HttpRequest::decodeFormData() {
// 	size_t	pos = _requestData.find("\r\n\r\n") + 4;

// 	std::string tmp = _requestData.substr(pos);
// 	if (tmp.find(_boundary) == std::string::npos) {
// 		std::cerr << "Erreur: Boundary introuvable dans le corps de la requête" << std::endl;
// 		return;
// 	}
// 	size_t boundaryPos = tmp.find(_boundary);
// 	while (boundaryPos != std::string::npos) {
// 		boundaryPos += _boundary.size() + 1;  // Aller au début du contenu après boundary
// 		size_t nextBoundaryPos = tmp.find(_boundary, boundaryPos);

// 		if (nextBoundaryPos == std::string::npos) {
// 			nextBoundaryPos = tmp.find(_boundary + "--");
// 		}
// 			// break;

// 		std::string part = tmp.substr(boundaryPos, nextBoundaryPos - boundaryPos);

// 		// Vérifiez si c'est une image ou du texte
// 		if (part.find("Content-Disposition: form-data; name=\"image\"") != std::string::npos) {
// 			std::cout << "[Image data - binary content hidden]" << std::endl;
// 		} else {
// 			std::cout << "Données de formulaire texte reçues: " << part << std::endl;
// 		}

// 		boundaryPos = nextBoundaryPos;
// 	}
// }

void HttpRequest::decodeFormData() {
	size_t pos = _requestData.find("\r\n\r\n") + 4;
	std::string tmp = _requestData.substr(pos);

	if (tmp.find(_boundary) == std::string::npos) {
		std::cerr << "Erreur: Boundary introuvable dans le corps de la requête" << std::endl;
		return;
	}

	size_t boundaryPos = tmp.find(_boundary);
	while (boundaryPos != std::string::npos) {
		boundaryPos += _boundary.size() + 1; // Aller au début du contenu après boundary
		size_t nextBoundaryPos = tmp.find(_boundary, boundaryPos);
		if (nextBoundaryPos == std::string::npos) {
			nextBoundaryPos = tmp.find(_boundary + "--");
		}

		std::string part = tmp.substr(boundaryPos, nextBoundaryPos - boundaryPos);

		// Vérifiez si c'est une image ou du texte
		size_t namePos = part.find("name=\"");
		if (namePos != std::string::npos) {
			namePos += 6; // Aller au début du nom après 'name="'
			size_t endNamePos = part.find("\"", namePos);
			std::string name = part.substr(namePos, endNamePos - namePos);

			size_t contentPos = part.find("\r\n\r\n") + 4;
			std::string content = part.substr(contentPos);

			// Vérifier si c'est un fichier
			if (part.find("filename=\"") != std::string::npos) {
				_fileData[name] = content;
			} else {
				_formData[name] = content;
			}
		}

		boundaryPos = nextBoundaryPos;
	}
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
	_completed = false;
    size_t pos = _requestData.find("\r\n\r\n") + 4;  // Position du début du corps
    while (pos < _requestData.size()) {
        // Trouver la taille du chunk
        size_t nextPos = _requestData.find("\r\n", pos);
        if (nextPos == std::string::npos) {
			_completed = false;
			return false;
		}

        // Lire la taille en hexadécimal
        std::string chunkSizeStr = _requestData.substr(pos, nextPos - pos);
        int chunkSize = std::strtol(chunkSizeStr.c_str(), 0, 16);

        // Si la taille est zéro, le corps est terminé
        if (chunkSize == 0) {
			_completed = true;
			return true;
		}

        // Positionner au début du contenu du chunk
        pos = nextPos + 2 + chunkSize;  // Skip \r\n après la taille et avancer de chunkSize

        // Vérifier la fin du chunk
        if (_requestData.substr(pos, 2) != "\r\n"){
			_completed = false;
			return false;
		}

        pos += 2;  // Skip the \r\n after the chunk content
    }
    return _completed;  // Corps incomplet si on sort de la boucle sans retour
}

Client	*HttpRequest::getClient() const {
	return (this->_client);
}

std::string 	HttpRequest::getHeader(std::string find) {
	return (_headers[find]);
}

bool		 	HttpRequest::getCompleted() const {
	return (_completed);
}

bool		 	HttpRequest::getEnd() const {
	return (_endRequested);
}

bool HttpRequest::tooLarge() const {
	return (_tooLarge);
}

bool HttpRequest::keepAlive() const {
	return (_keepAliveConnection);
}

std::string	HttpRequest::returnURI() {
	return (_uri);
}
