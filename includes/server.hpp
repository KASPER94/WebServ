/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 15:49:20 by skapersk          #+#    #+#             */
/*   Updated: 2024/10/20 16:55:39 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "websocket.hpp"

class server : public websocket {
	private:

	public:
		server(int domain, int service, int proctocol, int port, unsigned long interface);
		~server();
		server(const server &cpy);

		server &operator=(const server &rhs);
		int connectToNetwork(int sock, struct sockaddr_in address);
}
