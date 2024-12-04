/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrigny <yrigny@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 13:34:08 by peanut            #+#    #+#             */
<<<<<<< HEAD
/*   Updated: 2024/12/05 11:36:23 by skapersk         ###   ########.fr       */
=======
/*   Updated: 2024/12/04 16:45:40 by yrigny           ###   ########.fr       */
>>>>>>> 1827865 (adding logMsg() function)
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

std::string intToString(int number) {
	std::stringstream ss;
	ss << number;
	return ss.str();
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
		return RETURN_URI;
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

std::vector<std::string> split_trim_path(std::string str) {
    std::vector<std::string>    split;
    std::istringstream	word(str);
    std::string w;

    while (word >> w) {
		if (w == "/")
			continue ;
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

<<<<<<< HEAD
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
std::string	getFullPath(std::string path) {
	std::vector<std::string>	sub;
	std::vector<std::string>	new_path;
	std::string			full_path = "/";
	bool			last = path[path.length() - 1] == '/';

	sub = split_trim_path(path);
	for (std::vector<std::string>::iterator it = sub.begin(); it != sub.end(); it++) {
		if (*it == "..") {
			if (new_path.size() > 0)
				new_path.pop_back();
		} else if (*it != ".")
			new_path.push_back(*it);
	}
	for (std::vector<std::string>::iterator it = new_path.begin(); it != new_path.end(); it++) {
		full_path += *it;
		if ((it + 1) != new_path.end())
			full_path += "/";
	}
	if (last)
		full_path += "/";
	return (full_path);
}

bool	childPath(std::string parent, std::string child) {
	if (child.length() <= parent.length())
		return (false);
	if (child.find(parent) != 0)
		return (false);
	return (true);
}

std::string parseContentType(const std::string &cgiHeaders) {
    // Look for the "Content-Type" header in the CGI response
    size_t start = cgiHeaders.find("Content-Type:");
    if (start == std::string::npos) {
        return "text/html";
    }
    start += 13;
    while (start < cgiHeaders.size() && (cgiHeaders[start] == ' ' || cgiHeaders[start] == '\t')) {
        ++start;
    }
    size_t end = cgiHeaders.find("\r\n", start);
    if (end == std::string::npos) {
        end = cgiHeaders.size();
    }
    return cgiHeaders.substr(start, end - start);
}

=======
std::string	getCurrTime()
{
	tzset();
	char		dt[64];
	time_t		now = time(0);
	struct tm*	localTime = localtime(&now);
	strftime(dt, sizeof(dt), "%Y-%m-%d %H:%M:%S", localTime);
	return dt;
}

void	logMsg(LogLevel level, std::string msg)
{
	const std::string levelStr[3] = {"[DEBUG]", "[INFO] ", "[ERROR]"};
	std::string	currTime = getCurrTime();
	std::cout << currTime << " " << levelStr[level] << " " << msg << std::endl;
}
>>>>>>> 1827865 (adding logMsg() function)
