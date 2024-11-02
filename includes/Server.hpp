/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 15:49:20 by skapersk          #+#    #+#             */
/*   Updated: 2024/11/02 00:41:41 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "websocket.hpp"
#include "webserv.h"
#include <iostream>
#include <vector>
#include <map>

class Server : public websocket {
	private:
		int								_port;
		std::string					_host;
		std::string						_name;
		bool							_directoryListing;
		std::string						_root;
		std::vector<std::string>		_indexes;
		std::string						_index;
		long long						_maxBodySize;
		std::vector<std::string>		*_allowedMethod;
		std::vector<std::string>		_locationBlock;

		std::map<int, std::string>		_errorPages ;
		std::map<int,std::string>		_returnURI ;
		std::string						_uploadPath ;
		std::vector<std::string>		_binPath ;
		std::vector<std::string>		_cgiExtension ;
	public:
		Server();
		virtual ~Server();
		Server(const Server &cpy);

		Server &operator=(const Server &rhs);
		int connectToNetwork();

		// SETTER
		void	setPort(int port);
		void	setHostname(std::string host);
		void	setServerName(std::string name);
		void	setErrorPage(std::map<int, std::string>);
		void	setClientMaxBody(size_t body);
		void 	setAllowedMethods(std::vector<std::string> *methods);
		void	setSock();


		// GETTER
		int		getPort();
		std::string		getHostname();
		std::string		getServerName();
		std::map<int, std::string>		getErrorPage();
		size_t getClientMaxBody();
		std::vector<std::string> *getAllowedMethods() ;
};

#endif
