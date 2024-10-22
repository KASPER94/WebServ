/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   conf.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: peanut <peanut@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 15:50:52 by skapersk          #+#    #+#             */
/*   Updated: 2024/10/22 21:01:07 by peanut           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <sstream>

class conf {
    private:
        std::vector<std::string> _data;
        bool _found;
        int _blockLevel;
        bool _getRawConfig(std::ifstream &ConfigFile);
        void _parseLine(std::string &line);
        void _parseLocation(std::string block);
        bool _findServerBlock(std::string &line);
        void _checkDuplicateServDecl(std::string &line);

        // GETTER
        void _getListen(std::vector<std::string> line);
        void _getHost(std::vector<std::string> line);
        void _getServerName(std::vector<std::string> line);
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
};