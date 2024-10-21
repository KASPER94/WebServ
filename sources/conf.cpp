/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   conf.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: peanut <peanut@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 15:50:49 by skapersk          #+#    #+#             */
/*   Updated: 2024/10/21 22:08:12 by peanut           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "conf.hpp"

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

void   conf::_findServerBlock(std::string &line) {
    std::istringstream	word(line);
    std::string w;
    this->_blockLevel = 0;

    while (word >> w) { 
        if (w.compare("server") == 0) {
            this->_found = true;
            if (word >> w && w == "{") {
                this->_blockLevel = 1;
                break ;
            }
        }
        else if (w == "server{") { 
            this->_found = true;
            this->_blockLevel = 1;
            break ;
        } else {
            this->_blockLevel = 0;
        }
    }
}

void    conf::_parseLine(std::string &line) {
    std::istringstream	word(line);
    std::string w;

    word >> w;
	if (!this->_blockLevel && w.compare("{") != 0) {
		throw std::runtime_error("Invalid server block");
    }
    else
        this->_blockLevel = 1;
}

bool conf::_getRawConfig(std::ifstream &ConfigFile) {
    std::string line;
    this->_found = false;

	while (std::getline(ConfigFile, line))
	{
		if (line.empty() || line[0] == '#' || line[0] == ';')
            continue;
        if (!this->_found)
            this->_findServerBlock(line);
        else if (this->_found)
            this->_parseLine(line);
	}
    if (line != "}")
		throw std::runtime_error("Unclosed bracket");
    if (!this->_found)
		throw (std::runtime_error("File is not containing a server block."));
    return (true);
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
    this->_getRawConfig(ConfigFile);

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