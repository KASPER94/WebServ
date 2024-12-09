/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 14:51:58 by skapersk          #+#    #+#             */
/*   Updated: 2024/12/10 00:15:07 by skapersk         ###   ########.fr       */
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

HttpResponse::~HttpResponse() {
    // if (_cgiEnv) {
    //     // Assurez-vous de libérer la mémoire allouée à chaque chaîne
    //     for (size_t i = 0; _cgiEnv[i] != NULL; ++i) {
    //         free(_cgiEnv[i]);
    //     }
    //     delete[] _cgiEnv; // Libère le tableau char**
    // }
}

HttpResponse &HttpResponse::operator=(const HttpResponse &rhs) {
	this->_client = rhs._client;
	return (*this);
}

void HttpResponse::sendHeader() {
    std::string header = "HTTP/1.1 " + intToString(this->_statusCode) + " ";

    std::string statusDescription;
    switch (this->_statusCode) {
        case 200: statusDescription = "OK"; break;
        case 201: statusDescription = "Created"; break;
        case 204: statusDescription = "No Content"; break;
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
		 if (this->getServer()->getAutoindex()) {
            return true;
        }
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
	if (this->_headers["Content-Type"].empty()) {
		if (!this->_mime.empty()) {
			this->_headers["Content-Type"] = this->_mime;
		} else {
			this->_headers["Content-Type"] = "text/html";
		}
	}
	//changer avec keepalive !!!!!
	if (this->getRequest()->keepAlive()) {
        this->_headers["Connection"] = "keep-alive";
        this->_headers["Keep-Alive"] = "timeout=" + intToString(this->_client->getTimeout());
    } else {
        this->_headers["Connection"] = "close";
    }
}

bool isDirectoryEmpty(const std::string &path) {
    DIR *dir = opendir(path.c_str());
    if (!dir)
        return true;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;
        if (name != "." && name != "..") {
            closedir(dir);
            return false;
        }
    }
    closedir(dir);
    return true;
}

void HttpResponse::sendDirectoryPage(std::string path) {
    if (isDirectoryEmpty(path)) {
        this->handleError(404, "Directory is empty.");
        return;
    }

    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(path.c_str())) == NULL) {
        this->handleError(500, "Unable to open directory: " + path);
        return;
    }

    // Obtenez l'URI demandé par le client
    std::string requestPath = this->getRequest()->returnPATH();

    std::string body = "<html><head><title>Index of " + requestPath + "</title></head>";
    body += "<body><h1>Index of " + requestPath + "</h1><ul>";

    // Parcourir les entrées du répertoire
    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;

        // Ignorer les entrées spéciales "." et ".."
        if (name == "." || name == "..")
            continue;

        // Créez un lien relatif basé sur l'URI demandé
        std::string link = requestPath;
        if (link[link.size() - 1] != '/')
            link += "/";
        link += name;

        struct stat s;
        std::string displayName = name;
        if (stat((path + "/" + name).c_str(), &s) == 0 && (s.st_mode & S_IFDIR)) {
            displayName += "/"; // Ajouter "/" pour indiquer un répertoire
        }

        body += "<li><a href=\"" + link + "\">" + displayName + "</a></li>";
    }

    // Fin de la page HTML
    body += "</ul></body></html>";

    // Fermer le répertoire
    closedir(dir);

    // Stocker le corps de la réponse et sa longueur
    this->_body = body;
    this->_headers["Content-Length"] = intToString(body.size());
    this->_headers["Content-Type"] = "text/html";
    this->_headers["Connection"] = "close";

    // Envoyer les en-têtes et le corps
    this->createHeader();
    this->sendHeader();
    this->sendData(body.c_str(), body.size());
}


void	HttpResponse::directoryListing(std::string path) {
	this->_statusCode = 200;
	this->_mime = Mime::getMimeType("html");
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
            this->_statusCode = 204;
            this->_mime = "image/x-icon";
            this->_headers["Content-Type"] = this->_mime;
            this->_headers["Content-Length"] = "0";
            this->createHeader();
            this->sendHeader();
            return;
        }
		struct stat s;
		if (stat(uri.c_str(), &s) != 0) {
			this->handleError(404, "The requested resource was not found.");
		} else {
			this->handleError(500, "Failed to Open File");
		}
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

