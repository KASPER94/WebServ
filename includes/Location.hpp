/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 15:34:59 by peanut            #+#    #+#             */
/*   Updated: 2024/11/19 15:52:07 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Location.hpp
#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>
#include <vector>
#include <map>

class Location {
private:
    std::string _root;
    std::string _index;
    std::string _cgi_bin;
    std::string _cgi_extension;
    std::string _upload_path;
    std::map<int, std::string> _return_uri;
    bool _autoindex;
    std::vector<std::string> _allowed_methods;
    std::map<int, std::string> _error_pages;
	size_t								_maxBodySize;

public:
    Location();
    Location(const Location &cpy);
	~Location();

	Location &operator=(const Location &rhs);

    // Getters
    std::string getRoot() const;
    std::string getIndex() const;
    std::string getCgiBin() const;
    std::string getCgiExtension() const;
    std::string getUploadPath() const;
    std::map<int, std::string> getReturnUri() const;
    bool getAutoindex() const;
    std::vector<std::string> getAllowedMethods() const;
    std::map<int, std::string> getErrorPages() const;
	size_t getClientMaxBody() const;

    // Setters avec validation
    void setRoot(const std::string &root);
    void setIndex(const std::string &index);
    void setCgiBin(const std::string &cgiBin);
    void setCgiExtension(const std::string &cgiExtension);
    void setUploadPath(const std::string &uploadPath);
    void setReturnUri(const std::map<int, std::string> &returnUri);
    void setAutoindex(bool autoindex);
    void setAllowedMethods(const std::vector<std::string> &methods);
    void setErrorPages(const std::map<int, std::string> &errorPages);
	void setClientMaxBody(const size_t &maxBodySize);

    // Ajout de méthodes supplémentaires pour la gestion de la configuration
    void addAllowedMethod(const std::string &method);
    void addErrorPage(int errorCode, const std::string &uri);

    // Méthode de validation
    bool isValidMethod(const std::string &method) const;
};

#endif
