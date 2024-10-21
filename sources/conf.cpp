/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   conf.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: peanut <peanut@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 15:50:49 by skapersk          #+#    #+#             */
/*   Updated: 2024/10/21 17:56:59 by peanut           ###   ########.fr       */
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

std::string trim(const std::string &str) {
    size_t first = str.find_first_not_of(" \t");
    size_t last = str.find_last_not_of(" \t");
    return (first == std::string::npos) ? "" : str.substr(first, last - first + 1);
}

void   conf::_findServerBlock(std::string &line) {
    size_t start;
    
    start = line.find(trim(line));
    if (start != std::string::npos) {
        _found = true;
        _blockLevel = 1;
    }
}

void    conf::_parseLine(std::string &line) {
    if (line.find("{") != std::string::npos)
        _blockLevel++;
    if (line.find("}") != std::string::npos) {
        _blockLevel--;
        if (_blockLevel == 0)
            return;
    }
}

bool conf::_getRawConfig(std::ifstream &ConfigFile) {
    std::string line;

	while (std::getline(ConfigFile, line))
	{
		if (line.empty() || line[0] == '#' || line[0] == ';')
            continue ;
        this->_findServerBlock(line);
        if (this->_found)
            this->_parseLine(line);
    // std::istringstream	w(line);

	}
    if (!this->_found)
		throw (std::runtime_error("File is not containing a server block."));
    std::cout << _blockLevel;
    if (this->_blockLevel != 0)
		throw (std::runtime_error("the server block is not closed."));
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