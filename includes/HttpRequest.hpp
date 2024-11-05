/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/02 19:10:41 by skapersk          #+#    #+#             */
/*   Updated: 2024/11/05 16:13:12 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include "webserv.h"
# include "Server.hpp"
# include "Client.hpp"
# include <map>

class Server;
class Client;

class HttpRequest {
	private:
		Client	*_client;
		std::string					_rawRequest;
		std::string					_path;
		bool						_goodRequest;
		enum HttpMethod				_method;
		std::string					_uri;
		std::string					_version;
		std::vector<std::string>	_acceptedMimes;
		bool						_keepAliveConnection;
		
		std::string _requestData;        // Contient les données de la requête reçues
		bool _headersParsed;     // Indicateur si les en-têtes ont été analysés
		std::map<std::string, std::string> _headers; // Contient les en-têtes HTTP

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
		std::string		getUri() const;
		bool			keepAlive() const;
		// void 			parseHttpRequest(const std::string& request);
		void 			parseHttpRequest();
		std::string		HttpMethodTostring();
		HttpMethod 		stringToHttpMethod(const std::string &methodStr);
		bool			appendRequest(const char *data, int bytes);
		std::string 	getFullRequest() const;
		bool 			hasCompleteBody();
		void 			parseHeaders();
		bool 			isChunkedBodyComplete();
};

std::ostream &operator<<(std::ostream &o, const HttpRequest &request);

#endif
