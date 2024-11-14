/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrigny <yrigny@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 13:14:47 by yrigny            #+#    #+#             */
/*   Updated: 2024/10/30 19:19:20 by yrigny           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/socket.h> // socket, bind, listen, accept
#include <sys/epoll.h> // epoll_create, epoll_ctl, epoll_wait
#include <netinet/in.h> // sockaddr_in
#include <cstring> // memset
#include <unistd.h> // close
#include <iostream> // cout, cerr, perror
#include <fcntl.h> // fcntl

int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

void	addfd(int epollfd, int fd, bool oneshot)
{
	struct epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
	if (oneshot)
		event.events |= EPOLLONESHOT;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event) == -1)
	{
		perror("epoll_ctl() failed");
		close(fd);
		close(epollfd);
		exit(1);
	}
	setnonblocking(fd);
}

int main()
{
	// create a socket descriptor
	int listen_fd = socket(PF_INET, SOCK_STREAM, 0); // IPv4, TCP
	if (listen_fd == -1)
	{
		perror("socket() failed");
		return 1;
	}

	// set the socket address
	struct sockaddr_in addr; // IPv4 address
	memset(&addr, sizeof(addr), 0); // zero out the structure
	addr.sin_family = AF_INET; // IPv4
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // 127.0.0.1, host to network long
	addr.sin_port = htons(8080); // host to network short

	// Allow socket descriptor to be reuseable
	int flag = 1;
	if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) == -1)
	{
		perror("setsockopt() failed");
		close(listen_fd);
		return 1;
	}

	// bind the socket
	if (bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
	{
		perror("bind() failed");
		close(listen_fd);
		return 1;
	}
	
	// listen
	if (listen(listen_fd, 10) == -1) // 10 is the maximum size of the queue
	{
		perror("listen");
		close(listen_fd);
		return 1;
	}

	int epollfd; // create the unique epoll instance
	if (epollfd = epoll_create(1) == -1)
	{
		perror("epoll_create() failed");
		close(listen_fd);
		return 1;
	}

	// register the listen_fd to the epoll instance
	addfd(epollfd, listen_fd, false);

	int nfds; // number of file descriptors ready
	struct sockaddr_in client_addr;
	socklen_t client_addrlen;
	char buf[1024];
	struct sockaddr_in* ptr;
	// create a buffer to store the ready events
	struct epoll_event events[1024];

	while (1)
	{
		if (nfds = epoll_wait(epollfd, events, 1024, -1) == -1)
		{
			perror("epoll_wait() failed");
			close(listen_fd);
			close(epollfd);
			return 1;
		}
		for (int i = 0; i < nfds; i++)
		{
			int sockfd = events[i].data.fd;
			if (sockfd == listen_fd) // new connection
			{
				client_addrlen = sizeof(struct sockaddr_in);
				int connfd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_addrlen);
				if (connfd == -1)
				{
					perror("accept() failed");
					close(listen_fd);
					close(epollfd);
					return 1;
				}
				addfd(epollfd, connfd, false);
			}
			else
			{}
		}
	}
	return 0;
}