/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 15:49:20 by skapersk          #+#    #+#             */
/*   Updated: 2024/11/15 11:01:35 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "websocket.hpp"
#include "Location.hpp"
#include "webserv.h"
#include <iostream>
#include <vector>
#include <map>

class Location;

class Server : public websocket {
	private:
		int								_port;
		std::string						_host;
		std::string						_name;
		bool							_directoryListing;
		std::string						_root;
		std::vector<std::string>		_indexes;
		std::string						_index;
		size_t							_maxBodySize;
		std::vector<std::string>		*_allowedMethod;
		std::vector<std::string>		_locationBlock;

		std::map<int, std::string>		_errorPages ;
		std::map<int,std::string>		_returnURI ;
		std::string						_uploadPath ;
		std::vector<std::string>		_binPath ;
		std::vector<std::string>		_cgiExtension ;
		std::map<std::string, Location> _locations;
		std::vector<std::string>		*_uri;

	public:
		Server();
		virtual ~Server();
		Server(const Server &cpy);

		Server &operator=(const Server &rhs);
		int connectToNetwork();

		// SETTER
		void	setPort(int port);
		void	setRoot(std::string root);
		void	setHostname(std::string host);
		void	setServerName(std::string name);
		void	setErrorPage(std::map<int, std::string>);
		void	setClientMaxBody(size_t body);
		void 	setAllowedMethods(std::vector<std::string> *methods);
		void	setSock();
		void	setUri(std::string uri);
		void	setReturnUri(std::map<int, std::string> returnUri);
		void	setIndexes(std::vector<std::string> indexes);


		// GETTER
		int								getPort();
		std::string						getHostname();
		std::string						getRoot();
		std::string						getServerName();
		std::map<int, std::string>		getErrorPage();
		size_t 							getClientMaxBody();
		std::vector<std::string> 		*getAllowedMethods() ;
		std::vector<std::string>		*getUri();
    	std::map<int, std::string> 		getReturnUri() const;
		std::vector<std::string>		getIndexes();

		void addLocation(const std::string &uri, const Location &location);
    	Location getLocation(const std::string &uri) const;
};

#endif