char **HttpResponse::createEnv(HttpRequest *request) {
    std::vector<std::string> envVars;

    // Variables CGI standard
    envVars.push_back("SERVER_PROTOCOL=HTTP/1.1");
    envVars.push_back("REQUEST_URI=" + request->returnPATH());
    envVars.push_back("CONTENT_TYPE=" + request->getHeaders()["Content-Type"]);
	if (request->getContentLen() == 0)
	    envVars.push_back("CONTENT_LENGTH=0");
	else
    	envVars.push_back("CONTENT_LENGTH=" + intToString(request->getContentLen()));
    envVars.push_back("SERVER_PORT=" + intToString(this->getServer()->getPort()));
    // envVars.push_back("REQUEST_METHOD=" + request->getMethod());

    // Query String
    const t_query &query = request->getQueryString();
    envVars.push_back("QUERY_STRING=" + query.strquery);
    for (std::map<std::string, std::string>::const_iterator it = query.params.begin(); it != query.params.end(); ++it) {
        envVars.push_back("QUERY_PARAM_" + it->first + "=" + it->second);
    }

    // Ajouter les en-têtes HTTP convertis en format CGI
    const std::map<std::string, std::string> &headers = request->getHeaders();
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        std::string key = it->first;
        std::string value = it->second;

        // Convertir les noms d'en-têtes en CGI-compliant
        for (size_t i = 0; i < key.size(); ++i) {
            if (key[i] == '-') {
                key[i] = '_';
            } else {
                key[i] = toupper(key[i]);
            }
        }
        envVars.push_back("HTTP_" + key + "=" + value);
    }

    // Ajouter les données de formulaire si disponibles
    const std::map<std::string, std::string> &formData = request->getFormData();
    for (std::map<std::string, std::string>::const_iterator it = formData.begin(); it != formData.end(); ++it) {
        envVars.push_back("FORM_" + it->first + "=" + it->second);
    }
    char **env = new char*[envVars.size() + 1];
    for (size_t i = 0; i < envVars.size(); ++i) {
        if (!envVars[i].empty()) {
            env[i] = strdup(envVars[i].c_str());
            if (!env[i]) {
                for (size_t j = 0; j < i; ++j) {
                    delete[] env[j];
                }
                delete[] env;
                return NULL;
            }
        }
    }
    env[envVars.size()] = NULL; // Terminateur

    return env;
}

char **buildArgv(const std::string &cgiBin, const std::string &uri) {
    char **argv = new char*[3]; // 2 arguments + NULL
    argv[0] = strdup(cgiBin.c_str());
    argv[1] = strdup(uri.c_str());
    argv[2] = NULL; // Terminaison avec NULL
    return argv;
}

void freeArgv(char **argv) {
    for (size_t i = 0; argv[i] != NULL; ++i) {
        free(argv[i]);
    }
    delete[] argv;
}

char **buildEnvp(const std::vector<std::string> &environ) {
    char **envp = new char*[environ.size() + 1]; // +1 pour le NULL final
    for (size_t i = 0; i < environ.size(); ++i) {
        envp[i] = strdup(environ[i].c_str()); // Copie chaque chaîne dans envp
    }
    envp[environ.size()] = NULL; // Terminaison avec NULL
    return envp;
}

void freeEnvp(char **envp, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        free(envp[i]);
    }
    delete[] envp;
}

char **HttpResponse::mergeEnvironments(char **originalEnv, char **cgiEnv) {
    std::set<std::string> envSet;
    std::vector<std::string> mergedEnv;

    // Add original environment variables to the set and vector
    for (size_t i = 0; originalEnv && originalEnv[i] != NULL; ++i) {
        std::string var(originalEnv[i]);
        envSet.insert(var.substr(0, var.find('='))); // Store variable name only
        mergedEnv.push_back(var);
    }

    // Add CGI environment variables, avoiding duplicates
    for (size_t i = 0; cgiEnv && cgiEnv[i] != NULL; ++i) {
        std::string var(cgiEnv[i]);
        std::string key = var.substr(0, var.find('='));
        if (envSet.find(key) == envSet.end()) { // Add only if not already in set
            envSet.insert(key);
            mergedEnv.push_back(var);
        }
    }

    // Convert the vector to a NULL-terminated array
    char **mergedArray = new char*[mergedEnv.size() + 1];
    for (size_t i = 0; i < mergedEnv.size(); ++i) {
        mergedArray[i] = strdup(mergedEnv[i].c_str());
    }
    mergedArray[mergedEnv.size()] = NULL; // Add NULL terminator

    return mergedArray;
}



