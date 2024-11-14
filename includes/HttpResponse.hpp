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
# include	<map>

class Client;
class HttpRequest;

class HttpResponse {
	private:
		Client								*_client;
		int									_statusCode;
		std::map<int, std::string>			_errorPage;
		std::string 						_response;
		std::map<std::string, std::string>	_headers;
		std::string 						_body;
		bool								_readyToSend;

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

};

#endif
