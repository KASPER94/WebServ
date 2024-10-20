/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   websocket.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 15:24:17 by skapersk          #+#    #+#             */
/*   Updated: 2024/10/20 15:58:46 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>

class websocket {
	private:
		int	_fd;
		struct sockaddr_in address;
		int addrlen;
		int connection;

	public:
		websocket(int domain, int service, int protocol);
		~websocket();
		websocket(const websocket &cpy);

		websocket &operator=(const websocket &rhs);
		virtual int connection() = 0;
};