bool HttpResponse::executeCGI(const std::string &uri) {
    char tempFileName[] = "/tmp/cgi_output_XXXXXX";
    int tempFd = mkstemp(tempFileName); // Fichier temporaire sécurisé
    if (tempFd == -1) {
        this->handleError(500, "CGI execution failed: unable to create temp file");
        return false;
    }

    this->_cgiTmpFile = tempFileName;

    pid_t pid = fork();
    if (pid < 0) {
        this->handleError(500, "CGI execution failed: fork error");
        close(tempFd);
        unlink(tempFileName);
        return false;
    }
	HttpRequest *request = this->getRequest();
	char **en = createEnv(request);
	// char **cgiEnv;
	if (this->_client->getCgiEnv() == NULL || this->_client->getCgiEnv()[0] == NULL) {
		_cgiEnv = mergeEnvironments(en, env()->envp);
	}
	else
		_cgiEnv = mergeEnvironments(en, this->_client->getCgiEnv());
	_client->setCgiEnv(_cgiEnv);
	freeEnv(en);
	// int i = 0;
	// while (cgiEnv[i]) {
	// 	std::cout << cgiEnv[i] << std::endl;
	// 	i++;
	// }
    if (pid == 0) {
        // Processus enfant
        dup2(tempFd, STDOUT_FILENO); // Redirige stdout vers le fichier temporaire
        dup2(tempFd, STDERR_FILENO); // Redirige stderr pour capturer les erreurs
        close(tempFd);

        // Préparer les variables d'environnement et les arguments
    	// HttpRequest *request = this->getRequest();
        // char **en = createEnv(request);
		// char **cgiEnv = mergeEnvironments(env()->envp, en);
        char **argv = buildArgv(this->_cgiBin, uri);

        // Exécuter le script CGI
        if (execve(this->_cgiBin.c_str(), argv, _cgiEnv) == -1) {
            perror("execve failed"); // Affiche l'erreur dans stderr
            freeArgv(en);
            freeArgv(argv);
            exit(127); // Code d'erreur pour execve
        }
    } else {
        // Processus parent
        close(tempFd);

        int status;
        waitpid(pid, &status, 0); // Attend la fin du processus enfant

        if (WIFEXITED(status)) {
            int exitCode = WEXITSTATUS(status);
            if (exitCode != 0) {
                this->handleError(500, "CGI execution failed with code: " + intToString(exitCode));
                unlink(tempFileName);
                return false;
            }
        } else {
            this->handleError(500, "CGI process terminated abnormally");
            unlink(tempFileName);
            return false;
        }
    }

    // Le CGI a réussi, la sortie est dans le fichier temporaire
    return true;
}

void HttpResponse::handleCGI(std::string uri) {
    if (!executeCGI(uri)) {
        this->handleError(500, "CGI Execution Failed");
        return;
    }

    // Lire le fichier temporaire contenant la sortie CGI
    std::ifstream cgiOutput(this->_cgiTmpFile.c_str(), std::ios::binary);
    if (!cgiOutput.is_open()) {
        this->handleError(500, "Failed to Open CGI Output");
        return;
    }

    std::stringstream buffer;
    buffer << cgiOutput.rdbuf(); // Lire tout le contenu du fichier CGI
    std::string cgiResponse = buffer.str();
    cgiOutput.close();

    // Vérifier la sortie CGI
    size_t headerEnd = cgiResponse.find("\r\n\r\n");
    if (headerEnd == std::string::npos) {
        this->handleError(500, "Malformed CGI Response");
        return;
    }

    // Extraire les en-têtes et le corps
    std::string headers = cgiResponse.substr(0, headerEnd);
    std::string body = cgiResponse.substr(headerEnd + 4);

    // Vérifiez et configurez les en-têtes (ex : Content-Type, Content-Length)
    if (headers.find("Content-Length:") == std::string::npos) {
        _headers["Content-Length"] = intToString(body.size());
    }
    if (headers.find("Content-Type:") == std::string::npos) {
        _headers["Content-Type"] = "text/html"; // Définir un type MIME par défaut
    }

    // Envoyer la réponse
    this->_statusCode = 200;
    this->createHeader();
    this->sendHeader();
    this->sendData(body.c_str(), body.size());

    // Supprimez le fichier temporaire
    unlink(this->_cgiTmpFile.c_str());
}

