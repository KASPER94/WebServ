/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: peanut <peanut@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 15:49:20 by skapersk          #+#    #+#             */
/*   Updated: 2024/10/23 11:36:07 by peanut           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "websocket.hpp"
#include "webserv.h"
#include <iostream>
#include <vector>
#include <map>

class server : public websocket {
	private:	
		int								_port;
		std::string						_host;
		std::string						_name;
		bool							_directoryListing;
		std::string						_root;
		std::vector<std::string>		_indexes;
		std::string						_index;	
		long long						_maxBodySize;
		std::vector<enum HttpMethod>	_allowedMethod;
		std::vector<std::string>		_locationBlock;

		std::map<int, std::string>		_errorPages ;
		std::map<int,std::string>		_returnURI ;
		std::string						_uploadPath ;
		std::vector<std::string>		_binPath ;
		std::vector<std::string>		_cgiExtension ;
	public:
		server();
		~server();
		server(const server &cpy);

		server &operator=(const server &rhs);

		int connectToNetwork(int sock, struct sockaddr_in address);
};
