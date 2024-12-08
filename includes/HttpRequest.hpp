/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/02 19:10:41 by skapersk          #+#    #+#             */
/*   Updated: 2024/12/07 21:55:33 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include "webserv.h"
# include "Server.hpp"
# include "Client.hpp"
# include <map>
# include <vector>

class Server;
class Client;

typedef struct s_query
{
	std::string					strquery;
	std::map<std::string, std::string>	params;
} t_query;

typedef struct s_form
{
	std::string					strform;
	std::map<std::string, std::string>	form;
} t_form;

class HttpRequest {
	private:
		Client								*_client;
		size_t								_receivedBodySize;
		bool								_completed;
		bool								_endRequested;
		bool								_tooLarge;
		bool								_isGood;
		std::string							_rawRequest;
		std::string							_path;
		bool								_goodRequest;
		enum HttpMethod						_method;
		std::string							_uri;
		t_query								_query;
		t_form								_form;
		std::string							_version;
		std::vector<std::string>			_acceptedMimes;
		std::string							_contentType;
		size_t								_contentLen;
		std::string							_userAgent;
		std::string							_host;
		std::string							_cookie;
		std::string							_boundary;
		std::string							_plainTextBody;
		bool								_keepAliveConnection;
		std::map<std::string, std::string> 	_formData; // Pour les données texte du formulaire
    	std::map<std::string, std::string>	_fileData;

		std::string					 		_requestData;        // Contient les données de la requête reçues
		bool 								_headersParsed;     // Indicateur si les en-têtes ont été analysés
		std::map<std::string, std::string>	_headers; // Contient les en-têtes HTTP

		// bool	isFullRequest();
		// void	parseHttpRequest();
		// void	parseRequestLine(std::string line);
		// void	setMethod(std::string str);
		// void	parseAcceptedMimes(std::string line);
		// void	parseConnection(std::string line);
	public:
		HttpRequest();
		HttpRequest(Client *client);
		// HttpRequest(const std::string &req);
		// HttpRequest(const HttpRequest &cpy);
		~HttpRequest();

		HttpRequest	&operator=(const HttpRequest &rhs);

		Client			*getClient() const;
		// std::string		getRawRequest() const;
		// bool			appendRequest(const std::string str);
		// bool			isGood() const;
		enum HttpMethod	getMethod() const;
		void			getUri();
		bool			keepAlive() const;
		void			setQuery(std::string query);
		void			parseUrn(std::string queryString);
		void 			initializeHeaderParsers();
		void 			parseAcceptedMimes(std::string &line);
		void 			parseConnection(std::string &line);
		void 			parseUserAgent(std::string &line);
		void			parseContentLen(std::string &line);
		void 			parseHost(std::string &line);
		void 			parseCookie(std::string &line);
		void 			parseContentType(std::string &line);
		void			processMultipartData();
		void			decodeFormData();
		void 			setForm(std::string form);
		void			getForm(std::string &str);
		void			decodeUrl();


		void 			parseHttpRequest();
		std::string		HttpMethodTostring();
		HttpMethod 		stringToHttpMethod(const std::string &methodStr);
		bool			appendRequest(const char *data, int bytes);
		std::string 	getFullRequest() const;
		bool 			hasCompleteBody();
		void 			parseHeaders();
		bool 			isChunkedBodyComplete();
		void 			setHeader(std::map<std::string, std::string> *header) const;
		std::string 	getHeader(std::string find);
		std::map<std::string, std::string> 	getHeaders();
		bool		 	getCompleted() const;
		bool		 	getEnd() const;
		std::map<std::string, std::string> &getFormData() const;
		std::map<std::string, std::string> &getfileData() const;
		bool tooLarge() const ;
		bool	isGood() const;
		std::string	returnURI();
		size_t getContentLen() const;
		std::string	returnPATH();
		std::map<std::string, std::string> &getFileData();
		std::map<std::string, std::string> &getFormData();
		s_query getQueryString();
};

std::ostream &operator<<(std::ostream &o, const HttpRequest &request);

#endif
