/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 14:51:58 by skapersk          #+#    #+#             */
/*   Updated: 2024/11/15 13:41:39 by skapersk         ###   ########.fr       */
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

void HttpResponse::sendResponse() {
	if (this->getRequest()->tooLarge())
		return handleError(413, "Le contenu de la requête dépasse la taille maximale autorisée par le serveur.");
	if (!initializeResponse())
		return handleError(400, "La requête est invalide. Veuillez vérifier les paramètres et le format de votre demande.");
	// if (!this->methodAllowed(this->getRequest()->getMethod())) {
    //     return handleError(405, "Method Not Allowed");
    // }
    std::string uri;
    bool isDir;
    if (!resolveUri(uri, isDir)) {
        return handleError(404, "Not Found");
    }
    // if (this->getRequest()->getMethod() == DELETE) {
    //     return handleDelete(uri);
    // }
    
    // if (isDir) {
    //     return handleDirectory(uri);
    // }
    
    // if (!hasAccess(uri)) {
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
		_response = "HTTP/1.1 200 OK\r\n"
					"Content-Type: text/plain\r\n"
					"Content-Length: 13\r\n"
					"\r\n"
					"COUCOU WEBSERV !!!";
	} else {
		_response = "HTTP/1.1 405 Method Not Allowed\r\n"
					"Content-Length: 0\r\n"
					"\r\n";
	}
}

void	HttpResponse::setInfos() {
	std::vector<std::string> *tmp = this->_client->getServer()->getUri();
	std::vector<std::string>::iterator it = tmp->begin();
	(void)it;

	this->_root = this->getServer()->getRoot();
	this->_maxBodySize = this->getServer()->getClientMaxBody();
	this->_allowedMethod = *(this->getServer()->getAllowedMethods());
	// this->_directoryListing = this->getServer()->getDirectoryListing();
	this->_errorPage = this->getServer()->getErrorPage();
	this->_returnURI = this->getServer()->getReturnUri();
	// this->_uploadPath = this->getServer()->getUploadPath();
	// this->_cgiBin = this->getServer()->getBinPath();
	// this->_cgiExt = this->getServer()->getCgiExtension();
	this->_isLocation = false;
	this->_indexes = this->getServer()->getIndexes();
}

bool HttpResponse::resolveUri(std::string &uri, bool &isDir) {
    (void)uri;
	(void)isDir;
	return (true);
}

bool	HttpResponse::initializeResponse() {
	this->setInfos();
	if (!this->getRequest()->isGood()) {
		this->handleError(400, "La requête est invalide. Veuillez vérifier les paramètres et le format de votre demande.");
		return (false);
	}
	if (!this->_returnURI.empty()) {
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

    // Prepare a simple body for the redirect response
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
