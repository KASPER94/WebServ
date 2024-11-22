/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 15:34:56 by peanut            #+#    #+#             */
/*   Updated: 2024/11/22 23:55:39 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"
#include <algorithm>

Location::Location() : _autoindex(false) {}

Location::Location(const Location &cpy)
    : _root(cpy._root), _index(cpy._index), _cgi_bin(cpy._cgi_bin),
      _cgi_extension(cpy._cgi_extension), _upload_path(cpy._upload_path),
      _return_uri(cpy._return_uri), _autoindex(cpy._autoindex),
      _allowed_methods(cpy._allowed_methods), _error_pages(cpy._error_pages),
      _maxBodySize(cpy._maxBodySize) {}

Location::~Location() {
}

Location &Location::operator=(const Location &rhs) {
    if (this != &rhs) {
        this->_root = rhs._root;
        this->_index = rhs._index;
        this->_cgi_bin = rhs._cgi_bin;
        this->_cgi_extension = rhs._cgi_extension;
        this->_cgiExtensions = rhs._cgiExtensions;
        this->_upload_path = rhs._upload_path;
        this->_return_uri = rhs._return_uri;
        this->_autoindex = rhs._autoindex;
        this->_allowed_methods = rhs._allowed_methods;
        this->_error_pages = rhs._error_pages;
        this->_maxBodySize = rhs._maxBodySize;
    }
    return *this;
}

// Getters
std::string Location::getRoot() const { return _root; }
std::string Location::getIndex() const { return _index; }
std::string Location::getCgiBin() const { return _cgi_bin; }
std::vector<std::string> Location::getCgiExtension() const { return _cgiExtensions; }
std::string Location::getUploadPath() const { return _upload_path; }
std::map<int, std::string> Location::getReturnUri() const { return _return_uri; }
bool Location::getAutoindex() const { return _autoindex; }
std::vector<std::string> Location::getAllowedMethods() const { return _allowed_methods; }
std::map<int, std::string> Location::getErrorPages() const { return _error_pages; }
size_t Location::getClientMaxBody() const {return _maxBodySize;}

// Setters avec validation
void Location::setRoot(const std::string &root) { _root = root; }
void Location::setIndex(const std::string &index) { _index = index; }
void Location::setCgiBin(const std::string &cgiBin) { _cgi_bin = cgiBin; }
void Location::setCgiExtension(const std::vector<std::string> &cgiExtension) { _cgiExtensions = cgiExtension; }
void Location::setUploadPath(const std::string &uploadPath) { _upload_path = uploadPath; }
void Location::setReturnUri(const std::map<int, std::string> &returnUri) { _return_uri = returnUri; }
void Location::setAutoindex(bool autoindex) { _autoindex = autoindex; }
void Location::setAllowedMethods(const std::vector<std::string> &methods) { _allowed_methods = methods; }
void Location::setErrorPages(const std::map<int, std::string> &errorPages) { _error_pages = errorPages; }
void Location::setClientMaxBody(const size_t &maxBodySize) { _maxBodySize = maxBodySize; }

void Location::addAllowedMethod(const std::string &method) {
	if (isValidMethod(method)) {
		_allowed_methods.push_back(method);
	}
}

void Location::addErrorPage(int errorCode, const std::string &uri) {
    _error_pages[errorCode] = uri;
}

const std::vector<std::string> &getValidMethods() {
	static std::vector<std::string> validMethods;
	if (validMethods.empty()) {  // Initialisez une seule fois
		validMethods.push_back("GET");
		validMethods.push_back("POST");
		validMethods.push_back("DELETE");
	}
	return validMethods;
}


// Validation des méthodes autorisées
bool Location::isValidMethod(const std::string &method) const {
	const std::vector<std::string>& validMethods = getValidMethods();
	return std::find(validMethods.begin(), validMethods.end(), method) != validMethods.end();
}
