/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mank <ael-mank@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 16:38:32 by peanut            #+#    #+#             */
/*   Updated: 2024/12/11 13:57:57 by ael-mank         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <vector>
# include "Server.hpp"
# include "Client.hpp"
# include <vector>
# include <map>
# include <sys/epoll.h>
# include <unistd.h>


class Server;
class Client;

class Webserv {
    private:
		int 	_epollfd;
        std::vector<Server>				_servers;
		std::map<int, Client*>			_clients;
		std::map<int, Server*> 			_serverSockets;

    public:
        Webserv();
        Webserv(std::vector<Server> const &servers) ;
		~Webserv();
		std::vector<Server> &getAllServer();
		std::map<int, Client*>& getClients() { 
			return _clients; 
		}
		void initializeSockets();
		void sendResponse(int fd);
		void getRequest(int clientSock);
		bool isServerSocket(int sock) const ;
		void deleteClient(int fd);
		void checkClientTimeouts();
};

#endif
