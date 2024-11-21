/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 14:51:58 by skapersk          #+#    #+#             */
/*   Updated: 2024/11/21 13:56:26 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "HttpResponse.hpp"

HttpResponse::HttpResponse(): _client(NULL) {}

HttpResponse::HttpResponse(Client *client):
	_client(client) {
}

HttpResponse::HttpResponse(const HttpResponse &cpy) {
	*this = cpy;
}

HttpResponse::~HttpResponse() {}

HttpResponse &HttpResponse::operator=(const HttpResponse &rhs) {
	this->_client = rhs._client;
	return (*this);
}

void HttpResponse::sendHeader() {
    std::string header = "HTTP/1.1 " + intToString(this->_statusCode) + " ";

    std::string statusDescription;
    switch (this->_statusCode) {
        case 200: statusDescription = "OK"; break;
        case 301: statusDescription = "Moved Permanently"; break;
        case 400: statusDescription = "Bad Request"; break;
        case 403: statusDescription = "Forbidden"; break;
        case 404: statusDescription = "Not Found"; break;
        case 413: statusDescription = "Payload Too Large"; break;
        case 500: statusDescription = "Internal Server Error"; break;
        default:  statusDescription = "Unknown Status"; break;
    }
    header += statusDescription + "\r\n";

    // Ajout en-têtes HTTP à partir de _headers
    for (std::map<std::string, std::string>::iterator it = this->_headers.begin(); it != this->_headers.end(); ++it) {
        header += it->first + ": " + it->second + "\r\n";
    }

    // Terminer les en-têtes avec une ligne vide
    header += "\r\n";
	std::cout << header << std::endl;

    int bytes = send(this->_client->getFd(), header.c_str(), header.length(), 0);
	this->checkSend(bytes);
}

void	HttpResponse::checkSend(int bytes) {
	if (bytes <= 0) {
		this->_client->setError();
	}
}

bool HttpResponse::hasAccess(std::string &uri, bool &isDir) {
    struct stat	s;

	std::cout << uri << std::endl;

	if (stat(uri.c_str(), &s) == 0) {
		std::cout << "uri" << std::endl;
		if (s.st_mode & S_IFDIR) {
			isDir = true;
			if (uri[uri.length() - 1] != '/') {
				this->movedPermanently(this->getRequest()->returnPATH() + "/");
				return (false);
			}
		}
		else
			isDir = false;
	}
	if (isDir) {
		for (std::vector<std::string>::iterator it = this->_indexes.begin(); it != this->_indexes.end(); it++) {
			if (*it == "")
				continue;
			if (access((uri + *it).c_str(), F_OK) != -1) {
				if (access((uri + *it).c_str(), R_OK) == -1) {
					this->handleError(403, "La requête est invalide. Veuillez vérifier les paramètres et le format de votre demande.");
					return (false);
				}
				uri += *it;
				isDir = false;
				break;
			}
		}
	}
	return (true);
    // return access(uri.c_str(), F_OK) != -1 && access(uri.c_str(), R_OK) != -1;
}

bool	HttpResponse::methodAllowed(enum HttpMethod method) {
	std::vector<std::string>::iterator it = this->_allowedMethod.begin();
	(void)method;
	if (it == this->_allowedMethod.end())
		if (*it == this->getRequest()->HttpMethodTostring())
			return (true);
	for (; it != this->_allowedMethod.end(); it++) {
		std::cout << *it << std::endl;
		if (*it == this->getRequest()->HttpMethodTostring())
			return (true);
	}
	return (false);
}

// void HttpResponse::handleError(string uri) {
// 	string root;

// 	if (this->)
// }

void HttpResponse::sendResponse() {
	if (this->getRequest()->tooLarge())
		return handleError(413, "Le contenu de la requête dépasse la taille maximale autorisée par le serveur.");
	if (!initializeResponse())
		return handleError(400, "La requête est invalide. Veuillez vérifier les paramètres et le format de votre demande.");
	if (!this->methodAllowed(this->getRequest()->getMethod())) {
        return handleError(405, "Method Not Allowed");
    }
    std::string uri = this->getRequest()->returnPATH();
    bool isDir;
    if (!resolveUri(uri, isDir)) {
        return handleError(404, "Not Found");
    }
	// std::string bestMatch = matchLocation(uri);
    // if (bestMatch.empty()) {
    //     return handleError(404, "Not Found");
    // }
    // if (this->getRequest()->getMethod() == DELETE) {
    //     return handleDelete(uri);
    // }
    
    // if (isDir) {
    //     return handleDirectory(uri);
    // }
    
    // if (!hasAccess(bestMatch)) {
    //     return handleError(403, "Forbidden");
    // }

    // if (isCGIRequest(uri)) {
    //     if (!handleCGI(uri)) {
    //         return handleError(500, "Internal Server Error");
    //     }
    // } else {
    //     if (!handleFileResponse(uri)) {
    //         return handleError(404, "Not Found");
    //     }
    // }
    
    // finalizeResponse();
	if (this->getRequest()->getMethod() == GET) {
		std::string body = "COUCOU WEBSERV !!!";
		std::string headers = "HTTP/1.1 200 OK\r\n"
							"Content-Type: text/plain\r\n"
							"Content-Length: " + intToString(body.size()) + "\r\n"
							"Connection: close\r\n"
                  			"\r\n";
		_response = headers + body;
	} else {
		_response = "HTTP/1.1 405 Method Not Allowed\r\n"
					"Content-Length: 0\r\n"
					"Connection: close\r\n"
					"\r\n";
	}
}

void	HttpResponse::setInfos() {
	this->_isLocation = false;
	std::map<std::string, Location*> locs = this->getServer()->returnLoc();

	if (!locs.empty()) {
		this->_isLocation = true;
		std::vector<std::string> *tmp = this->_client->getServer()->getUri();
		std::vector<std::string>::iterator it = tmp->begin();
		this->_uri = *it;
	}
	this->_uri = "";
	this->_root = this->getServer()->getRoot();
	this->_maxBodySize = this->getServer()->getClientMaxBody();
	this->_allowedMethod = *(this->getServer()->getAllowedMethods());
	// this->_directoryListing = this->getServer()->getDirectoryListing();
	this->_errorPage = this->getServer()->getErrorPage();
	this->_returnURI = this->getServer()->getReturnUri();
	// this->_uploadPath = this->getServer()->getUploadPath();
	this->_cgiBin = this->getServer()->getCgiBin();
	this->_cgiExt = this->getServer()->getCgiExtension();
	this->_indexes = this->getServer()->getIndexes();
}

bool HttpResponse::resolveUri(std::string &uri, bool &isDir) {
	std::string resolvePath;
	std::string location;
	bool follow = true;
	isDir = false;
	
	if (_isLocation) {
		location = matchLocation(uri);
		if (!location.empty()) {
			// resolvePath = _root + location;
			resolvePath = this->getServer()->getLocation(location)->getRoot() + location;
		} else {
			follow = false;
			// return (follow);
		}
	}
	if (uri == "/" || uri == "") {
		if (this->_root.c_str()[_root.size() - 1] == '/')
			resolvePath = _root + this->_indexes[0];
		else 
			resolvePath = _root + "/" + this->_indexes[0];
		follow = true;
	}
	for (std::vector<std::string>::iterator it = this->_cgiExt.begin(); it != this->_cgiExt.end(); it++) {
		if (*it != "" && (uri.find(*it) != std::string::npos || uri.find(*it) != std::string::npos)) {
			resolvePath = _root + uri;
            // std::cout << "CGI Request Detected: " << resolvePath << std::endl;
			follow = true;
            break;
		}
		else if (uri != "" && !_isLocation) {
			follow = false;
		}
	}

	if (!hasAccess(resolvePath, isDir)) {
        return (follow);
    }
	return (follow);
}

std::string HttpResponse::matchLocation(std::string &requestUri) const {
    std::vector<std::string> *locations = this->_client->getServer()->getUri();
    std::string bestMatch = "";
    size_t bestMatchLength = 0;

    for (std::vector<std::string>::iterator it = locations->begin(); it != locations->end(); ++it) {
        const std::string &location = *it;
        if (requestUri.find(location) == 0 && location.length() > bestMatchLength) {
            bestMatch = location;
            bestMatchLength = location.length();
        }
    }
    return bestMatch;
}

bool	HttpResponse::initializeResponse() {
	this->setInfos();
	if (!this->getRequest()->isGood()) {
		this->handleError(400, "You do not have permission to access this resource.");
		return (false);
	}
	std::string uri = this->_returnURI.begin()->second;
	if (!uri.empty()) {
		std::cout << uri << std::endl;
        std::map<int, std::string>::iterator redirect = this->_returnURI.begin();
        if (redirect->first == 301) {
            this->movedPermanently(redirect->second);
        } else if (redirect->first >= 300 && redirect->first < 400) { 
            this->handleRedirect(redirect->first, redirect->second);
		} else {
            this->handleError(500, "Unexpected redirection code in configuration.");
        }
        return (false);
    }
	return (true);
}

void HttpResponse::handleRedirect(int code, const std::string &uri) {
    this->_statusCode = code;
    this->_headers["Location"] = uri;

    // sample body for the redirect response
    this->_body = "<html><head><title>Redirect</title></head>";
    this->_body += "<body><h1>" + intToString(code) + " Redirect</h1>";
    this->_body += "<p>The document has moved <a href=\"" + uri + "\">here</a>.</p>";
    this->_body += "</body></html>";

    this->_headers["Content-Length"] = intToString(this->_body.size());
    this->_headers["Content-Type"] = "text/html";

    // Log the redirect for debugging
    std::cerr << "Redirecting to " << uri << " with status code " << code << std::endl;
}

void HttpResponse::movedPermanently(const std::string &url) {
    this->_statusCode = 301;
    this->_headers["Location"] = url;
    this->_body = "<html><head><title>301 Moved Permanently</title></head>"
                  "<body><h1>301 Moved Permanently</h1>"
                  "<p>The requested resource has been permanently moved to "
                  "<a href=\"" + url + "\">" + url + "</a>.</p></body></html>";
    this->_headers["Content-Length"] = intToString(this->_body.size());
	this->sendHeader();
	this->_client->setError();
}


HttpRequest	*HttpResponse::getRequest() const {
	return (this->_client->getRequest());
}

void HttpResponse::error(const std::string &message) {
	std::string statusDescription;
	switch (this->_statusCode) {
		case 404:
			statusDescription = "Not Found";
			break;
		case 400:
			statusDescription = "Bad Request";
			break;
		case 500:
			statusDescription = "Internal Server Error";
			break;
		case 403:
			statusDescription = "Forbidden";
			break;
		case 413:
			statusDescription = "Payload Too Large";
			break;
		// Ajouter d'autres codes de statut;
		default:
			statusDescription = "Error";
	}

	this->_headers["Content-Type"] = "text/html";

	this->_body = "<html><head><title>" + intToString(this->_statusCode) + " " + statusDescription + "</title></head>";
	this->_body += "<body><h1>" + intToString(this->_statusCode) + " " + statusDescription + "</h1>";
	this->_body += "<p>" + message + "</p></body></html>";

	this->_headers["Content-Length"] = intToString(this->_body.size());
	// std::string header;
	// for (std::map<std::string, std::string>::iterator it = this->_headers.begin(); it != this->_headers.end(); ++it) {
    //     header += it->first + ": " + it->second + "\r\n";
    // }

    // // Terminer les en-têtes avec une ligne vide
    // header += "\r\n";
	_response = _body;
	sendHeader();
	this->_readyToSend = true;
}


void HttpResponse::handleError(int code, const std::string &message) {
	this->_statusCode = code;
	this->error(message);
}

std::string	HttpResponse::getResponse() {
	return (_response);
}

Server	*HttpResponse::getServer() const {
	return (this->_client->getServer());
}

void	HttpResponse::sendChunkEnd() {
	int	bytes;

	bytes = send(this->_client->getFd(), "\r\n", 2, 0);
	this->checkSend(bytes);
}

void	HttpResponse::sendFinalChunk() {
	int	bytes;

	bytes = send(this->_client->getFd(), "0\r\n\r\n", 5, 0);
	this->checkSend(bytes);
}
