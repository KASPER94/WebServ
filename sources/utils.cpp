/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 13:34:08 by peanut            #+#    #+#             */
/*   Updated: 2024/10/31 18:18:39 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <vector>
#include "webserv.h"

void	ltrim(std::string &str) {
	int	i = 0;

	if (str.length() == 0)
		return ;
	while (isspace(str[i]))
		i++;
	str = str.substr(i, str.length());
}

void	rtrim(std::string &str) {
	int	i = str.length() - 1;

	if (str.length() == 0)
		return ;
	while (isspace(str[i]))
		i--;
	str = str.substr(0, i + 1);
}

void convertAllowMethods(const std::vector<HttpMethod>& methods) {
    std::stringstream result;
	// std::vector<HttpMethod>::const_iterator it;

	std::vector<HttpMethod>::const_iterator it;
	it = methods.begin();
	for (; it != methods.end(); it++){
		std::cout << "coucou" << std::endl;
		std::cout << *it << std::endl;
		if (*it == GET)
			std::cout << "+++GET+++" << std::endl;
		else if (*it == POST)
			std::cout << "++POST++" << std::endl;
		else if (*it == DELETE)
			std::cout << "+DELETE+" << std::endl;
	}
}

Directive getDirective(const std::string &directive) {
	if (directive == "server")
		return SERVER;
	if (directive == "listen")
		return LISTEN;
	if (directive == "server_name")
		return SERVER_NAME;
	if (directive == "host")
		return HOST;
	if (directive == "index")
		return INDEX;
	if (directive == "error_page")
		return ERROR_PAGE;
	if (directive == "client_max_body_size")
		return CLIENT_MAX_BODY_SIZE;
	if (directive == "allowedMethods")
		return ALLOWED_METHODS;
	if (directive == "cgi_bin")
		return CGI_BIN;
	if (directive == "autoindex")
		return AUTOINDEX;
	if (directive == "upload_path")
		return UPLOAD_PATH;
	if (directive == "root")
		return ROOT;
	if (directive == "return")
		return RETURN;
	if (directive == "cgi_extension")
		return CGI_EXTENSION;
	if (directive == "cgi_path")
		return CGI_PATH;
	if (directive == "}")
		return CLOSE_BRACKET;
	return UNKNOWN;
}

t_env *env() {
	static t_env e;
	return (&e);
}

std::vector<std::string> split_trim_conf(std::string str) {
    std::vector<std::string>    split;
    std::istringstream	word(str);
    std::string w;

    while (word >> w) {
        split.push_back(w);
    }
    return (split);
}

unsigned long convertIpToUnsignedLong(const std::string &ip) {
	in_addr_t ipAddress = inet_addr(ip.c_str());

	if (ipAddress == INADDR_NONE) {
		throw std::runtime_error("Invalid IP address format");
	}
	return htonl(ipAddress);
}

int		setsocknonblock(int sock)
{
	int flag;

	flag = fcntl(sock, F_GETFL, 0);
	if (flag < 0)
	{
		perror("Fcntl (F_GETFL) failed");
		return (-1);
	}
	if (fcntl(sock, F_SETFL, flag | O_NONBLOCK) < 0)
	{
		perror("Fcntl (F_SETFL) failed");
		return (-1);
	}
	return (1);
}