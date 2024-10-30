/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 16:38:32 by peanut            #+#    #+#             */
/*   Updated: 2024/10/30 11:55:54 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <vector>
# include "Server.hpp"

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
};

#endif
