/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 15:49:20 by skapersk          #+#    #+#             */
/*   Updated: 2024/11/20 17:21:08 by skapersk         ###   ########.fr       */
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
		bool							_autoindex;
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
		std::string						_cgiBin ;
		std::vector<std::string>		_cgiExtensions ;
		// std::string						_cgiExtension ;
		std::map<std::string, Location*> _locations;
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
		void 	setCgiBin(const std::string &cgiBin);
		void 	setCgiExtension(const std::vector<std::string> &cgiExtension);
		void 	setUploadPath(const std::string &uploadPath);
		void 	setAutoindex(bool autoindex);


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
		std::string getCgiBin() const;
		std::vector<std::string> getCgiExtension() const;
		std::string getUploadPath() const;
		bool getAutoindex() const ;

		void reset();
		void addLocation(const std::string &uri, const Location &location);
    	Location *getLocation(const std::string &uri) const;
    	std::map<std::string, Location*> getLocs() const;
		std::map<std::string, Location*> returnLoc();
};

std::ostream &operator<<(std::ostream &o, Server &server);

#endif
