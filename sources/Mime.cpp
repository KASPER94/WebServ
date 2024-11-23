/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mime.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 13:27:35 by skapersk          #+#    #+#             */
/*   Updated: 2024/11/23 15:19:57 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Mime.hpp"

std::map<std::string, std::string> Mime::mimeTypes = std::map<std::string, std::string>();

Mime::Mime() {
	mimeTypes["html"] = "text/html";
	mimeTypes["htm"] = "text/html";
	mimeTypes["css"] = "text/css";
	mimeTypes["js"] = "application/javascript";
	mimeTypes["php"] = "application/x-httpd-php";
	mimeTypes["json"] = "application/json";
	mimeTypes["xml"] = "application/xml";
	mimeTypes["jpg"] = "image/jpeg";
	mimeTypes["jpeg"] = "image/jpeg";
	mimeTypes["png"] = "image/png";
	mimeTypes["gif"] = "image/gif";
	mimeTypes["ico"] = "image/x-icon";
	mimeTypes["svg"] = "image/svg+xml";
	mimeTypes["txt"] = "text/plain";
	mimeTypes["pdf"] = "application/pdf";
	mimeTypes["zip"] = "application/zip";
	mimeTypes["tar"] = "application/x-tar";
	mimeTypes["gz"] = "application/gzip";
	mimeTypes["mp4"] = "video/mp4";
	mimeTypes["mpeg"] = "video/mpeg";
	mimeTypes["mp3"] = "audio/mpeg";
}

Mime::Mime(const Mime &cpy) {
	*this = cpy;
}

Mime::~Mime() {}

Mime &Mime::operator=(const Mime &rhs) {
	if (this->mimeTypes.size() != rhs.mimeTypes.size()){
		this->mimeTypes.clear();
        for (std::map<std::string, std::string>::const_iterator it = rhs.mimeTypes.begin(); it != rhs.mimeTypes.end(); ++it) {
            this->mimeTypes[it->first] = it->second;
        }
	}
	return (*this);
}

std::string Mime::getMimeType(const std::string &extension) {
	Mime tmp;
	std::map<std::string, std::string>::const_iterator it = tmp.mimeTypes.find(extension);
	if (it != mimeTypes.end()) {
		return it->second;
	}
	return "application/octet-stream";
}

void Mime::addMimeType(const std::string &extension, const std::string &type) {
	mimeTypes[extension] = type;
}