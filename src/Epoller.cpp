/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Epoller.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrigny <yrigny@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 10:50:20 by yrigny            #+#    #+#             */
/*   Updated: 2024/11/13 18:03:51 by yrigny           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Epoller.hpp"

Epoller::Epoller(VecServer& servers, int maxEvent) : _epollFd(epoll_create1(0)), _events(maxEvent), _servers(servers)
{
	if (_epollFd == -1)
	{
		Log::LogMsg(ERROR, "epoll_create1() failed");
		exit(1);
	}
	std::cout << "Epoller created, epollFd: " << _epollFd << std::endl;
}

Epoller::~Epoller()
{
	close(_epollFd);
}

void	Epoller::InitEpoller()
{
	if (!this->AddServerSockets())
	{
		Log::LogMsg(ERROR, "AddServerSockets() failed");
		close(_epollFd);
		exit(1);
	}
	// if (!this->AddFd(STDIN_FILENO, EPOLLIN))
	// {
	// 	Log::LogMsg(ERROR, "AddFd() failed");
	// 	close(_epollFd);
	// 	exit(1);
	// }
	Log::LogMsg(INFO, "Start webserv...");
	while (this->EpollWait(-1) != -1)
		;
	Log::LogMsg(INFO, "Stop webserv...");
	for (size_t i = 0; i < _servers.size(); i++)
	{
		this->DelFd(_servers[i].GetListenFd());
		close(_servers[i].GetListenFd());
		this->DelFd(_servers[i].GetConnFd());
		close(_servers[i].GetConnFd());
	}
}

bool	Epoller::AddServerSockets()
{
	for (size_t i = 0; i < _servers.size(); i++)
	{
		std::cout << _servers[i];
		if (!AddFd(_servers[i].GetListenFd(), EPOLLIN | EPOLLET))
			return false;
	}
	return true;
}

bool	Epoller::AddFd(int fd, uint32_t events)
{
	if (fd < 0)
		return false;
	struct epoll_event event;
	event.data.fd = fd;
	event.events = events;
	std::cout << "Add to epoll: Fd: " << fd << ", events: " << EventToStr(events) << std::endl;
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &event) == -1)
	{
		Log::LogMsg(ERROR, "epoll_ctl() failed");
		close(fd);
		close(_epollFd);
		exit(1);
	}
	return true;
}

bool	Epoller::ModFd(int fd, uint32_t events)
{
	if (fd < 0)
		return false;
	struct epoll_event event;
	event.data.fd = fd;
	event.events = events;
	if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, fd, &event) == -1)
	{
		Log::LogMsg(ERROR, "epoll_ctl() failed");
		close(fd);
		close(_epollFd);
		exit(1);
	}
	return true;
}

bool	Epoller::DelFd(int fd)
{
	if (fd < 0)
		return false;
	if (epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, NULL) == -1)
	{
		Log::LogMsg(ERROR, "epoll_ctl() failed");
		close(fd);
		close(_epollFd);
		exit(1);
	}
	return true;
}

int	Epoller::MatchListenFd(int fd)
{
	for (size_t i = 0; i < _servers.size(); i++)
	{
		if (_servers[i].GetListenFd() == fd)
			return i;
	}
	return -1;
}

int	Epoller::MatchClientFd(int fd)
{
	for (size_t i = 0; i < _servers.size(); i++)
	{
		if (_servers[i].GetConnFd() == fd)
			return i;
	}
	return -1;
}

int		Epoller::EpollWait(int timeoutMs)
{
	int nfds;

	if ((nfds = epoll_wait(_epollFd, &_events[0], static_cast<int>(_events.size()), timeoutMs)) == -1)
	{
		Log::LogMsg(ERROR, "epoll_wait() failed");
		return -1;
	}
	for (int i = 0; i < nfds; i++)
	{
		int sockFd = _events[i].data.fd;
		if (_events[i].events & (EPOLLERR | EPOLLHUP) || !(_events[i].events & EPOLLIN))
		{
			Log::LogMsg(ERROR, "epoll error, events: " + Log::ToString(_events[i].events));
			close(sockFd);
			return 0;
		}
		int serverIdx = this->MatchListenFd(sockFd);
		if ((_events[i].events & EPOLLIN) && serverIdx >= 0) // new connection
		{
			if (!this->InitConnection(sockFd, serverIdx))
				return -1;
		}
		else // existing connection
		{
			serverIdx = this->MatchClientFd(sockFd);
			if (!this->RequestHandler(sockFd, serverIdx))
				return -1;
		}
	}
	return 0;
}

bool	Epoller::InitConnection(int sockFd, int serverIdx)
{
	struct sockaddr_in sockAddr;
	socklen_t sockAddrLen = sizeof(sockAddr);
	Log::LogMsg(INFO, "New connection at port " + Log::ToString(_servers[serverIdx].GetPort()));
	
	int connFd = accept(sockFd, (struct sockaddr*)&sockAddr, &sockAddrLen);
	if (connFd == -1)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return true;
		else
		{
			Log::LogMsg(ERROR, "accept() failed");
			return false;
		}
	}
	if (!this->SetNonBlocking(connFd))
		return false;
	_servers[serverIdx].SetClientConnection(connFd);
	
	Log::LogMsg(INFO, "New socket " + Log::ToString(connFd) + " created for communication");
	if (!this->AddFd(connFd, EPOLLIN))
		return false;
	return true;
}

bool	Epoller::SetNonBlocking(int connFd)
{
	int oldOption = fcntl(connFd, F_GETFL);
	int newOption = oldOption | O_NONBLOCK;
	if (fcntl(connFd, F_SETFL, newOption) == -1)
	{
		Log::LogMsg(ERROR, "fcntl() failed");
		return false;
	}
	return true;
}

bool	Epoller::RequestHandler(int connFd, int serverIdx)
{
	_servers[serverIdx].HandleRequest(connFd);
	return false;
}

int		Epoller::GetEventFd(size_t i) const
{
	return _events[i].data.fd;
}

uint32_t	Epoller::GetEvents(size_t i) const
{
	return _events[i].events;
}

std::string	Epoller::EventToStr(uint32_t events) const
{
	// uint32_t events = _events[i].events;
	std::string str;
	if (events & EPOLLIN)
		str += "EPOLLIN ";
	if (events & EPOLLOUT)
		str += "EPOLLOUT ";
	if (events & EPOLLRDHUP)
		str += "EPOLLRDHUP ";
	if (events & EPOLLPRI)
		str += "EPOLLPRI ";
	if (events & EPOLLERR)
		str += "EPOLLERR ";
	if (events & EPOLLHUP)
		str += "EPOLLHUP ";
	if (events & EPOLLET)
		str += "EPOLLET ";
	if (events & EPOLLONESHOT)
		str += "EPOLLONESHOT ";
	return str;
}
