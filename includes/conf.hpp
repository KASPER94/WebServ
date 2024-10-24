/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   conf.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 15:50:52 by skapersk          #+#    #+#             */
/*   Updated: 2024/10/24 17:13:49 by skapersk         ###   ########.fr       */
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
		void _parseLocation(std::string block);
		bool _findServerBlock(std::string &line);
		void _checkDuplicateServDecl(std::string &line);

		// GETTER
		int _getListen(std::vector<std::string> line);
		std::string _getHost(std::vector<std::string> line);
		std::string  _getServerName(std::vector<std::string> line);
		void _getIndex(std::vector<std::string> line);
		// void _getLocation(std::vector<std::string> line);
		void _getErrorPage(std::vector<std::string> line);
		void _getClientMaxBodySize(std::vector<std::string> line);
		void _getAllowedMethods(std::vector<std::string> line);
		void _getAutoindex(std::vector<std::string> line);
		void _getUploadPath(std::vector<std::string> line);
		void _getRoot(std::vector<std::string> line);
		void _getRedirection(std::vector<std::string> line);
		void _getCgiExtension(std::vector<std::string> line);
		void _getCgiPath(std::vector<std::string> line);
		void _getCgiBin(std::vector<std::string> line);

	public:
		conf(const std::string &str);
		~conf();
		conf(const conf &cpy);

		conf &operator=(const conf &rhs);
		int getNbServer();
};

#endif
