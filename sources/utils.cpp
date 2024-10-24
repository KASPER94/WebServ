/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 13:34:08 by peanut            #+#    #+#             */
/*   Updated: 2024/10/24 10:42:41 by skapersk         ###   ########.fr       */
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

