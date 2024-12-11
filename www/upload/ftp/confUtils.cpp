/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   confUtils.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 18:46:09 by peanut            #+#    #+#             */
/*   Updated: 2024/12/04 16:58:47 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "conf.hpp"

int    conf::_getListen(std::vector<std::string> line) {
	if (line.size() != 2)
		throw std::runtime_error("Error: listen directive requires exactly one argument (port)");

	const std::string& portStr = line[1];
	if (portStr[portStr.size() - 1] != ';')
			throw std::runtime_error("Error: missing ';'");
	for (size_t i = 0; i < portStr.size()- 1; ++i) {
		if (!std::isdigit(portStr[i])) {
			throw std::runtime_error("Error: listen directive requires a valid port number");
		}
	}

	std::istringstream nb(portStr);
	int port;
	nb >> port;
	if (nb.fail() || port < 0 || port > 65535) {
		throw std::runtime_error("Error: listen directive requires a port number between 0 and 65535");
	}
	return port;
}

std::string    conf::_getServerName(std::vector<std::string> line) {
	if (line.size() != 2)
		throw std::runtime_error("Error: server_name directive requires exactly one argument (server_name)");
	if (line[1][line[1].size() - 1] != ';')
		throw std::runtime_error("Error: missing ';'");
	std::string serverName = line[1].substr(0, line[1].size() - 1);
	if (serverName.empty())
		throw std::runtime_error("Error: missing server_name");

	return serverName;
}

bool allDigit(std::string &seg) {
	if (seg.empty())
		return false;
	for (size_t i = 0; i < seg.size(); ++i) {
		if (!isdigit(seg[i])) {
			return false;
		}
	}
	int num = atoi(seg.c_str());
	return (num >= 0 && num <= 255);
}

bool isValidIpAdd(std::string &ip) {
	int	numDots = 0;
	std::string seg;

	for (size_t i = 0; i < ip.length(); i++){
		if (ip[i] == '.') {
			numDots++;
			if (seg.empty() || !allDigit(seg) || atoi(seg.c_str()) > 255) {
				return (false);
			}
			seg.clear();
		}
		else if (isdigit(ip[i])) {
			seg += ip[i];
			if (seg.size() > 3)
				return (false);
		}
		else
			return (false);
	}
	if (seg.empty() || !allDigit(seg) || atoi(seg.c_str()) > 255)
		return (false);
	return (numDots == 3);
}

bool isValidHostname(std::string &host) {
	if (host.empty() || host.length() > 63)
		return (false);
	for (size_t i = 0; i < host.length(); i++) {
		if (!isalnum(host[i]) && host[i] != '-')
			return false;
		if ((i == 0 || i == host.length() - 1) && host[i] == '-')
		return (false);
	}
	return (true);
}

std::string    conf::_getHost(std::vector<std::string> line) {
	if (line.size() != 2)
		throw std::runtime_error("Error: host directive requires exactly one argument (host)");
	if (line[1][line[1].size() - 1] != ';')
		throw std::runtime_error("Error: missing ';'");
	std::string host = line[1].substr(0, line[1].size() - 1);
	if (!isValidIpAdd(host)) {
		if (!isValidHostname(host))
			throw std::runtime_error("Error: invalid host name (must be a valid IP or hostname)");
	}
	return host;
}

std::map<int, std::string>	conf::_getErrorPage(std::vector<std::string> line) {
	std::string errorPage;
	std::map<int, std::string> ErrorData;
	int			errorCode;

	if (line.size() != 3)
		throw std::runtime_error("Error: error_page directive requires exactly one argument (error_page)");
	if (line[2][line[2].size() - 1] != ';')
		throw std::runtime_error("Error: missing ';'");
	errorPage = line[2].substr(0, line[2].size() - 1);
	if (errorPage.empty() || allDigit(line[1]))
		throw(std::runtime_error("Error: error_page directive"));
	errorCode = atoi(line[1].c_str());
	ErrorData.insert(std::make_pair(errorCode, errorPage));
	return (ErrorData);
}

size_t    conf::_getClientMaxBodySize(std::vector<std::string> line) {
	size_t clientMaxBodysize;

	if (line.size() != 2)
		throw std::runtime_error("Error: client_max_body_size directive requires exactly one argument (client_max_body_size)");
	if (line[1][line[1].size() - 1] != ';')
		throw std::runtime_error("Error: missing ';'");
	if (allDigit(line[1]))
		throw(std::runtime_error("Error: client_max_body_size directive"));
	clientMaxBodysize = atoi(line[1].c_str());
	return (clientMaxBodysize);
}

std::vector<std::string>    conf::_getAllowedMethods(std::vector<std::string> line) {
	std::vector<std::string> methods;
	std::string method;

	if (line.size() < 2)
		throw std::runtime_error("Error: allowedMethods directive requires at least one method");
	std::string lastMethod = line.back();
	if (lastMethod[lastMethod.size() - 1] != ';')
		throw std::runtime_error("Error: missing ';'");

	for (size_t i = 1; i < line.size(); i++) {
		method = line[i].substr(0, line[i].size());
		method.erase(method.find_last_not_of(" \n\r\t;") + 1);
		method.erase(0, method.find_first_not_of(" \n\r\t"));
		if (method == "GET") {
			methods.push_back("GET");
		} else if (method == "POST") {
			methods.push_back("POST");
		} else if (method == "DELETE") {
			methods.push_back("DELETE");
		} else {
			throw std::runtime_error("Error: unsupported method in allowedMethods directive");
		}
	}
	return (methods);
}

// void    conf::_getRedirection(std::vector<std::string> line) {
//     std::vector<std::string>::iterator it;

//     it = line.begin();
//     for (; it != line.end(); it++) {
//         std::cout << *it << std::endl;
//     }
// }

// void    conf::_getCgiExtension(std::vector<std::string> line) {
//     std::vector<std::string>::iterator it;

//     it = line.begin();
//     for (; it != line.end(); it++) {
//         std::cout << *it << std::endl;
//     }
// }

// void    conf::_getCgiPath(std::vector<std::string> line) {
//     std::vector<std::string>::iterator it;

//     it = line.begin();
//     for (; it != line.end(); it++) {
//         std::cout << *it << std::endl;
//     }
// }

// void    conf::_getCgiBin(std::vector<std::string> line) {
//     std::vector<std::string>::iterator it;

//     it = line.begin();
//     for (; it != line.end(); it++) {
//         std::cout << *it << std::endl;
//     }
// }

std::string conf::_getRoot(const std::vector<std::string> &line) {
	if (line.size() != 2)
		throw std::runtime_error("Error: root directive requires exactly one argument (root)");
	if (line[1][line[1].size() - 1] != ';')
		throw std::runtime_error("Error: missing ';'");
	std::string root = line[1].substr(0, line[1].size() - 1);
	if (root.empty())
		throw std::runtime_error("Error: missing root");

	return root;
}

bool conf::_getAutoindex( std::vector<std::string> &line) {
	if (line.size() != 2)
		throw std::runtime_error("Error: autoindex directive requires exactly one argument");

	if (line[1][line[1].size() - 1] != ';')
		throw std::runtime_error("Error: missing ';'");
	std::string value = line[1].substr(0, line[1].size() - 1);

	if (value == "on")
		return true;
	else if (value == "off")
		return false;
	else
		throw std::runtime_error("Error: invalid value for autoindex (must be 'on' or 'off')");
}

std::string conf::_getIndexLoc(std::vector<std::string> line) {
    if (line.size() != 2) // Attente de exactement un argument + le mot-clé `index`
        throw std::runtime_error("Error: 'index' directive requires exactly one argument.");

    std::string index = line[1];

    if (index[index.size() - 1] != ';') // Vérification du point-virgule final
        throw std::runtime_error("Error: missing ';' at the end of 'index' directive.");

    // Retirer le point-virgule final
    index = index.substr(0, index.size() - 1);

    // Vérification du contenu non vide après traitement
    if (index.empty())
        throw std::runtime_error("Error: missing index value in 'index' directive.");

    // Nettoyage des espaces superflus
    size_t start = 0;
    while (start < index.size() && std::isspace(index[start]))
        start++;
    size_t end = index.size();
    while (end > start && std::isspace(index[end - 1]))
        end--;

    index = index.substr(start, end - start);

    // Vérification de validité après nettoyage
    if (index.empty())
        throw std::runtime_error("Error: invalid or empty index value in 'index' directive.");

    return index;
}


std::vector<std::string> conf::_getIndex(std::vector<std::string> line) {
	if (line.size() < 2)
		throw std::runtime_error("Error: 'index' directive requires at least one argument.");
	std::string indices_str;
	unsigned long i = 1;
	while (line.size() > i) {
		indices_str += line[i];
		i++;
	}
	if (indices_str[indices_str.size() - 1] != ';')
		throw std::runtime_error("Error: missing ';' at the end of 'index' directive.");
	indices_str = indices_str.substr(0, indices_str.size() - 1);
	if (indices_str.empty())
		throw std::runtime_error("Error: missing index values in 'index' directive.");

	std::vector<std::string> indices;
	std::istringstream iss(indices_str);
	std::string token;

	while (std::getline(iss, token, ',')) {
		size_t start = 0;
		while (start < token.size() && std::isspace(token[start]))
			start++;
		size_t end = token.size();
		while (end > start && std::isspace(token[end - 1]))
			end--;
		token = token.substr(start, end - start);
		if (token.empty())
			throw std::runtime_error("Error: empty index name found in 'index' directive.");
		indices.push_back(token);
	}
	if (indices.empty())
		throw std::runtime_error("Error: no valid indices provided in 'index' directive.");
	return indices;
}

std::map<int, std::string> conf::_getRedirection(std::vector<std::string> line) {
    if (line.size() != 3)
        throw std::runtime_error("Error: redirection directive requires exactly one argument (redirection).");
    std::string return_str;
    unsigned long i = 0;
    while (line.size() > i) {
        return_str += line[i];
        i++;
    }
    if (return_str[return_str.size() - 1] != ';')
        throw std::runtime_error("Error: missing ';' at the end of 'index' directive.");
    std::string redirection = line[2].substr(0, line[2].size() - 1);
    for (std::string::const_iterator it = line[1].begin(); it != line[1].end(); ++it) {
        if (!isdigit(*it))
            throw std::runtime_error("Error: invalid HTTP code in redirection directive.");
    }
    int code = std::atoi(line[1].c_str());
    if (code < 200 || code > 599) {
        throw std::runtime_error("Error: unsupported HTTP code in redirection directive.");
    }
    if (redirection.empty())
        throw std::runtime_error("Error: [emerg] invalid return code \"700\" in /etc/nginx/nginx.conf:XX");
    std::map<int, std::string> ret;
    ret.insert(std::make_pair(code, redirection));
    return ret;
}


// std::string conf::_getCgiExtension(std::vector<std::string> line) {
// 	if (line.size() != 2)
// 		throw std::runtime_error("Error: cgi_extension directive requires exactly one argument");
// 	if (line[1][line[1].size() - 1] != ';')
// 		throw std::runtime_error("Error: missing ';'");
// 	std::string extension = line[1].substr(0, line[1].size() - 1);
// 	if (extension.empty())
// 		throw std::runtime_error("Error: missing extension");
// 	return extension;
// }

std::vector<std::string> conf::_getCgiExtensions(std::vector<std::string> line) {
	if (line.size() < 2)
		throw std::runtime_error("Error: cgi_extension directive requires at least one argument");
	if (line[line.size() - 1][line[line.size() - 1].size() - 1] != ';')
		throw std::runtime_error("Error: missing ';' at the end of the directive");
	line[line.size() - 1] = line[line.size() - 1].substr(0, line[line.size() - 1].size() - 1);
	std::vector<std::string> extensions;
	for (size_t i = 1; i < line.size(); ++i) {
		if (line[i].empty())
			throw std::runtime_error("Error: missing extension");
		extensions.push_back(line[i]);
	}
	return extensions;
}

std::string conf::_getUploadPath(std::vector<std::string> line) {
	if (line.size() != 2)
		throw std::runtime_error("Error: upload_path directive requires exactly one argument");
	if (line[1][line[1].size() - 1] != ';')
		throw std::runtime_error("Error: missing ';'");
	std::string uploadPath = line[1].substr(0, line[1].size() - 1);
	if (uploadPath.empty())
		throw std::runtime_error("Error: missing uploadPath");
	return uploadPath;
}

std::string conf::_getCgiBin(std::vector<std::string> line) {
	if (line.size() != 2)
		throw std::runtime_error("Error: cgi_bin directive requires exactly one argument");
	if (line[1][line[1].size() - 1] != ';')
		throw std::runtime_error("Error: missing ';'");
	std::string cgiBin = line[1].substr(0, line[1].size() - 1);
	if (cgiBin.empty())
		throw std::runtime_error("Error: missing cgiBin");
	return cgiBin;
}
