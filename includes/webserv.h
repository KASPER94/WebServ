/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 13:40:34 by peanut            #+#    #+#             */
/*   Updated: 2024/12/09 01:23:08 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef WEBSERV_H
# define WEBSERV_H

# include <fstream>
# include <sstream>
# include <fcntl.h>
# include <cstring>
# include <sys/wait.h>
#include <sys/stat.h>
# include <errno.h>
# include <set>

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

enum LogLevel
{
	DEBUG,
	INFO,
	ERROR,
};

std::string	getCurrTime();
void		logMsg(LogLevel level, std::string msg);

template <typename T>
std::string toString(T value)
{
	std::stringstream ss;
	ss << value;
	return ss.str();
}

# include "Server.hpp"
# include "Webserv.hpp"
class Webserv;
class Server;

typedef struct s_env {
	Webserv	*webserv;
	char **envp;
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
std::string	strFromCharVec(size_t len, std::vector<char> &vec);
std::vector<std::string> split_trim(std::string str, std::string needle);
size_t	findInCharVec(std::string str, std::vector<char> &vec);
std::string str_split_trim(const std::string& str, const std::string& needle);


#endif
