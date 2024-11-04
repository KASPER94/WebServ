/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/02 19:24:38 by skapersk          #+#    #+#             */
/*   Updated: 2024/11/04 18:01:39 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

HttpRequest::HttpRequest() {}

HttpRequest::HttpRequest(const std::string &req) {
    parseHttpRequest(req);
}

HttpRequest::~HttpRequest() {}

enum HttpMethod HttpRequest::getMethod() const {
    return this->_method;
}

HttpMethod HttpRequest::stringToHttpMethod(const std::string &methodStr) {
    if (methodStr == "GET") return GET;
    if (methodStr == "POST") return POST;
    if (methodStr == "DELETE") return DELETE;
    return OTHER;
}

void HttpRequest::parseHttpRequest(const std::string& request) {
    std::istringstream stream(request);
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

// void HttpRequest::parseHttpRequest(const std::string& request) {
	
// }
