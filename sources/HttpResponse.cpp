/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 14:51:58 by skapersk          #+#    #+#             */
/*   Updated: 2024/12/11 11:49:01 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "HttpResponse.hpp"
# include <algorithm>
# include <sstream>
# include <iomanip>


std::string urlDecode(const std::string& encoded) {
    std::string decoded;
    for (size_t i = 0; i < encoded.length(); ++i) {
        if (encoded[i] == '%' && i + 2 < encoded.length()) {
            std::string hex = encoded.substr(i + 1, 2);
            int ch;
            std::stringstream ss;
            ss << std::hex << hex;
            ss >> ch;
            decoded += static_cast<char>(ch);
            i += 2;
        }
        else if (encoded[i] == '+') {
            decoded += ' ';
        }
        else {
            decoded += encoded[i];
        }
    }
    return decoded;
}

HttpResponse::HttpResponse(): _client(NULL) {}

HttpResponse::HttpResponse(Client *client):
	_client(client) {
}

HttpResponse::HttpResponse(const HttpResponse &cpy) {
	*this = cpy;
}

HttpResponse::~HttpResponse() {
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

    for (std::map<std::string, std::string>::iterator it = this->_headers.begin(); it != this->_headers.end(); ++it) {
        header += it->first + ": " + it->second + "\r\n";
    }
	
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

std::string normalizeUrl(const std::string& path) {
    std::string normalized = path;
    size_t pos;
    while ((pos = normalized.find("//")) != std::string::npos) {
        normalized.replace(pos, 2, "/");
    }
    while (normalized.length() > 1 && normalized[normalized.length() - 1] == '/') {
        normalized.erase(normalized.length() - 1);
    }
    return normalized;
}

bool HttpResponse::hasAccess(std::string &uri, bool &isDir) {
    struct stat s;

    if (stat(uri.c_str(), &s) == 0) {
        if (s.st_mode & S_IFDIR) {
            isDir = true;
            std::string path = this->getRequest()->returnPATH();
            if (path != "/" && path[path.length() - 1] != '/') {
                this->movedPermanently(path + "/");
                return false;
            }
        }
        else
            isDir = false;
    }
    
    if (isDir) {
        if (this->getServer()->getAutoindex()) {
            isDir = true;
            return true;
        }
        else {
            std::vector<std::string>::iterator it = this->_indexes.begin();
            if (*it == "") {
                isDir = true;
                return true;
            }
            if (access((uri + *it).c_str(), F_OK) != -1) {
                if (access((uri + *it).c_str(), R_OK) == -1) {
                    this->handleError(403, "Permission denied");
                    return false;
                }
                uri += *it;
                isDir = false;
                return true;
            }
        }
    }
    return true;
}

bool	HttpResponse::methodAllowed(enum HttpMethod method) {
	(void)method;
	if (this->_allowedMethod.empty())
		return false;

	std::string requestMethod = this->getRequest()->HttpMethodTostring();
	
	for (std::vector<std::string>::iterator it = this->_allowedMethod.begin(); 
		 it != this->_allowedMethod.end(); ++it) {
		if (*it == requestMethod)
			return true;
	}
	return false;
}

void HttpResponse::tryDeleteFile(std::string &uri) {
    std::string root;
    std::string decodedUri = urlDecode(uri);

    if (this->_isLocation) {
        root = this->_root;
    } else {
        root = this->getServer()->getRoot();
    }

    decodedUri = normalizeUrl(decodedUri);

    logMsg(DEBUG, "Attempting to delete file: " + decodedUri);
    logMsg(DEBUG, "Root path: " + root);

    if (access(decodedUri.c_str(), F_OK) == -1) {
        this->handleError(404, "File not found");
        return;
    }
    if (access(decodedUri.c_str(), W_OK) == -1) {
        this->handleError(403, "Permission denied");
        return;
    }
    if (!childPath(getFullPath(root), getFullPath(decodedUri))) {
        this->handleError(403, "Access denied");
        return;
    }
    if (remove(decodedUri.c_str()) == 0) {
        this->_statusCode = 204;
        this->_headers.clear();
        this->_headers["Content-Length"] = "0";
        this->createHeader();
        this->sendHeader();
    } else {
        std::string errorMsg = "Failed to delete file: ";
        errorMsg += strerror(errno);
        this->handleError(500, errorMsg);
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

class DirectoryEntryCompare {
public:
    bool operator()(const std::pair<std::string, std::string>& a, 
                   const std::pair<std::string, std::string>& b) const {
        if (a.second != b.second) 
            return a.second > b.second;
        return a.first < b.first;
    }
};

std::string formatFileSize(size_t bytes) {
    const char* units[] = {"B", "KB", "MB", "GB"};
    int unit = 0;
    double size = static_cast<double>(bytes);
    
    while (size >= 1024.0 && unit < 3) {
        size /= 1024.0;
        unit++;
    }
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << size << " " << units[unit];
    return ss.str();
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

    std::string requestPath = this->getRequest()->returnPATH();
    
    std::string body = "<!DOCTYPE html><html><head><title>File Manager - " + requestPath + "</title>";
    body += "<link rel='stylesheet' href='https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css'>";
    body += "<link rel='stylesheet' href='https://cdn.jsdelivr.net/npm/bootstrap-icons@1.11.3/font/bootstrap-icons.min.css'>";
    body += "<style>";
    body += "body { background-color: #f8f9fa; padding: 2rem; }";
    body += ".file-manager { background: white; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }";
    body += ".file-item { display: flex; align-items: center; padding: 0.75rem; border-bottom: 1px solid #eee; }";
    body += ".file-item:hover { background-color: #f8f9fa; }";
    body += ".file-icon { font-size: 1.2rem; margin-right: 1rem; width: 24px; }";
    body += ".file-name { flex-grow: 1; }";
    body += ".file-actions { min-width: 100px; text-align: right; }";
    body += ".file-size { color: #6c757d; font-size: 0.9rem; min-width: 100px; text-align: right; margin-right: 1rem; }";
    body += ".upload-zone { border: 2px dashed #dee2e6; border-radius: 8px; padding: 1.5rem; margin: 1rem 0; text-align: center; }";
    body += ".upload-zone:hover { border-color: #0d6efd; }";
    body += "</style></head>";
    
    body += "<body><div class='container'><div class='file-manager p-3'>";
    body += "<div class='d-flex justify-content-between align-items-center mb-4'>";
    body += "<h4 class='m-0'><i class='bi bi-folder2-open me-2'></i>File Manager - " + requestPath + "</h4>";
    

    body += "<div class='upload-zone'>";
    body += "<form method='post' enctype='multipart/form-data' class='mb-0'>";
    body += "<div class='d-flex align-items-center gap-3'>";
    body += "<input type='file' name='file' class='form-control' style='width: auto;'>";
    body += "<button type='submit' class='btn btn-primary'><i class='bi bi-upload me-2'></i>Upload</button>";
    body += "</div></form></div></div>";

    std::vector<std::pair<std::string, std::string> > entries;

    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;
        if (name != "." && name != "..") {
            struct stat s;
            std::string fullPath = path + "/" + name;
            if (stat(fullPath.c_str(), &s) == 0) {
                std::string type = S_ISDIR(s.st_mode) ? "dir" : "file";
                entries.push_back(std::make_pair(name, type));
            }
        }
    }

    std::sort(entries.begin(), entries.end(), DirectoryEntryCompare());

    // Add parent directory link if not at root
    if (requestPath != "/") {
        body += "<div class='file-item'>";
        body += "<i class='bi bi-arrow-up-circle file-icon text-primary'></i>";
        body += "<a href='../' class='file-name text-decoration-none'>Parent Directory</a>";
        body += "<span class='file-size'>-</span>";
        body += "<div class='file-actions'></div>";
        body += "</div>";
    }

    // Output sorted entries
    for (std::vector<std::pair<std::string, std::string> >::iterator it = entries.begin(); 
         it != entries.end(); ++it) {
        std::string name = it->first;
        bool isDir = it->second == "dir";
        std::string link = requestPath;
        // Remove trailing slashes except for root path
        while (link.length() > 1 && link[link.length() - 1] == '/') {
            link.erase(link.length() - 1);
        }
        link += "/";
        link += name;

        body += "<div class='file-item'>";
        if (isDir) {
            body += "<i class='bi bi-folder-fill file-icon text-warning'></i>";
            body += "<a href='" + link + "/' class='file-name text-decoration-none'>" + name + "</a>";
            body += "<span class='file-size'>-</span>";
        } else {
            body += "<i class='bi bi-file-earmark file-icon text-secondary'></i>";
            body += "<a href='" + link + "' class='file-name text-decoration-none'>" + name + "</a>";
            struct stat s;
            std::string fullPath = path + "/" + name;
            if (stat(fullPath.c_str(), &s) == 0) {
                std::string size = formatFileSize(s.st_size);
                body += "<span class='file-size'>" + size + "</span>";
            }
        }

        if (!isDir) {
            body += "<div class='file-actions'>";
            body += "<button onclick='deleteFile(\"" + link + "\")' class='btn btn-sm btn-outline-danger'>";
            body += "<i class='bi bi-trash'></i></button></div>";
        } else {
            body += "<div class='file-actions'></div>";
        }
        body += "</div>";
    }

    body += "<script>";
    body += "function deleteFile(path) {";
    body += "  if (!confirm('Are you sure you want to delete this file?')) return;";
    body += "  var xhr = new XMLHttpRequest();";
    body += "  xhr.open('DELETE', path, true);";
    body += "  xhr.onload = function() {";
    body += "    if (xhr.status === 204) {";
    body += "      window.location.reload();";
    body += "    } else {";
    body += "      alert('Error deleting file');";
    body += "    }";
    body += "  };";
    body += "  xhr.onerror = function() {";
    body += "    alert('Error deleting file');";
    body += "  };";
    body += "  xhr.send();";
    body += "}";
    body += "</script>";

    body += "</div></div></body></html>";
    closedir(dir);

    this->_statusCode = 200;
    this->_headers["Content-Type"] = "text/html";
    this->_headers["Content-Length"] = intToString(body.size());
    
    this->createHeader();
    this->sendHeader();
    this->sendData(body.c_str(), body.size());
}

void	HttpResponse::directoryListing(std::string path) {
	this->_statusCode = 200;
	this->_mime = Mime::getMimeType("html");
	this->sendDirectoryPage(path);
}

int HttpResponse::sendData(const void *data, int len) {
    const char *ptr = static_cast<const char *>(data);
    int bytes;

    while (len > 0 && !this->_client->getError()) {
        bytes = send(this->_client->getFd(), ptr, len, 0);
        this->checkSend(bytes);
        ptr += bytes;
        len -= bytes;
    }
    return (0);
}

void HttpResponse::serveStaticFile(const std::string &uri) {
    std::string decodedUri = urlDecode(uri);
    std::ifstream file(decodedUri.c_str(), std::ios::binary);
    if (!file.is_open()) {
        if (decodedUri.find("favicon.ico") != std::string::npos) {
            this->_statusCode = 204;
            this->_mime = "image/x-icon";
            this->_headers["Content-Type"] = this->_mime;
            this->_headers["Content-Length"] = "0";
            this->createHeader();
            this->sendHeader();
            return;
        }
        this->handleError(404, "Not Found");
        return;
    }

    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    std::string ext = decodedUri.substr(decodedUri.find_last_of(".") + 1);
    this->_mime = Mime::getMimeType(ext);
    this->_statusCode = 200;
    this->_headers["Content-Type"] = this->_mime;
    this->_headers["Content-Length"] = intToString(fileSize);
    std::string filename = decodedUri.substr(decodedUri.find_last_of("/") + 1);
    this->_headers["Content-Disposition"] = "inline; filename=\"" + filename + "\"";
    this->createHeader();
    this->sendHeader();
    char buffer[4096];
    while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
        this->sendData(buffer, file.gcount());
    }
    file.close();
}

char **HttpResponse::createEnv(HttpRequest *request, std::string uri) {
    std::vector<std::string> envVars;

    // Variables CGI standard
    envVars.push_back("REDIRECT_STATUS=200");
    envVars.push_back("GATEWAY_INTERFACE=CGI/1.1");
    envVars.push_back("SERVER_SOFTWARE=Webserv/1.0");
    envVars.push_back("SERVER_PROTOCOL=HTTP/1.1");
    envVars.push_back("SERVER_NAME=" + this->getServer()->getServerName());

    std::stringstream port;
    port << this->getServer()->getPort();
    envVars.push_back("SERVER_PORT=" + port.str());

    envVars.push_back("REQUEST_METHOD=" + request->HttpMethodTostring());

    // Define PATH_INFO and PATH_TRANSLATED
    envVars.push_back("PATH_INFO=" + request->returnPATH());
    envVars.push_back("PATH_TRANSLATED=" + this->getServer()->getRoot() + request->returnPATH());

    // SCRIPT_NAME
    std::string scriptName = uri;
	//ATTENTION A CHANGER CA !
	char cwd[4096];
	std::string serverRoot;
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
    	serverRoot = std::string(cwd) + "/";
	}
	else
		serverRoot = "/home/";
	envVars.push_back("SCRIPT_FILENAME=" + serverRoot + uri); // Chemin absolu
    envVars.push_back("SCRIPT_NAME=" + uri); 
	envVars.push_back("REDIRECT_STATUS=200");

    // Query String
    const t_query &query = request->getQueryString();
    envVars.push_back("QUERY_STRING=" + query.strquery);

    // Client info
    envVars.push_back("REMOTE_HOST=" + this->getServer()->getHostname());
    // envVars.push_back("REMOTE_ADDR=" + this->getServer()->getHost());

    // Content headers
    envVars.push_back("CONTENT_TYPE=" + request->getHeaders()["Content-Type"]);
    if (request->getContentLen() == 0) {
        envVars.push_back("CONTENT_LENGTH=0");
    } else {
        envVars.push_back("CONTENT_LENGTH=" + intToString(request->getContentLen()));
    }

    // HTTP headers converted to CGI-compliant variables
    const std::map<std::string, std::string> &headers = request->getHeaders();
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        std::string key = it->first;
        std::string value = it->second;

        // Convert header names to uppercase and replace '-' with '_'
        for (size_t i = 0; i < key.size(); ++i) {
            if (key[i] == '-') {
                key[i] = '_';
            } else {
                key[i] = toupper(key[i]);
            }
        }
        envVars.push_back("HTTP_" + key + "=" + value);
    }

    // Form data if available
    const std::map<std::string, std::string> &formData = request->getFormData();
    for (std::map<std::string, std::string>::const_iterator it = formData.begin(); it != formData.end(); ++it) {
        envVars.push_back("FORM_" + it->first + "=" + it->second);
    }

    // Convert envVars to char**
    char **env = new char*[envVars.size() + 1];
    for (size_t i = 0; i < envVars.size(); ++i) {
        env[i] = strdup(envVars[i].c_str());
        if (!env[i]) {
            for (size_t j = 0; j < i; ++j) {
                delete[] env[j];
            }
            delete[] env;
            return NULL;
        }
    }
    env[envVars.size()] = NULL; // Null terminator

    return env;
}

// char **HttpResponse::createEnv(HttpRequest *request) {
//     std::vector<std::string> envVars;

//     // Variables CGI standard
// 	envVars.push_back("REDIRECT_STATUS=200");
//     envVars.push_back("GATEWAY_INTERFACE=CGI/1.1");
// 	envVars.push_back("SERVER_SOFTWARE=Webserv/1.0");
//     envVars.push_back("SERVER_PROTOCOL=HTTP/1.1");
//     envVars.push_back("REQUEST_URI=" + request->returnPATH());
//     envVars.push_back("CONTENT_TYPE=" + request->getHeaders()["Content-Type"]);
// 	if (request->getContentLen() == 0)
// 	    envVars.push_back("CONTENT_LENGTH=0");
// 	else
//     	envVars.push_back("CONTENT_LENGTH=" + intToString(request->getContentLen()));
//     envVars.push_back("SERVER_PORT=" + intToString(this->getServer()->getPort()));
//     envVars.push_back("REQUEST_METHOD=" + this->getRequest()->getMethodstr());
// 	std::cout << "REQUEST_METHOD=" + this->getRequest()->getMethodstr() << std::endl;

//     // Query String
//     const t_query &query = request->getQueryString();
//     envVars.push_back("QUERY_STRING=" + query.strquery);
//     for (std::map<std::string, std::string>::const_iterator it = query.params.begin(); it != query.params.end(); ++it) {
//         envVars.push_back("QUERY_PARAM_" + it->first + "=" + it->second);
//     }

//     // Ajouter les en-têtes HTTP convertis en format CGI
//     const std::map<std::string, std::string> &headers = request->getHeaders();
//     for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
//         std::string key = it->first;
//         std::string value = it->second;

//         // Convertir les noms d'en-têtes en CGI-compliant
//         for (size_t i = 0; i < key.size(); ++i) {
//             if (key[i] == '-') {
//                 key[i] = '_';
//             } else {
//                 key[i] = toupper(key[i]);
//             }
//         }
//         envVars.push_back("HTTP_" + key + "=" + value);
//     }

//     // Ajouter les données de formulaire si disponibles
//     const std::map<std::string, std::string> &formData = request->getFormData();
//     for (std::map<std::string, std::string>::const_iterator it = formData.begin(); it != formData.end(); ++it) {
//         envVars.push_back("FORM_" + it->first + "=" + it->second);
//     }
//     char **env = new char*[envVars.size() + 1];
//     for (size_t i = 0; i < envVars.size(); ++i) {
//         if (!envVars[i].empty()) {
//             env[i] = strdup(envVars[i].c_str());
//             if (!env[i]) {
//                 for (size_t j = 0; j < i; ++j) {
//                     delete[] env[j];
//                 }
//                 delete[] env;
//                 return NULL;
//             }
//         }
//     }
//     env[envVars.size()] = NULL; // Terminateur

//     return env;
// }

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
    // Créer un fichier temporaire pour capturer la sortie CGI
    char tempFileName[] = "/tmp/cgi_output_XXXXXX";
    int tempFd = mkstemp(tempFileName);
    if (tempFd == -1) {
        this->handleError(500, "CGI execution failed: unable to create temp file");
        return false;
    }
    this->_cgiTmpFile = tempFileName;

    // Créer un fichier temporaire pour transmettre le corps de la requête
    FILE *contentTmp = tmpfile();
    if (!contentTmp) {
        this->handleError(500, "CGI execution failed: unable to create temp input file");
        close(tempFd);
        unlink(tempFileName);
        return false;
    }
    int contentFd = fileno(contentTmp);
    if (contentFd == -1) {
        this->handleError(500, "CGI execution failed: unable to get file descriptor");
        fclose(contentTmp);
        close(tempFd);
        unlink(tempFileName);
        return false;
    }

    // Écrire le corps de la requête dans le fichier temporaire
    fputs(this->getRequest()->getBody().c_str(), contentTmp);
    lseek(contentFd, 0, SEEK_SET); // Réinitialiser le pointeur pour la lecture

    pid_t pid = fork();
    if (pid < 0) {
        this->handleError(500, "CGI execution failed: fork error");
        fclose(contentTmp);
        close(contentFd);
        close(tempFd);
        unlink(tempFileName);
        return false;
    } else if (pid == 0) {
        // Processus enfant
        char **env = createEnv(this->getRequest(), uri);

        // Préparer les arguments pour le script CGI
        std::string script = uri.substr();
        char **argv = new char*[3];
        argv[0] = strdup(this->_cgiBin.c_str());
        argv[1] = strdup(script.c_str());
        argv[2] = NULL;

        // Rediriger les entrées/sorties
        if (dup2(contentFd, STDIN_FILENO) == -1 || dup2(tempFd, STDOUT_FILENO) == -1) {
            perror("dup2 failed");
            exit(127);
        }
        close(contentFd);
        close(tempFd);

        // Exécuter le script CGI
        if (execve(this->_cgiBin.c_str(), argv, env) == -1) {
            perror("execve failed");
            exit(127);
        }
    } else {
        // Processus parent
        close(contentFd);
        fclose(contentTmp);
        close(tempFd);

        int status;
        waitpid(pid, &status, 0); // Attendre la fin du processus enfant

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
        // this->handleError(500, "CGI Execution Failed");
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

    // Create full path
    std::string fullPath = _root;
    if (uploadPath[0] != '/')
        fullPath += "/";
    fullPath += uploadPath;

    // Ensure directory exists
    if (!DirectoriesRecursively(fullPath)) {
        // Try to create directory if it doesn't exist
        std::string cmd = "mkdir -p " + fullPath;
        if (system(cmd.c_str()) != 0) {
            this->handleError(500, "Cannot create upload directory.");
            return false;
        }
    }

    const std::map<std::string, std::string> &files = this->_client->getRequest()->getFileData();
    for (std::map<std::string, std::string>::const_iterator it = files.begin(); it != files.end(); ++it) {
        std::string filePath = fullPath;
        if (filePath[filePath.size() - 1] != '/')
            filePath += "/";
        filePath += it->first;

        // Open file in binary mode with large buffer
        std::ofstream file(filePath.c_str(), std::ios::binary | std::ios::trunc);
        if (!file.is_open()) {
            logMsg(DEBUG, "Failed to create uploaded file " + filePath + ": " + std::string(strerror(errno)));
            this->handleError(500, "Failed to create uploaded file.");
            return false;
        }

        // Set buffer size for better performance with large files
        char buffer[8192];
        file.rdbuf()->pubsetbuf(buffer, sizeof(buffer));

        // Write file in chunks
        const std::string &data = it->second;
        size_t totalSize = data.size();
        size_t written = 0;
        
        while (written < totalSize) {
            size_t chunkSize = std::min(size_t(8192), totalSize - written);
            file.write(data.c_str() + written, chunkSize);
            
            if (file.fail()) {
                file.close();
                unlink(filePath.c_str()); // Delete partially written file
                logMsg(DEBUG, "Failed writing to file " + filePath + ": " + std::string(strerror(errno)));
                this->handleError(500, "Failed writing to uploaded file.");
                return false;
            }
            written += chunkSize;
        }

        file.close();
        if (file.fail()) {
            unlink(filePath.c_str());
            logMsg(DEBUG, "Failed closing file " + filePath + ": " + std::string(strerror(errno)));
            this->handleError(500, "Failed finalizing uploaded file.");
            return false;
        }
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
            // Create an HTML response with a nice notification
            std::string htmlResponse = "<!DOCTYPE html><html><head>";
            htmlResponse += "<title>Upload Success</title>";
            htmlResponse += "<link rel='stylesheet' href='https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css'>";
            htmlResponse += "<style>";
            htmlResponse += "body { font-family: 'Poppins', sans-serif; background: linear-gradient(135deg, #f5f7fa 0%, #c3cfe2 100%); min-height: 100vh; display: flex; align-items: center; justify-content: center; }";
            htmlResponse += ".notification { background: white; padding: 2rem; border-radius: 10px; box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1); text-align: center; max-width: 500px; width: 90%; }";
            htmlResponse += ".success-icon { color: #28a745; font-size: 3rem; margin-bottom: 1rem; }";
            htmlResponse += ".file-list { margin: 1rem 0; padding: 0.5rem; background: #f8f9fa; border-radius: 5px; }";
            htmlResponse += "</style>";
            htmlResponse += "</head><body>";
            htmlResponse += "<div class='notification'>";
            htmlResponse += "<div class='success-icon'>GG</div>";
            htmlResponse += "<h4 class='mb-3'>Upload Successful!</h4>";
            htmlResponse += "<p class='text-muted'>The following files were uploaded:</p>";
            htmlResponse += "<div class='file-list'>";
            
            for (std::map<std::string, std::string>::const_iterator it = files.begin(); it != files.end(); ++it) {
                htmlResponse += "<div class='text-primary'>" + it->first + "</div>";
            }
            
            htmlResponse += "</div>";
            htmlResponse += "<button onclick='window.history.back()' class='btn btn-primary mt-3'>Go Back</button>";
            htmlResponse += "</div>";
            htmlResponse += "<script>";
            htmlResponse += "setTimeout(function() { window.history.back(); }, 2000);";  // Auto redirect after 2 seconds
            htmlResponse += "</script>";
            htmlResponse += "</body></html>";

            this->_statusCode = 201;
            this->_headers["Content-Type"] = "text/html";
            this->_headers["Content-Length"] = intToString(htmlResponse.size());
            this->createHeader();
            this->sendHeader();
            this->sendData(htmlResponse.c_str(), htmlResponse.size());
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
    handleError(404, "No valid data provided in the POST request.");
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
	//std::cout << uri << std::endl;
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

static std::string joinPaths(const std::string& path1, const std::string& path2) {
    if (path1.empty()) return path2;
    if (path2.empty()) return path1;
    
    std::string result = path1;
    if (result[result.length()-1] == '/' && path2[0] == '/') {
        result = result.substr(0, result.length()-1);
    } else if (result[result.length()-1] != '/' && path2[0] != '/') {
        result += '/';
    }
    return result + path2;
}

bool HttpResponse::resolveUri(std::string &uri, bool &isDir) {
    // Remove any double slashes from the input URI
    size_t pos;
    while ((pos = uri.find("//")) != std::string::npos) {
        uri.erase(pos, 1);
    }

    std::string resolvePath;
    std::string location;
    isDir = false;
    saveLoc.inLoc = false;
    _isCGI = false;

    if (_isLocation) {
        location = matchLocation(uri);
        if (!location.empty()) {
            saveLoc.inLoc = true;
            Location *loc = this->getServer()->getLocation(location);
            saveLoc.loc = *loc;
            _allowedMethod = loc->getAllowedMethods();
            _directoryListing = loc->getAutoindex();
            
            if (uri.compare(location)) {
                std::string relativePath = uri.substr(uri.find(location) + location.size());
                resolvePath = joinPaths(loc->getRoot(), relativePath);
            } else {
                resolvePath = loc->getRoot();
            }

            // If autoindex is enabled, we want to show directory listing
            // regardless of index files
            struct stat s;
            if (stat(resolvePath.c_str(), &s) == 0 && S_ISDIR(s.st_mode) && _directoryListing) {
                uri = resolvePath;
                isDir = true;
                return true;
            }

            // Otherwise, proceed with normal index file handling
            if (!loc->getIndex().empty()) {
                std::string indexPath = joinPaths(resolvePath, loc->getIndex());
                if (access(indexPath.c_str(), F_OK) != -1) {
                    uri = indexPath;
                    isDir = false;
                    const std::vector<std::string>& cgiExtensions = loc->getCgiExtension();
                    if (!cgiExtensions.empty()) {
                        for (std::vector<std::string>::const_iterator it = cgiExtensions.begin();
                             it != cgiExtensions.end(); ++it) {
                            if (!it->empty() && uri.find(*it) != std::string::npos) {
                                _isCGI = true;
                                this->_cgiBin = loc->getCgiBin();
                                if (!loc->getAllowedMethods().empty()) {
                                    this->_allowedMethod = loc->getAllowedMethods();
                                }
                                return true;
                            }
                        }
                    }
                    return true;
                }
            }
            else if (!_directoryListing && loc->getIndex().empty()) {
                return (false);
            }
        } else if (uri.compare("/")) {
            resolvePath = uri;
        } else {
            resolvePath = joinPaths(_root, uri);
        }
    } else {
        resolvePath = joinPaths(_root, uri);
    }

    if (!hasAccess(resolvePath, isDir)) {
        return false;
    }

    for (std::vector<std::string>::const_iterator it = this->_cgiExt.begin();
         it != this->_cgiExt.end(); ++it) {
        if (!it->empty() && resolvePath.find(*it) != std::string::npos) {
            uri = resolvePath;
            _isCGI = true;
            return true;
        }
    }

    uri = resolvePath;
    return true;
}

std::string HttpResponse::matchLocation(std::string &requestUri) const {
    if (!_client || !_client->getServer()) {
        return "";
    }

    std::vector<std::string> *locations = _client->getServer()->getUri();
    if (!locations) {
        return "";
    }

    std::string bestMatch = "";
    size_t bestMatchLength = 0;

    for (std::vector<std::string>::iterator it = locations->begin(); 
         it != locations->end(); ++it) {
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

void HttpResponse::movedPermanently(std::string path) {
    this->_statusCode = 301;
    this->_headers["Location"] = path;
    this->createHeader();
    this->sendHeader();
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
	_response = _body;
	sendHeader();
	this->_readyToSend = true;
}


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
