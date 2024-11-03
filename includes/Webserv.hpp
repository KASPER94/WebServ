/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 16:38:32 by peanut            #+#    #+#             */
/*   Updated: 2024/11/03 14:36:34 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <vector>
# include "Server.hpp"
# include "Client.hpp"
# include <vector>
# include <map>
// # include <poll.h>
# include <sys/epoll.h>
# include <unistd.h>


class Server;
class Client;

class Webserv {
    private:
        std::vector<Server>				_servers;
		// Client _clients;
		std::map<int, Client>			_clients;
		// Webserv(const Webserv &cpy);

		// Webserv &operator=(const Webserv &rhs);
    public:
        Webserv();
        Webserv(std::vector<Server> const &servers) ;
		~Webserv();
		std::vector<Server> getAllServer();
		void initializeSockets();
		void sendResponse(int fd);
		void getRequest(int clientSock);
};

#endif
