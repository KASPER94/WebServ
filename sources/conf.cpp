/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   conf.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 15:50:49 by skapersk          #+#    #+#             */
/*   Updated: 2024/11/19 17:01:27 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "conf.hpp"
#include "webserv.h"

bool isDir(const std::string &str){
    struct stat	type;

	if (stat(str.c_str(), &type) == 0 && type.st_mode &S_IFDIR)
		return (true);
    return (false);
}

bool isConf(const std::string &str) {
    size_t i = str.find(".conf");

    if (i == str.size() - 5)
        return (true);
    return (false);
}

bool   conf::_findServerBlock(std::string &line) {
    std::istringstream	word(line);
    std::string w;
    this->_blockLevel = 0;

    while (word >> w) {
        if (w.compare("server") == 0) {
            this->_found = true;
            if (word >> w && w == "{") {
                this->_blockLevel = 1;
                return (this->_nbServer++, true);
            }
        }
        else if (w == "server{") {
            this->_found = true;
            this->_blockLevel = 1;
          return (this->_nbServer++, true);
        } else {
            this->_blockLevel = 0;
        }
    }
    return (false);
}

void conf::_parseLocation(std::string line, Server &serv, std::ifstream &ConfigFile) {
	std::istringstream word(line);
	std::string location_keyword;
	std::string modifier_or_uri;
	std::string uri;

	// Lire "location" et l'URI ou le modificateur
	word >> location_keyword >> modifier_or_uri;
	if (modifier_or_uri == "~" || modifier_or_uri == "~*" || modifier_or_uri == "^~") {
		// Si un modificateur est détecté, l’URI est le mot suivant
		word >> uri;
		// Stocker le modificateur dans la configuration de location (si nécessaire)
		// ATTETNION PENSER A : locationConfig.setModifier(modifier_or_uri);
	} else {
		uri = modifier_or_uri;
	}

	Location locationConfig;
	int brace_count = 1;  // '{' trouvé dans "location"

	while (brace_count > 0 && std::getline(ConfigFile, line)) {
		ltrim(line);
		rtrim(line);
		if (line.empty() || line[0] == '#' || line[0] == ';')
			continue;

		if (line.find("{") != std::string::npos)
			brace_count++;
		if (line.find("}") != std::string::npos)
			brace_count--;

		if (brace_count == 0) 
			break;

		std::vector<std::string> tokens = split_trim_conf(line);
		if (tokens.empty())
			continue;

		std::string directive = tokens[0];
		if (directive == "root") {
			locationConfig.setRoot(this->_getRoot(tokens));
		} else if (directive == "index") {
		// 	locationConfig.setIndex(this->_getIndex(tokens));
		} else if (directive == "cgi_bin") {
			locationConfig.setCgiBin(this->_getCgiBin(tokens));
		} else if (directive == "cgi_extension") {
			locationConfig.setCgiExtension(this->_getCgiExtension(tokens));
		} else if (directive == "allowedMethods") {
			locationConfig.setAllowedMethods(*this->_getAllowedMethods(tokens));
		} else if (directive == "upload_path") {
			locationConfig.setUploadPath(this->_getUploadPath(tokens));
		} else if (directive == "return") {
			locationConfig.setReturnUri(this->_getRedirection(tokens));
		} else if (directive == "autoindex") {
			locationConfig.setAutoindex(this->_getAutoindex(tokens));
		} else if (directive == "error_page") {
			locationConfig.setErrorPages(this->_getErrorPage(tokens));
		} else if (directive == "client_max_body_size") {
			locationConfig.setClientMaxBody(this->_getClientMaxBodySize(tokens));
		} else {
			throw std::runtime_error("Unknown directive in location block: " + directive);
		}
	}
	serv.addLocation(uri, locationConfig);
}

void    conf::_parseLine(std::string &line, Server	&serv, std::vector<Server> &allServ) {
    std::istringstream	word(line);
    std::string w;
    std::vector<std::string> line_trim;

    word >> w;
	if (!this->_blockLevel && w.compare("{") != 0) {
		throw std::runtime_error("Error: Invalid server block");
    }
	else if ( w.compare("#") == 0) {
		return ;
    }
    else
        this->_blockLevel = 1;
    line_trim = split_trim_conf(line);
    std::string directive = *line_trim.begin();
	switch (getDirective(directive)) {
		case SERVER:
			throw std::runtime_error("Error: Syntax of config file is not ok");
		case LISTEN:
			serv.setPort(this->_getListen(line_trim));
			break;
		case SERVER_NAME:
			serv.setServerName(this->_getServerName(line_trim));
			break;
		case HOST:
			serv.setHostname(this->_getHost(line_trim));
			break;
		case INDEX:
			serv.setIndexes(this->_getIndex(line_trim));
			break;
		case ERROR_PAGE:
			serv.setErrorPage(this->_getErrorPage(line_trim));
			break;
		case CLIENT_MAX_BODY_SIZE:
			serv.setClientMaxBody(this->_getClientMaxBodySize(line_trim));
			break;
		case ALLOWED_METHODS:
			serv.setAllowedMethods(this->_getAllowedMethods(line_trim));
			break;
		case RETURN_URI:
			serv.setReturnUri(this->_getRedirection(line_trim));
			break;
		case CGI_BIN:
			// this->_getCgiBin(line_trim);
			break;
		case AUTOINDEX:
			// this->_getAutoindex(line_trim);
			break;
		case UPLOAD_PATH:
			// this->_getUploadPath(line_trim);
			break;
		case ROOT:
			serv.setRoot(this->_getRoot(line_trim));
			break;
		case RETURN:
			// this->_getRedirection(line_trim);
			break;
		case CGI_EXTENSION:
			// this->_getCgiExtension(line_trim);
			break;
		case CGI_PATH:
			// this->_getCgiPath(line_trim);
			break;
		case CLOSE_BRACKET:
			allServ.push_back(serv);
			serv.reset();
			this->_found = false;
			return;
		case UNKNOWN:
		default:
			throw std::runtime_error("Unknown directive: " + directive);
	}
}

// std::vector<Server> conf::_getRawConfig(std::ifstream &ConfigFile) {
//     std::string line;
// 	std::vector<Server> allServ;
// 	Server	serv;
//     this->_found = false;
//     int brace_count = 0;
//     this->_nbServer = 0;

//     while (std::getline(ConfigFile, line)) {
//         if (line.empty() || line[0] == '#' || line[0] == ';')
//             continue;
//         if (!this->_found) {
//             this->_findServerBlock(line);
//         } else if (this->_found && line.find("location") != std::string::npos) {
//             if (line.find("{") != std::string::npos)
//                 brace_count++;
//             // this->_parseLocation(line, serv, ConfigFile);
//             if (line.find("}") != std::string::npos) {
//                 brace_count--;
//                 if (brace_count == 0)
//                     continue;
//             }
//         } else if (this->_found && brace_count > 0) {
//             // this->_parseLocation(line, serv, ConfigFile);
//             if (line.find("}") != std::string::npos) {
//                 brace_count--;
//                 if (brace_count == 0)
//                     continue;
//             }
//         } else if (this->_found) {
//             this->_parseLine(line, serv, allServ);
//         }
//     }
//     if (brace_count != 0)
//         throw std::runtime_error("Unclosed bracket");
//     if (!this->_found && !this->_nbServer){
//         throw std::runtime_error("File is not containing a server block.");
// 	}
//     return allServ;
// }

std::vector<Server> conf::_getRawConfig(std::ifstream &ConfigFile) {
    std::string line;
	Server	serv;
	std::vector<Server> allServ;
    this->_found = false;
    int brace_count = 0;
    this->_nbServer = 0;

    while (std::getline(ConfigFile, line)) {
		// Server	serv;
        if (line.empty() || line[0] == '#' || line[0] == ';')
            continue ;
        if (!this->_found) {
            this->_findServerBlock(line);
        } else if (this->_found && brace_count > 0) {
            if (line.find("{") != std::string::npos)
                brace_count++;
            // this->_parseLocation(line, serv, ConfigFile);
            if (line.find("}") != std::string::npos) {
                brace_count--;
                if (brace_count == 0)
                    continue;
            }
        // } else if (this->_found && brace_count > 0) {
        //     if (line.find("}") != std::string::npos) {
        //         brace_count--;
        //         if (brace_count == 0)
        //             continue;
        //     }
        } else if (this->_found) {
			if (line.find("location") != std::string::npos) {
            	this->_parseLocation(line, serv, ConfigFile);
			}
			else
        	   	this->_parseLine(line, serv, allServ);
        }
		
    }
    if (brace_count != 0)
        throw std::runtime_error("Unclosed bracket");
    if (!this->_found && !this->_nbServer){
        throw std::runtime_error("File is not containing a server block.");

	}
	for (std::vector<Server>::iterator it = allServ.begin(); it != allServ.end(); it++)
				std::cout << *it << std::endl;
    return allServ;	
}

conf::conf(const std::string &str) {

    if (str.empty())
        throw (std::runtime_error("error the config file seems to be empty"));
    if (isDir(str))
        throw (std::runtime_error("error the config file seems to be directory"));
    if (!isConf(str))
        throw (std::runtime_error("error the config file has not the extension .conf"));
    if (access(str.c_str(), R_OK))
		throw (std::runtime_error("error the config file does not provide read access"));
    std::ifstream ConfigFile(str.c_str());
    if (!ConfigFile.is_open()) {
        throw (std::runtime_error("error opening the config file"));
    }
    env()->webserv = new Webserv(this->_getRawConfig(ConfigFile));
}

conf::~conf() {

}

conf::conf(const conf &cpy) {
    *this = cpy;
}

conf &conf::operator=(const conf &rhs){
	(void) rhs;
	return (*this);
}

int conf::getNbServer() {
    return (this->_nbServer);
}
