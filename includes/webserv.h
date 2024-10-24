/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 13:40:34 by peanut            #+#    #+#             */
/*   Updated: 2024/10/24 17:51:11 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_H
# define WEBSERV_H

# include <fstream>
# include <sstream>

enum Directive {
	SERVER,
	LISTEN,
	SERVER_NAME,
	HOST,
	INDEX,
	ERROR_PAGE,
	CLIENT_MAX_BODY_SIZE,
	ALLOWED_METHODS,
	CGI_BIN,
	AUTOINDEX,
	UPLOAD_PATH,
	ROOT,
	RETURN,
	CGI_EXTENSION,
	CGI_PATH,
	CLOSE_BRACKET,
	UNKNOWN
};

enum HttpMethod {
	GET,
	POST,
	DELETE,
	OTHER
};

# include "Server.hpp"
# include "Webserv.hpp"
class Webserv;
class Server;

typedef struct s_env {
	Webserv	*webserv;
}	t_env;

t_env *env();
std::vector<std::string> split_trim_conf(std::string str);
Directive getDirective(const std::string &directive);
unsigned long convertIpToUnsignedLong(const std::string &ip);

#endif
