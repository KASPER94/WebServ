/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mank <ael-mank@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 15:49:48 by skapersk          #+#    #+#             */
/*   Updated: 2024/12/11 13:56:42 by ael-mank         ###   ########.fr       */
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
		char			**cgiEnv;
		time_t _lastActivityTime; // Temps de la dernière activité
    	static const int _timeout = 60; 


	public:
		Client();
		Client(int fd, Server *servers);
		Client(const Client &cpy);
		~Client();
		Client &operator=(const Client &rhs);

		HttpRequest		*getRequest() const;
		HttpResponse		*getResponse() const;
		std::string		getResponsestr() const;
		Server			*getServer() const;
		void 			setRequest(HttpRequest& request);
		HttpRequest* getRequestPtr() const { return _request; };
		bool			appendRequest(const char *data, int bytes);
		void			sendResponse();
		bool	getError() const;
		void	setError();
		int getFd() const;
		void setFd(int fd);
		void resetForNextRequest();
		void updateLastActivity();
		bool isTimeout();
		time_t getLastActivityTime() const;
		int getTimeout() const;
		char **getCgiEnv();
		void setCgiEnv(char **mergedEnv);
		void setRequest(HttpRequest* request) {
        	_request = request;
    	}
		void setResponse(HttpResponse* response) {
			_response = response;
		}		
};

std::ostream &operator<<(std::ostream &o, const Client &client);

#endif
