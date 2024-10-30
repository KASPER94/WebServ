/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   websocket.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 15:24:17 by skapersk          #+#    #+#             */
/*   Updated: 2024/10/30 12:06:48 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <cerrno>
#include <cstdio>

class websocket {
	protected:
		int	_sock;
		struct sockaddr_in _address;
		int _addrlen;
		int _connection;

	public:
		websocket(int domain, int service, int protocol, int port, unsigned long interface);
		virtual ~websocket();
		websocket(const websocket &cpy);

		websocket &operator=(const websocket &rhs);
		virtual int connectToNetwork() = 0;
		void initializeConnection();
		void	testConnection(int item);
		int		getSock();
		int		getConnect();
};
