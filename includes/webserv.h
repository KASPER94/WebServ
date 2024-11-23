/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 13:40:34 by peanut            #+#    #+#             */
/*   Updated: 2024/11/23 15:37:12 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_H
# define WEBSERV_H

# include <fstream>
# include <sstream>
# include <fcntl.h>
# include <cstring>

# define MAX_EVENTS 10
# define BUFFER_SIZE 4096
# define EPOLL_TIMEOUT 5000
# define CLIENT_TIMEOUT 5

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
	RETURN_URI,
	UNKNOWN
};

enum HttpMethod {
	GET = 0,
	POST = 1,
	DELETE = 2,
	OTHER = 3
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
void convertAllowMethods(const std::vector<HttpMethod>& methods) ;
int		setsocknonblock(int sock);
void	rtrim(std::string &str);
void	ltrim(std::string &str);
std::string intToString(int number);
bool	childPath(std::string parent, std::string child);
std::string	getFullPath(std::string path);
std::string parseContentType(const std::string &cgiHeaders);

#endif
