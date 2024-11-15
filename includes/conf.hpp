/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   conf.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 15:50:52 by skapersk          #+#    #+#             */
/*   Updated: 2024/11/15 10:49:53 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONF_HPP
# define CONF_HPP

# include <sstream>
# include <iostream>
# include <fstream>
# include <sys/stat.h>
# include <unistd.h>
# include <vector>
# include <map>
# include "webserv.h"
# include <cstring>
# include <cctype>

class conf {
	private:
		std::vector<std::string> _data;
		bool _found;
		bool _reset;
		int _nbServer;
		int _blockLevel;
		std::vector<Server> _getRawConfig(std::ifstream &ConfigFile);
		void _parseLine(std::string &line, Server &serv, std::vector<Server> &allServ);
		void _parseLocation(std::string line, Server &serv, std::ifstream &ConfigFile);
		bool _findServerBlock(std::string &line);
		void _checkDuplicateServDecl(std::string &line);

		// GETTER
		int _getListen(std::vector<std::string> line);
		std::string _getHost(std::vector<std::string> line);
		std::string  _getServerName(std::vector<std::string> line);
		std::vector<std::string>		_getIndex(std::vector<std::string> line);
		// void _getLocation(std::vector<std::string> line);
		std::map<int, std::string> _getErrorPage(std::vector<std::string> line);
		size_t _getClientMaxBodySize(std::vector<std::string> line);
		std::vector<std::string> *_getAllowedMethods(std::vector<std::string> line);
		std::string _getRoot(const std::vector<std::string> &line);

		bool _getAutoindex(std::vector<std::string> &line);
		std::string _getUploadPath(std::vector<std::string> line);
		// void _getRoot(std::vector<std::string> line);
		std::map<int,std::string> _getRedirection(std::vector<std::string> line);
		std::string _getCgiExtension(std::vector<std::string> line);
		std::string _getCgiPath(std::vector<std::string> line);
		std::string _getCgiBin(std::vector<std::string> line);

	public:
		conf(const std::string &str);
		~conf();
		conf(const conf &cpy);

		conf &operator=(const conf &rhs);
		int getNbServer();
};

#endif
