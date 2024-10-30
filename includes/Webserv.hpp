/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 16:38:32 by peanut            #+#    #+#             */
/*   Updated: 2024/10/30 17:18:43 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <vector>
# include "Server.hpp"
# include <vector>
# include <poll.h>
#include <unistd.h>


class Server;

class Webserv {
    private:
        std::vector<Server>				_servers;
		// Webserv(const Webserv &cpy);

		// Webserv &operator=(const Webserv &rhs);
    public:
        Webserv();
        Webserv(std::vector<Server> const &servers) ;
		~Webserv();
		std::vector<Server> getAllServer();
		void initializeSockets();
		void sendResponse(int fd);
};

#endif