bool DirectoriesRecursively(const std::string &path) {
    size_t pos = 0;

    while (pos != std::string::npos) {
        pos = path.find('/', pos + 1);
        std::string subPath = path.substr(0, pos);

        struct stat info;
        if (stat(subPath.c_str(), &info) != 0) {
			logMsg(DEBUG, "Failed access to requested path: " + std::string(strerror(errno)));
			return false;
        } else if (!S_ISDIR(info.st_mode)) {
			logMsg(DEBUG, "Requested path exists but is not a directory");
            return false;
        }
    }
    return true;
}

bool HttpResponse::handleUpload() {
	std::string uploadPath = this->_client->getServer()->getUploadPath();
	if (uploadPath.empty()) {
		logMsg(DEBUG, "No upload path defined in configuration");
		this->handleError(500, "Upload path not configured.");
		return false;
	}

	std::string Path = _root + uploadPath;
    if (!DirectoriesRecursively(Path)) {
        this->handleError(500, "Cannot create upload directory.");
        return false;
    }

	const std::map<std::string, std::string> &files = this->_client->getRequest()->getFileData();
	for (std::map<std::string, std::string>::const_iterator it = files.begin(); it != files.end(); ++it) {
		std::string filePath = _root;
		if (uploadPath.c_str()[uploadPath.size() - 1] == '/')
			filePath += uploadPath + it->first;
		else
			filePath += uploadPath + "/" + it->first;
       	std::ofstream file(filePath.c_str(), std::ios::binary | std::ios::trunc);
        if (!file.is_open()) {
			logMsg(DEBUG, "Failed to create uploaded file " + filePath + ": " + std::string(strerror(errno)));
            this->handleError(500, "Failed to create uploaded file.");
            return false;
        }
		file << it->second;
		file.close();
		_statusCode = 201;
	}
	logMsg(DEBUG, "Files uploaded successfully to " + uploadPath);
	return true;
}

