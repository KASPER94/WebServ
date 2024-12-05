/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 14:51:58 by skapersk          #+#    #+#             */
/*   Updated: 2024/12/05 13:32:02 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "HttpResponse.hpp"

HttpResponse::HttpResponse(): _client(NULL) {}

HttpResponse::HttpResponse(Client *client):
	_client(client) {
}

HttpResponse::HttpResponse(const HttpResponse &cpy) {
	*this = cpy;
}

HttpResponse::~HttpResponse() {}

HttpResponse &HttpResponse::operator=(const HttpResponse &rhs) {
	this->_client = rhs._client;
	return (*this);
}

void HttpResponse::sendHeader() {
    std::string header = "HTTP/1.1 " + intToString(this->_statusCode) + " ";

    std::string statusDescription;
    switch (this->_statusCode) {
        case 200: statusDescription = "OK"; break;
        case 301: statusDescription = "Moved Permanently"; break;
        case 400: statusDescription = "Bad Request"; break;
        case 403: statusDescription = "Forbidden"; break;
        case 404: statusDescription = "Not Found"; break;
        case 413: statusDescription = "Payload Too Large"; break;
        case 500: statusDescription = "Internal Server Error"; break;
        default:  statusDescription = "Unknown Status"; break;
    }
    header += statusDescription + "\r\n";

    // Ajout en-têtes HTTP à partir de _headers
    for (std::map<std::string, std::string>::iterator it = this->_headers.begin(); it != this->_headers.end(); ++it) {
        header += it->first + ": " + it->second + "\r\n";
    }

    // Terminer les en-têtes avec une ligne vide
    header += "\r\n";
	std::cout << header << std::endl;

    int bytes = send(this->_client->getFd(), header.c_str(), header.length(), 0);
	this->checkSend(bytes);
}

void	HttpResponse::checkSend(int bytes) {
	if (bytes <= 0) {
		this->_client->setError();
	}
}

bool HttpResponse::hasAccess(std::string &uri, bool &isDir) {
    struct stat	s;

	if (stat(uri.c_str(), &s) == 0) {
		if (s.st_mode & S_IFDIR) {
			isDir = true;
			if (uri[uri.length() - 1] != '/') {
				this->movedPermanently(this->getRequest()->returnPATH() + "/");
				return (false);
			}
		}
		else
			isDir = false;
	}
	if (isDir) {
		for (std::vector<std::string>::iterator it = this->_indexes.begin(); it != this->_indexes.end(); it++) {
			if (*it == "")
				continue;
			if (access((uri + *it).c_str(), F_OK) != -1) {
				if (access((uri + *it).c_str(), R_OK) == -1) {
					this->handleError(403, "La requête est invalide. Veuillez vérifier les paramètres et le format de votre demande.");
					return (false);
				}
				uri += *it;
				isDir = false;
				break;
			}
		}
		if (saveLoc.inLoc) {
			std::string index = saveLoc.loc.getIndex();
			if (!index.empty() && access((uri + index).c_str(), F_OK) != -1) {
				if (access((uri + index).c_str(), R_OK) == -1) {
					this->handleError(403, "La requête est invalide. Veuillez vérifier les paramètres et le format de votre demande.");
					return (false);
				}
				uri += index;
				isDir = false;
				return true;
			}
		}
	}
	return (true);
    // return access(uri.c_str(), F_OK) != -1 && access(uri.c_str(), R_OK) != -1;
}

bool	HttpResponse::methodAllowed(enum HttpMethod method) {
	std::vector<std::string>::iterator it = this->_allowedMethod.begin();
	(void)method;
	if (it == this->_allowedMethod.end())
		if (*it == this->getRequest()->HttpMethodTostring())
			return (true);
	for (; it != this->_allowedMethod.end(); it++) {
		if (*it == this->getRequest()->HttpMethodTostring())
			return (true);
	}
	return (false);
}

void	HttpResponse::tryDeleteFile(std::string &uri) {
	std::string	root;

	if (this->_isLocation) {
		root = this->_root;
	} else
		root = this->getServer()->getRoot();
	if (!childPath(getFullPath(root), getFullPath(uri)))
		this->handleError(403, "La requête est invalide. Veuillez vérifier les paramètres et le format de votre demande.");
	else {
		if (access(uri.c_str(), F_OK) != -1) {
			if (access(uri.c_str(), R_OK) == -1) {
				this->handleError(403, "La requête est invalide. Veuillez vérifier les paramètres et le format de votre demande.");
				return ;
			}
		}
		if (remove(uri.c_str()) == 0)
			this->handleError(200, "Le fichier a été supprimé avec succès.");
		else
			this->handleError(204, "Le fichier n'existait pas ou n'a pas pu être supprimé.");
	}
}

void HttpResponse::createHeader() {
    this->_headers["Server"] = "Webserv/1.0";
	if (!this->_mime.empty()) {
		this->_headers["Content-Type"] = this->_mime;
	} else {
		this->_headers["Content-Type"] = "text/html";
	}
	//changer avec keepalive !!!!!
	this->_headers["Connection"] = "close";
}

void HttpResponse::sendDirectoryPage(std::string path) {
    DIR *dir;
    struct dirent *entry;

    // Ouvrir le répertoire spécifié
    if ((dir = opendir(path.c_str())) == NULL) {
        this->handleError(500, "Impossible d'ouvrir le répertoire : " + path);
        return;
    }

    // Début de la page HTML
    std::string body = "<html><head><title>Index of " + path + "</title></head>";
    body += "<body><h1>Index of " + path + "</h1><ul>";

    // Parcourir les entrées du répertoire
    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;

        // Ignorer les entrées spéciales "." et ".."
        if (name == "." || name == "..")
			continue;

        // Construire un lien pour chaque fichier ou répertoire
        std::string link = path;
        if (link.c_str()[link.size() - 1] != '/')
			link += "/";
        link += name;

        body += "<li><a href=\"" + name + "\">" + name + "</a></li>";
    }

    // Fin de la page HTML
    body += "</ul></body></html>";

    // Fermer le répertoire
    closedir(dir);

    // Stocker le corps de la réponse et sa longueur
    this->_body = body;
    this->_headers["Content-Length"] = intToString(body.size());
    this->_headers["Content-Type"] = "text/html";
}


void	HttpResponse::directoryListing(std::string path) {
	this->_statusCode = 200;
	this->_mime = Mime::getMimeType("html");
	this->createHeader();
	this->sendHeader();
	this->sendDirectoryPage(path);
}

int	HttpResponse::sendData(const void *data, int len) {
	const char	*ptr = static_cast<const char *>(data);
	int			bytes;

	// if (this->keepAlive() && !this->getClientError() && this->_cgiIndex == -1)
	// 	this->sendChunkSize(len);
	while (len > 0 && !this->_client->getError()) {
		bytes = send(this->_client->getFd(), ptr, len, 0);
		this->checkSend(bytes);
		ptr += bytes;
		len -= bytes;
	}
	// if (this->keepAlive() && !this->getClientError() && this->_cgiIndex == -1){
	// 	this->sendChunkEnd();
	// }
	return (0);
}

void HttpResponse::serveStaticFile(const std::string &uri) {
    std::ifstream file(uri.c_str(), std::ios::binary);

    if (!file.is_open()) {
		if (uri.find("favicon.ico") != std::string::npos) {
            this->_statusCode = 404;
            this->_mime = "image/x-icon";
            this->_headers["Content-Type"] = this->_mime;
            this->_headers["Content-Length"] = "0";
            this->createHeader();
            this->sendHeader();
            return;
        }
        this->handleError(500, "Failed to Open File");
        return;
    }
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::string ext = uri.substr(uri.find_last_of(".") + 1);
    this->_mime = Mime::getMimeType(ext);

    this->_statusCode = 200;
    this->_headers["Content-Type"] = this->_mime;
    this->_headers["Content-Length"] = intToString(fileSize);
    this->createHeader();
    this->sendHeader();

    char buffer[2048];
    while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
        this->sendData(buffer, file.gcount());
    }
    file.close();
}

bool HttpResponse::executeCGI(const std::string &uri) {
    char tempFileName[] = "/tmp/cgi_output_XXXXXX";
    int tempFd = mkstemp(tempFileName); // Create a secure temporary file
    if (tempFd == -1) {
        this->handleError(500, "CGI execution failed: unable to create temp file");
        return false;
    }

    this->_cgiTmpFile = tempFileName;

    pid_t pid = fork();
    if (pid < 0) {
        this->handleError(500, "CGI execution failed: fork error");
        close(tempFd);
        unlink(tempFileName); // Clean up temporary file
        return false;
    }

    if (pid == 0) {
        dup2(tempFd, STDOUT_FILENO); // Redirect CGI output to the temp file
        close(tempFd);               // Close the temp file descriptor in the child process
        if (execl(this->_cgiBin.c_str(), this->_cgiBin.c_str(), uri.c_str(), NULL) == -1) {
            perror("exec failed");
            exit(127); // Exit with a specific code indicating `execl` failure
        }
    } else {
        // Parent process: Wait for the child and handle the output
        close(tempFd); // Close the temp file descriptor in the parent process

        int status;
        waitpid(pid, &status, 0); // Wait for the child process to finish

        if (WIFEXITED(status)) {
            int exitCode = WEXITSTATUS(status);
            if (exitCode != 0) {
                if (exitCode == 127) {
                    this->handleError(500, "CGI binary not found or execution failed");
                } else {
                    this->handleError(500, "CGI execution failed with error code: " + intToString(exitCode));
                }
                unlink(tempFileName); // Clean up the temporary file
                return false;
            }
        } else {
            this->handleError(500, "CGI process terminated abnormally");
            unlink(tempFileName); // Clean up the temporary file
            return false;
        }
    }

    // Success: Output written to the temporary file
    return true;
}

// bool HttpResponse::executeCGI(const std::string &uri) {
//     int pipe_fd[2];
//     if (pipe(pipe_fd) == -1) {
//         this->handleError(500, "CGI execution failed: pipe error");
//         return false;
//     }
//     pid_t pid = fork();
//     if (pid < 0) {
//         this->handleError(500, "CGI execution failed: fork error");
//         return false;
//     } else if (pid == 0) {
//         dup2(pipe_fd[1], STDOUT_FILENO);
//         close(pipe_fd[0]);
//         close(pipe_fd[1]);
//         execl(this->_cgiBin.c_str(), this->_cgiBin.c_str(), uri.c_str(), NULL);
// 	// std::cerr << "LOL" << uri << " and " << _cgiBin.c_str() <<std::endl;
//         exit(1); // Si exec échoue ATTENTION A METTRE LE BON CODE ERREUR SI CGIBIN EXISTE PAS
//     } else {
//         // Parent: Lire la sortie du CGI
//         close(pipe_fd[1]);              // Ferme l'écriture
//         char buffer[4096];
//         ssize_t bytesRead;
//         std::string output;

//         while ((bytesRead = read(pipe_fd[0], buffer, sizeof(buffer) - 1)) > 0) {
//             buffer[bytesRead] = '\0';
//             output += buffer;
//         }
//         close(pipe_fd[0]);

//         if (output.empty()) {
//             this->handleError(500, "CGI execution failed: no output");
//             return false;
//         }

//         this->_body = output;           // Assigner la sortie du script au body
//         this->_headers["Content-Type"] = "text/html"; // Assurer un Content-Type correct
//         return true;
//     }
// }

void HttpResponse::handleCGI(std::string uri) {
    if (!executeCGI(uri)) {
        this->handleError(500, "CGI Execution Failed");
        return;
    }
    // Gérer le fichier temporaire produit par le CGI
    std::ifstream cgiOutput(this->_cgiTmpFile.c_str(), std::ios::binary);
    if (!cgiOutput.is_open()) {
        this->handleError(500, "Failed to Open CGI Output");
        return;
    }

    // Lire et analyser la sortie CGI
    std::stringstream buffer;
    buffer << cgiOutput.rdbuf();
    std::string cgiResponse = buffer.str();

    // Parse en-têtes CGI
    size_t headerEnd = cgiResponse.find("\r\n\r\n");
    if (headerEnd == std::string::npos) {
        this->handleError(500, "Malformed CGI Response");
        return;
    }
    this->_headers["Content-Type"] = parseContentType(cgiResponse.substr(0, headerEnd));
    this->_body = cgiResponse.substr(headerEnd + 4);

    // Créer l'en-tête HTTP
    this->_statusCode = 200;
    this->createHeader();
    this->sendHeader();
    this->sendData(this->_body.c_str(), this->_body.size());
    cgiOutput.close();
    return;
}

bool createDirectoriesRecursively(const std::string &path) {
    size_t pos = 0;

    while (pos != std::string::npos) {
        pos = path.find('/', pos + 1);
        std::string subPath = path.substr(0, pos);

        struct stat info;
        if (stat(subPath.c_str(), &info) != 0) {
            if (mkdir(subPath.c_str(), 0777) != 0 && errno != EEXIST) {
                std::cerr << "[DEBUG] Failed to create directory: " << subPath
                          << " with error: " << strerror(errno) << std::endl;
                return false;
            }
        } else if (!S_ISDIR(info.st_mode)) {
            std::cerr << "[DEBUG] Path exists but is not a directory: " << subPath << std::endl;
            return false;
        }
    }
    return true;
}

bool HttpResponse::handleUpload() {
	std::string uploadPath = this->_client->getServer()->getUploadPath();
	if (uploadPath.empty()) {
		std::cerr << "[DEBUG] No upload path defined in configuration." << std::endl;
		this->handleError(500, "Upload path not configured.");
		return false;
	}

	std::string Path = _root + uploadPath;
    // if (!createDirectoriesRecursively(Path)) {
    //     this->handleError(500, "Cannot create upload directory.");
    //     return false;
    // }

	const std::map<std::string, std::string> &files = this->_client->getRequest()->getFileData();
	for (std::map<std::string, std::string>::const_iterator it = files.begin(); it != files.end(); ++it) {
		std::string filePath = _root;
		if (uploadPath.c_str()[uploadPath.size() - 1] == '/')
			filePath += uploadPath + it->first;
		else
			filePath += uploadPath + "/" + it->first;
		// std::cerr << it->first << " ###" << std::endl;
		// std::ofstream file(filePath.c_str(), std::ios::binary);
       	std::ofstream file(filePath.c_str(), std::ios::binary | std::ios::trunc);
        if (!file.is_open()) {
			std::cerr << filePath << std::endl;
            std::cerr << "[DEBUG] Failed to create file: " << filePath
                      << " with error: " << strerror(errno) << std::endl;
            this->handleError(500, "Failed to create uploaded file.");
            return false;
        }
		file << it->second;
		file.close();
	}
	std::cout << "[DEBUG] Files uploaded successfully to " << uploadPath << std::endl;
	return true;
}

void HttpResponse::sendResponse() {
	if (this->getRequest()->tooLarge())
		return handleError(413, "Le contenu de la requête dépasse la taille maximale autorisée par le serveur.");
	if (!initializeResponse())
		return handleError(400, "La requête est invalide. Veuillez vérifier les paramètres et le format de votre demande.");
    std::string uri = this->getRequest()->returnPATH();
    bool isDir;
    if (!resolveUri(uri, isDir)) {
        return handleError(404, "Not Found");
    }
	if (!this->methodAllowed(this->getRequest()->getMethod())) {
        return handleError(405, "Method Not Allowed");
    }
	if (this->getRequest()->getMethod() == DELETE) {
		tryDeleteFile(uri);
		return ;
	}
	if (this->getRequest()->getMethod() == POST && !this->getRequest()->getFileData().empty()) {
		if (!this->handleUpload()) {
			return ;
		}
	}
	if (isDir) {
		if (this->_directoryListing)
			this->directoryListing(uri);
		else {
			handleError(404, "Not Found");
		}
		return ;
	}
	if  (_isCGI) {
		handleCGI(uri);
	}
	if (!isDir && !_isCGI) {
		this->serveStaticFile(uri);
		return;
	}

    // finalizeResponse();
	// if (this->getRequest()->getMethod() == GET) {
	// 	std::string body = "COUCOU WEBSERV !!!";
	// 	std::string headers = "HTTP/1.1 200 OK\r\n"
	// 						"Content-Type: text/plain\r\n"
	// 						"Content-Length: " + intToString(body.size()) + "\r\n"
	// 						"Connection: close\r\n"
    //               			"\r\n";
	// 	_response = headers + body;
	// } else {
	// 	_response = "HTTP/1.1 405 Method Not Allowed\r\n"
	// 				"Content-Length: 0\r\n"
	// 				"Connection: close\r\n"
	// 				"\r\n";
	// }
}

void	HttpResponse::setInfos() {
	this->_isLocation = false;
	std::map<std::string, Location*> locs = this->getServer()->returnLoc();

	if (!locs.empty()) {
		this->_isLocation = true;
		std::vector<std::string> *tmp = this->_client->getServer()->getUri();
		std::vector<std::string>::iterator it = tmp->begin();
		this->_uri = *it;
	}
	this->_uri = "";
	this->_root = this->getServer()->getRoot();
	this->_maxBodySize = this->getServer()->getClientMaxBody();
	this->_allowedMethod = *(this->getServer()->getAllowedMethods());
	this->_directoryListing = this->getServer()->getAutoindex();
	this->_errorPage = this->getServer()->getErrorPage();
	this->_returnURI = this->getServer()->getReturnUri();
	this->_uploadPath = this->getServer()->getUploadPath();
	this->_cgiBin = this->getServer()->getCgiBin();
	this->_cgiExt = this->getServer()->getCgiExtension();
	this->_indexes = this->getServer()->getIndexes();
}

bool HttpResponse::resolveUri(std::string &uri, bool &isDir) {
	std::string resolvePath;
	std::string location;
	bool follow = true;
	isDir = false;
	saveLoc.inLoc = false;
	_isCGI = false;

	if (_isLocation) {
        location = matchLocation(uri);
        if (!location.empty()) {
			saveLoc.inLoc = true;
            Location *loc = this->getServer()->getLocation(location);
			saveLoc.loc = *loc;
            resolvePath = loc->getRoot();
            if (resolvePath[resolvePath.size() - 1] != '/')
                resolvePath += '/';
            // resolvePath += uri.substr(location.size());
            if (!loc->getIndex().empty()) {
                std::string indexPath = resolvePath;
                if (indexPath[indexPath.size() - 1] != '/')
                    indexPath += '/';
                indexPath += loc->getIndex();
                if (access(indexPath.c_str(), F_OK) != -1) {
                    uri = indexPath;
                    isDir = false;
					const std::vector<std::string> &cgiExtensions = loc->getCgiExtension();
					if (!cgiExtensions.empty()) {
						for (std::vector<std::string>::const_iterator it = cgiExtensions.begin(); it != cgiExtensions.end(); ++it) {
							if (!it->empty() && uri.find(*it) != std::string::npos) {
								follow = true;
								_isCGI = true;
								this->_cgiBin = loc->getCgiBin();
								if (!loc->getAllowedMethods().empty()){
									this->_allowedMethod = loc->getAllowedMethods();
								}
								//allowed mthod
								return follow;
							}
						}
					} else
						return (true);
                }
            }
        } else {
			if (_root.c_str()[_root.size() - 1] == '/')
				resolvePath = _root;
			else
            	resolvePath = _root + uri;
		}
    }
	else {
		if (_root.c_str()[_root.size() - 1] == '/')
			resolvePath = _root;
		else
			resolvePath = _root + uri;
	}
	if (!hasAccess(resolvePath, isDir)) {
        follow = false;
    }
	for (std::vector<std::string>::iterator it = this->_cgiExt.begin(); it != this->_cgiExt.end(); it++) {
		if (*it != "" && (resolvePath.find(*it) != std::string::npos || resolvePath.find(*it) != std::string::npos)) {
			uri = resolvePath;
            // std::cout << "CGI Request Detected: " << resolvePath << std::endl;
			follow = true;
			_isCGI = true;
            return (follow);
		}
		// else if ((uri != "" && uri != "/") && !_isLocation) {
		// 	follow = false;
		// }
	}
	if (uri == "/")
		uri = resolvePath;
	else
		uri = _root + this->_client->getRequest()->returnPATH();

	// std::cout << "444 --> " << this->_client->getRequest()->returnPATH() << std::endl;
	// 	follow = true;
	return (follow);
}

std::string HttpResponse::matchLocation(std::string &requestUri) const {
    std::vector<std::string> *locations = this->_client->getServer()->getUri();
    std::string bestMatch = "";
    size_t bestMatchLength = 0;

    for (std::vector<std::string>::iterator it = locations->begin(); it != locations->end(); ++it) {
        const std::string &location = *it;
        if (requestUri.find(location) == 0 && location.length() > bestMatchLength) {
            bestMatch = location;
            bestMatchLength = location.length();
        }
    }
    return bestMatch;
}

bool	HttpResponse::initializeResponse() {
	this->setInfos();
	if (!this->getRequest()->isGood()) {
		this->handleError(400, "You do not have permission to access this resource.");
		return (false);
	}

	if (!_returnURI.empty()) {
        std::map<int, std::string>::iterator redirect = this->_returnURI.begin();
        if (redirect->first == 301) {
            this->movedPermanently(redirect->second);
        } else if (redirect->first >= 300 && redirect->first < 400) {
            this->handleRedirect(redirect->first, redirect->second);
		} else {
            this->handleError(500, "Unexpected redirection code in configuration.");
        }
        return (false);
    }
	return (true);
}

void HttpResponse::handleRedirect(int code, const std::string &uri) {
    this->_statusCode = code;
    this->_headers["Location"] = uri;

    // sample body for the redirect response
    this->_body = "<html><head><title>Redirect</title></head>";
    this->_body += "<body><h1>" + intToString(code) + " Redirect</h1>";
    this->_body += "<p>The document has moved <a href=\"" + uri + "\">here</a>.</p>";
    this->_body += "</body></html>";

    this->_headers["Content-Length"] = intToString(this->_body.size());
    this->_headers["Content-Type"] = "text/html";

    // Log the redirect for debugging
    std::cerr << "[DEBUG] Redirecting to " << uri << " with status code " << code << std::endl;
}

void HttpResponse::movedPermanently(const std::string &url) {
    this->_statusCode = 301;
    this->_headers["Location"] = url;
    this->_body = "<html><head><title>301 Moved Permanently</title></head>"
                  "<body><h1>301 Moved Permanently</h1>"
                  "<p>The requested resource has been permanently moved to "
                  "<a href=\"" + url + "\">" + url + "</a>.</p></body></html>";
    this->_headers["Content-Length"] = intToString(this->_body.size());
	this->sendHeader();
	this->_client->setError();
}


HttpRequest	*HttpResponse::getRequest() const {
	return (this->_client->getRequest());
}

void HttpResponse::error(const std::string &message) {
	std::string statusDescription;
	switch (this->_statusCode) {
		case 404:
			statusDescription = "Not Found";
			break;
		case 400:
			statusDescription = "Bad Request";
			break;
		case 500:
			statusDescription = "Internal Server Error";
			break;
		case 403:
			statusDescription = "Forbidden";
			break;
		case 413:
			statusDescription = "Payload Too Large";
			break;
		// Ajouter d'autres codes de statut;
		default:
			statusDescription = "Error";
	}

	this->_headers["Content-Type"] = "text/html";

	this->_body = "<html><head><title>" + intToString(this->_statusCode) + " " + statusDescription + "</title></head>";
	this->_body += "<body><h1>" + intToString(this->_statusCode) + " " + statusDescription + "</h1>";
	this->_body += "<p>" + message + "</p></body></html>";

	this->_headers["Content-Length"] = intToString(this->_body.size());
	// std::string header;
	// for (std::map<std::string, std::string>::iterator it = this->_headers.begin(); it != this->_headers.end(); ++it) {
    //     header += it->first + ": " + it->second + "\r\n";
    // }

    // // Terminer les en-têtes avec une ligne vide
    // header += "\r\n";
	_response = _body;
	sendHeader();
	this->_readyToSend = true;
}


void HttpResponse::handleError(int code, const std::string &message) {
	this->_statusCode = code;
	this->error(message);
}

std::string	HttpResponse::getResponse() {
	return (_response);
}

Server	*HttpResponse::getServer() const {
	return (this->_client->getServer());
}

void	HttpResponse::sendChunkEnd() {
	int	bytes;

	bytes = send(this->_client->getFd(), "\r\n", 2, 0);
	this->checkSend(bytes);
}

void	HttpResponse::sendFinalChunk() {
	int	bytes;

	bytes = send(this->_client->getFd(), "0\r\n\r\n", 5, 0);
	this->checkSend(bytes);
}
