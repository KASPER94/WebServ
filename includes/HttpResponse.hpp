/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 14:52:02 by skapersk          #+#    #+#             */
/*   Updated: 2024/11/14 12:29:02 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include <iostream>
# include "webserv.h"
# include <sys/stat.h>
# include <string>
# include <unistd.h>
# include <map>
# include <dirent.h>
# include "Mime.hpp"
# include <limits.h>

class Client;
class HttpRequest;
class Mime;
class Location;

typedef struct s_loc
{
	Location	loc;
	bool		inLoc;
} t_loc;

class HttpResponse {
	private:
		Client								*_client;
		int									_statusCode;
		std::map<int, std::string>			_errorPage;
		std::string 						_response;
		std::map<std::string, std::string>	_headers;
		std::string 						_body;
		bool								_readyToSend;
		bool								_directoryListing;
		std::string							_mime;
		std::string 						_cgiTmpFile;
		std::string							_root;
		std::string							_uri;
		std::string							_path;
		std::vector<std::string>			_indexes;
		std::string							_locPath;
		size_t								_maxBodySize;
		std::vector<std::string>		_allowedMethod;
		// bool								_directoryListing;
		std::map<int,std::string>				_returnURI;
		std::string							_uploadPath;
		// std::vector<std::string>					_cgiBin;
		std::string									_cgiBin;
		std::vector<std::string>					_cgiExt;
		char								**_cgiEnv;
		bool								_isLocation;
		bool								_isCGI;
		t_loc								saveLoc;

		void error(const std::string &message);
	public:
		HttpResponse();
		HttpResponse(Client *client);
		HttpResponse(const HttpResponse &cpy);
		~HttpResponse();
		HttpResponse &operator=(const HttpResponse &rhs);

		void	sendResponse();
		HttpRequest	*getRequest() const;
		std::string	getResponse();
		void handleError(int code, const std::string &message);
		void	setInfos();
		bool	initializeResponse();
		Server	*getServer() const;
		bool resolveUri(std::string &uri, bool &isDir);
		void movedPermanently(std::string path);
		void handleRedirect(int code, const std::string &uri);
		void	sendHeader();
		void	checkSend(int bytes);
		void	sendChunkEnd();
		void	sendFinalChunk();
		bool hasAccess(std::string &uri, bool &isDir);
		std::string matchLocation(std::string &requestUri) const;
		bool	methodAllowed(enum HttpMethod method);
		void	tryDeleteFile(std::string &uri);
		void	directoryListing(std::string path);
		void 	createHeader();
		void 	sendDirectoryPage(std::string path);
		bool findUriListing(std::string &uri, bool &isDir);
		void serveStaticFile(const std::string &uri);
		int	sendData(const void *data, int len);
		void handleCGI(std::string uri);
		bool executeCGI(const std::string &uri);
		bool handleUpload();
		void handlePostRequest();
		void prepareCGIEnvironment(std::vector<std::string> &env);
		char **createEnv(HttpRequest *request, std::string uri);
		char **mergeEnvironments(char **originalEnv, char **cgiEnv);
};

#endif
