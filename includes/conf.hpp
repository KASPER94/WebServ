/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   conf.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: peanut <peanut@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 15:50:52 by skapersk          #+#    #+#             */
/*   Updated: 2024/10/22 14:04:44 by peanut           ###   ########.fr       */
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
        bool _findServerBlock(std::string &line);
        void _checkDuplicateServDecl(std::string &line);
        void _getListen(std::vector<std::string> line);
    public:
        conf(const std::string &str);
        ~conf();
        conf(const conf &cpy);

        conf &operator=(const conf &rhs);
};