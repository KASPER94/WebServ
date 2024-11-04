/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 15:49:48 by skapersk          #+#    #+#             */
/*   Updated: 2024/11/04 14:31:37 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Server.hpp"
# include "HttpRequest.hpp"
// # include "HttpResponse.hpp"

class Server;
class HttpRequest;

class Client {
	private:
		HttpRequest		*_request;
		HttpRequest		*_response;
		int				_fd;


	public:
		Client();
		Client(int fd);
		Client(std::string buffer, int fd);
		Client(const Client &cpy);
		~Client();

		Client &operator=(const Client &rhs);

		HttpRequest		*getRequest() const;
		HttpRequest		*getResponse() const;
		void 			setRequest(HttpRequest& request);
		HttpRequest* getRequestPtr() const { return _request; };

    	HttpRequest& 	getRequest();   
		// std::string		getRawRequest() const;
		// bool			appendRequest(const std::string str);
		// void			sendResponse();
		int getFd() const;
		void setFd(int fd);
};

std::ostream &operator<<(std::ostream &o, const Client &client);

#endif