void HttpResponse::handlePostRequest() {
    // Check for file uploads
    const std::map<std::string, std::string>& files = this->getRequest()->getFileData();
    const std::map<std::string, std::string>& formData = this->getRequest()->getFormData();

    if (!files.empty()) {
        if (handleUpload()) {
            // Respond with a JSON success message for uploads
            this->_statusCode = 201; // Created
            std::string jsonResponse = "{\n  \"status\": \"success\",\n  \"message\": \"Files uploaded successfully\",\n  \"files\": [\n";

            for (std::map<std::string, std::string>::const_iterator it = files.begin(); it != files.end(); ++it) {
                jsonResponse += "    \"" + it->first + "\"";
                std::map<std::string, std::string>::const_iterator nextIt = it;
                ++nextIt; // Manual iterator increment to check the next element
                if (nextIt != files.end()) {
                    jsonResponse += ",";
                }
                jsonResponse += "\n";
            }

            jsonResponse += "  ]\n}";
            this->_headers["Content-Type"] = "application/json";
            this->_headers["Content-Length"] = intToString(jsonResponse.size());
			this->_headers["Location"] = _uri;
            this->createHeader();
            this->sendHeader();
            this->sendData(jsonResponse.c_str(), jsonResponse.size());
        } else {
            handleError(500, "Failed to upload files.");
        }
        return;
    }

    // Handle form data only
    if (!formData.empty()) {
        this->_statusCode = 200; // OK
        std::string jsonResponse = "{\n  \"status\": \"success\",\n  \"message\": \"Form data received successfully\",\n  \"data\": {\n";

        for (std::map<std::string, std::string>::const_iterator it = formData.begin(); it != formData.end(); ++it) {
            jsonResponse += "    \"" + it->first + "\": \"" + it->second + "\"";
            std::map<std::string, std::string>::const_iterator nextIt = it;
            ++nextIt; // Manual iterator increment to check the next element
            if (nextIt != formData.end()) {
                jsonResponse += ",";
            }
            jsonResponse += "\n";
        }

        jsonResponse += "  }\n}";
        this->_headers["Content-Type"] = "application/json";
        this->_headers["Content-Length"] = intToString(jsonResponse.size());
        this->createHeader();
        this->sendHeader();
        this->sendData(jsonResponse.c_str(), jsonResponse.size());
        return;
    }

    // If no form data or files were found
    handleError(400, "No valid data provided in the POST request.");
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
	_uri = uri;
	if (!this->methodAllowed(this->getRequest()->getMethod())) {
        return handleError(405, "Method Not Allowed");
    }
	if (this->getRequest()->getMethod() == DELETE) {
		tryDeleteFile(uri);
		return ;
	}
	if (this->getRequest()->getMethod() == POST) {
        handlePostRequest();
        return;
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
	this->_allowedMethod = this->getServer()->getAllowedMethods();
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
			_directoryListing = loc->getAutoindex();
			saveLoc.loc = *loc;
            resolvePath = loc->getRoot();
			if (uri.compare(location)) {
				std::string tmp = uri.substr(uri.find(location) + location.size());
				resolvePath = loc->getRoot() + tmp;
			}
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
			else if (!_directoryListing && loc->getIndex().empty()) {
				return (false);
			}
        } else if (uri.compare("/")) {
			resolvePath = uri;
		} else {
			if (_root.c_str()[_root.size() - 1] == '/')
				resolvePath = _root;
			else
            	resolvePath = _root + uri;
		}
    }
	else {
		// std::cerr << "### 00 " << uri << std::endl;
		// if (_root.c_str()[_root.size() - 1] == '/')
		// 	resolvePath = _root;
		// else
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
	// if (uri == "/")
		uri = resolvePath;

	// else
	// 	uri = _root + this->_client->getRequest()->returnPATH();

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
	
	std::string tmp = this->getRequest()->returnPATH();
	std::string location = matchLocation(tmp);
	if (!location.empty()) {
		Location *loc = this->getServer()->getLocation(location);
		_returnURI = loc->getReturnUri();
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
	logMsg(DEBUG, "Redirecting to " + uri + " with status code " + toString(code));
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


// void HttpResponse::handleError(int code, const std::string &message) {
// 	this->_statusCode = code;
// 	this->error(message);
// }

void HttpResponse::handleError(int code, const std::string &message) {
    this->_statusCode = code;

    // Vérifiez s'il y a une page d'erreur définie dans la location actuelle
    std::string errorPage;
    if (saveLoc.inLoc) {
        errorPage = saveLoc.loc.getErrorPage(code);
        if (!errorPage.empty()) {
            std::string errorUri = saveLoc.loc.getRoot() + errorPage;
            struct stat s;
            if (stat(errorUri.c_str(), &s) == 0) {
                this->serveStaticFile(errorUri); // Servir la page d'erreur personnalisée
                return;
            }
        }
    }

    // Si aucune page d'erreur définie dans la location, chercher au niveau global du serveur
    if (errorPage.empty()) {
        errorPage = this->getServer()->getErrorPage(code);
        if (!errorPage.empty()) {
            std::string errorUri = this->getServer()->getRoot() + errorPage;
            struct stat s;
            if (stat(errorUri.c_str(), &s) == 0) {
                this->serveStaticFile(errorUri); // Servir la page d'erreur globale
                return;
            }
        }
    }

    // Aucune page d'erreur personnalisée ou erreur dans son chargement
    std::string statusDescription;
    switch (this->_statusCode) {
        case 404: statusDescription = "Not Found"; break;
        case 400: statusDescription = "Bad Request"; break;
        case 500: statusDescription = "Internal Server Error"; break;
        case 403: statusDescription = "Forbidden"; break;
        case 413: statusDescription = "Payload Too Large"; break;
        default: statusDescription = "Error"; break;
    }

    // Réponse générique
    this->_body = "<html><head><title>" + intToString(this->_statusCode) + " " + statusDescription + "</title></head>";
    this->_body += "<body><h1>" + intToString(this->_statusCode) + " " + statusDescription + "</h1>";
    this->_body += "<p>" + message + "</p></body></html>";

    this->_headers["Content-Length"] = intToString(this->_body.size());
    this->_headers["Content-Type"] = "text/html";

    this->sendHeader();
    this->sendData(this->_body.c_str(), this->_body.size());
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
