/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mank <ael-mank@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/02 19:24:38 by skapersk          #+#    #+#             */
/*   Updated: 2024/12/11 13:56:55 by ael-mank         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

HttpRequest::HttpRequest() {}

HttpRequest::HttpRequest(Client *client): _client(client) {
	logMsg(DEBUG, "HttpRequest created at " + toString(this));
}

HttpRequest::~HttpRequest() {
	logMsg(DEBUG, "HttpRequest created at " + toString(this));
	_headers.clear();
    _formData.clear();
    _fileData.clear();
    _query.params.clear();
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

std::string HttpRequest::getBody() const {
    size_t headerEnd = _requestData.find("\r\n\r\n");
    if (headerEnd == std::string::npos) {
        return "";
    }
    return _requestData.substr(headerEnd + 4);
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
	std::string tmp = line.substr(0, 10);
    if (tmp == "keep-alive" || tmp == "Keep-Alive" || tmp == "keep-alive") {
        _keepAliveConnection = true;
	}
    else
        _keepAliveConnection = false;
}

void HttpRequest::parseUserAgent(std::string &line) {
    _userAgent = line;
}

void HttpRequest::parseContentLen(std::string &line) {
    _contentLen = atoi(line.c_str());
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
}

void HttpRequest::parseHost(std::string &line) {
    _host = line;
}

void HttpRequest::parseCookie(std::string &line) {
    _cookie = line;
}

void HttpRequest::parseContentLength() {
    std::map<std::string, std::string>::iterator it = _headers.find("Content-Length");
    if (it != _headers.end()) {
        try {
            _contentLen = std::atoi(it->second.c_str());
        } catch (const std::invalid_argument &e) {
            _contentLen = 0;
        } catch (const std::out_of_range &e) {
            _contentLen = 0;
        }
    } else {
        size_t headerEnd = _rawRequest.find("\r\n\r\n");
        if (headerEnd != std::string::npos) {
            headerEnd += 4;
            _contentLen = _rawRequest.size() - headerEnd;
        } else {
            _contentLen = 0;
        }
    }
}

void HttpRequest::initializeHeaderParsers() {
	parseAcceptedMimes(_headers["Accept"]);
	parseConnection(_headers["Connection"]);
	parseUserAgent(_headers["User-Agent"]);
	parseContentType(_headers["Content-Type"]);
	// parseContentLen(_headers["Content-Length"]);
	parseContentLength();
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

    size_t maxBodySize = this->_client->getServer()->getClientMaxBody();
    // Si la taille maximale est définie et la requête dépasse cette limite
    if (maxBodySize > 0 && _receivedBodySize > maxBodySize) {
        _tooLarge = true; // Marquer la requête comme trop grande
        return true;     // Arrêter le traitement
    }
    _requestData.append(data, length);  // Accumule les données reçues dans une std::string
    // Vérifiez si la requête est complète
    if (_requestData.find("\r\n\r\n") != std::string::npos) {  // Fin de l'entête
        // Logique pour vérifier si le corps est complet si Content-Length est spécifié
		if (hasCompleteBody()) {
			this->processMultipartData();
			_endRequested = true;
            return (_endRequested);
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
	// Cas 1 : Vérifier Content-Length
    if (_headers.count("Content-Length") > 0) {
		size_t contentLength = std::strtol(_headers["Content-Length"].c_str(), 0, 10);
        size_t headerEndPos = _requestData.find("\r\n\r\n") + 4;
        size_t bodyLength = _requestData.size() - headerEndPos;
      	if (bodyLength >= contentLength) {
            return true; // Body is complete
        }
        return false;
    }
	if (_contentType == "multipart/form-data" && !_boundary.empty()) {
        size_t boundaryEnd = _requestData.rfind(_boundary + "--");
        if (boundaryEnd != std::string::npos && boundaryEnd + (_boundary.size() + 4) <= _requestData.size()) {
            return true; // Final boundary found
        }
        return false; // Boundary not found yet
    }
    // Cas 2 : Vérifier Transfer-Encoding: chunked
    if (_headers.count("Transfer-Encoding") > 0 && _headers["Transfer-Encoding"] == "chunked") {
        return isChunkedBodyComplete();
    }    // Si aucune taille de corps spécifiée, considérer comme complète
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
	else {
        size_t bodyStart = _requestData.find("\r\n\r\n") + 4;
		_contentLen = bodyStart;
        if (bodyStart != std::string::npos) {
            std::string bodyContent = _requestData.substr(bodyStart);
            _formData["raw_body"] = bodyContent;
        }
    }
	// else
		// this->_isGood = false;
}

bool	HttpRequest::isGood() const {
	return (this->_isGood);
}


void HttpRequest::decodeFormData() {
    size_t pos = _requestData.find("\r\n\r\n") + 4;
    std::string tmp = _requestData.substr(pos);

    if (tmp.find(_boundary) == std::string::npos) {
        logMsg(ERROR, "Broken request: delimiter not found");
        return;
    }

    size_t boundaryPos = tmp.find(_boundary);
    while (boundaryPos != std::string::npos) {
        boundaryPos += _boundary.size() + 2; // Sauter le boundary et "\r\n"
        size_t nextBoundaryPos = tmp.find(_boundary, boundaryPos);
        if (nextBoundaryPos == std::string::npos) {
            nextBoundaryPos = tmp.find(_boundary + "--", boundaryPos);
        }

        while (boundaryPos < tmp.size() && (tmp[boundaryPos] == '\r' || tmp[boundaryPos] == '\n' || tmp[boundaryPos] == ' ')) {
            ++boundaryPos;
        }
        std::string part = tmp.substr(boundaryPos, nextBoundaryPos - boundaryPos);

        size_t namePos = part.find("name=\"");
        if (namePos != std::string::npos) {
            namePos += 6;
            size_t endNamePos = part.find("\"", namePos);
            std::string name = part.substr(namePos, endNamePos - namePos);

            size_t contentPos = part.find("\r\n\r\n") + 4;
            std::string content = part.substr(contentPos);

            size_t contentBackS = content.find("\n");
            if (contentBackS != std::string::npos) {
                std::string tmp3 = _boundary + "--";
                if (content.find(tmp3) == std::string::npos) { 
                    content = content.substr(0, content.size() - _boundary.size() - 4);
                }
            }

            size_t contentEnd = content.rfind("\r\n--" + _boundary);
            if (contentEnd != std::string::npos) {
                content = content.substr(0, contentEnd);
            }

            if (part.find("filename=\"") != std::string::npos) {
                size_t filenamePos = part.find("filename=\"") + 10;
                size_t endFilenamePos = part.find("\"", filenamePos);
                std::string fileName = part.substr(filenamePos, endFilenamePos - filenamePos);
                _fileData[fileName] = content;
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
    size_t pos = _requestData.find("\r\n\r\n");
    if (pos == std::string::npos) {
        return false; // Headers are incomplete
    }

    pos += 4; // Start of the body

    while (pos < _requestData.size()) {
        size_t nextPos = _requestData.find("\r\n", pos);
        if (nextPos == std::string::npos) {
            return false; // Incomplete chunk size line
        }

        std::string chunkSizeStr = _requestData.substr(pos, nextPos - pos);
        int chunkSize = std::strtol(chunkSizeStr.c_str(), NULL, 16);

        if (chunkSize == 0) {
            size_t finalBoundaryPos = nextPos + 2; // After "0\r\n"
            if (_requestData.substr(finalBoundaryPos, 2) == "\r\n") {
                _completed = true;
                return true; // End of chunks
            }
            return false; // Incomplete final chunk ending
        }

        pos = nextPos + 2; // Skip "\r\n"
        if (pos + chunkSize > _requestData.size()) {
            return false; // Incomplete chunk data
        }
        pos += chunkSize + 2; // Skip chunk data and trailing "\r\n"
    }

    return false; // Still waiting for more chunks
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

size_t HttpRequest::getContentLen() const {
	return (_contentLen);
}

std::string	HttpRequest::returnURI() {
	return (_uri);
}

std::string	HttpRequest::returnPATH() {
	return (_path);
}

std::map<std::string, std::string> &HttpRequest::getFileData() {
	return this->_fileData;
}

std::map<std::string, std::string> &HttpRequest::getFormData() {
	return this->_formData;
}

std::map<std::string, std::string> 	HttpRequest::getHeaders() {
	return this->_headers;
}

s_query HttpRequest::getQueryString() {
    return _query;
}
