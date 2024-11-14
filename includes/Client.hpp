/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 15:49:48 by skapersk          #+#    #+#             */
/*   Updated: 2024/11/14 13:03:40 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Server.hpp"
# include "HttpRequest.hpp"
# include "HttpResponse.hpp"
# include <map>


class Server;
class HttpRequest;
class HttpResponse;

class Client {
	private:
		HttpRequest		*_request;
		HttpResponse		*_response;
		int				_fd;
		bool			_error;
		Server			*_servers;

		// std::string _requestData;        // Contient les données de la requête reçues
		// bool _headersParsed;     // Indicateur si les en-têtes ont été analysés
		// std::map<std::string, std::string> _headers; // Contient les en-têtes HTTP



	public:
		Client();
		// Client(int fd);
		Client(int fd, Server *servers);
		// Client(std::string buffer, int fd);
		Client(const Client &cpy);
		~Client();

		Client &operator=(const Client &rhs);

		HttpRequest		*getRequest() const;
		HttpResponse		*getResponse() const;
		std::string		getResponsestr() const;
		Server			*getServer() const;
		void 			setRequest(HttpRequest& request);
		HttpRequest* getRequestPtr() const { return _request; };

    	// HttpRequest& 	getRequest();
		bool			appendRequest(const char *data, int bytes);
		// std::string 	getFullRequest() const;
		// bool 			hasCompleteBody();
		// void 			parseHeaders();
		// bool 			isChunkedBodyComplete();
		// std::string		getRawRequest() const;
		void			sendResponse();
		bool	error() const;
		int getFd() const;
		void setFd(int fd);
};

std::ostream &operator<<(std::ostream &o, const Client &client);

#endif
