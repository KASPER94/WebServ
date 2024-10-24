/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   conf.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 15:50:49 by skapersk          #+#    #+#             */
/*   Updated: 2024/10/24 12:26:13 by skapersk         ###   ########.fr       */
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

void conf::_parseLocation(std::string block) {
    std::cout << block << std::endl;
}

void    conf::_parseLine(std::string &line, Server	serv, std::vector<Server> allServ) {
    std::istringstream	word(line);
    std::string w;
    std::vector<std::string> line_trim;

    word >> w;
	if (!this->_blockLevel && w.compare("{") != 0) {
		throw std::runtime_error("Error: Invalid server block");
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
			this->_getServerName(line_trim);
			break;
		case HOST:
			this->_getHost(line_trim);
			break;
		case INDEX:
			this->_getIndex(line_trim);
			break;
		case ERROR_PAGE:
			this->_getErrorPage(line_trim);
			break;
		case CLIENT_MAX_BODY_SIZE:
			this->_getClientMaxBodySize(line_trim);
			break;
		case ALLOWED_METHODS:
			this->_getAllowedMethods(line_trim);
			break;
		case CGI_BIN:
			this->_getCgiBin(line_trim);
			break;
		case AUTOINDEX:
			this->_getAutoindex(line_trim);
			break;
		case UPLOAD_PATH:
			this->_getUploadPath(line_trim);
			break;
		case ROOT:
			this->_getRoot(line_trim);
			break;
		case RETURN:
			this->_getRedirection(line_trim);
			break;
		case CGI_EXTENSION:
			this->_getCgiExtension(line_trim);
			break;
		case CGI_PATH:
			this->_getCgiPath(line_trim);
			break;
		case CLOSE_BRACKET:
			allServ.push_back(serv);
			this->_found = false;
			return;
		case UNKNOWN:
		default:
			throw std::runtime_error("Unknown directive: " + directive);
	}
}

std::vector<Server> conf::_getRawConfig(std::ifstream &ConfigFile) {
    std::string line;
	std::vector<Server> allServ;
	Server	serv;
    this->_found = false;
    int brace_count = 0;
    this->_nbServer = 0;

    while (std::getline(ConfigFile, line)) {
        if (line.empty() || line[0] == '#' || line[0] == ';')
            continue;
        if (!this->_found) {
            this->_findServerBlock(line);
        } else if (this->_found && line.find("location") != std::string::npos) {
            if (line.find("{") != std::string::npos)
                brace_count++;
            this->_parseLocation(line);
            if (line.find("}") != std::string::npos) {
                brace_count--;
                if (brace_count == 0)
                    continue;
            }
        } else if (this->_found && brace_count > 0) {
            this->_parseLocation(line);
            if (line.find("}") != std::string::npos) {
                brace_count--;
                if (brace_count == 0)
                    continue;
            }
        } else if (this->_found) {
            this->_parseLine(line, serv, allServ);
        }
    }
    if (brace_count != 0)
        throw std::runtime_error("Unclosed bracket");
    if (!this->_found && !this->_nbServer)
        throw std::runtime_error("File is not containing a server block.");
    return allServ;
}


// bool conf::_getRawConfig(std::ifstream &ConfigFile) {
//     std::string line;
//     this->_found = false;
//     bool in = false;

// 	while (std::getline(ConfigFile, line))
// 	{
// 		if (line.empty() || line[0] == '#' || line[0] == ';')
//             continue;
//         if (!this->_found)
//             this->_findServerBlock(line);
//         else if (this->_found && line.find("location") == 1) {
//             if (line.find("{") != std::string::npos && line.find("}") == std::string::npos)
//                 in = false;
//             else if (line.find("{") == std::string::npos)
//                 in = true;
//             this->_parseLocation(line);
//         }
//         else if (this->_found && in == true) {
//             std::cout << "+++" << in << std::endl;

//             this->_parseLocation(line);
//             if (line.find("}") != std::string::npos)
//                 in = false;
//         }
//         else if (this->_found)
//             this->_parseLine(line);
// 	}
//     if (line != "}")
// 		throw std::runtime_error("Unclosed bracket");
//     if (!this->_found)
// 		throw (std::runtime_error("File is not containing a server block."));
//     return (true);
// }

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
