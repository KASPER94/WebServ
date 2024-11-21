/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrigny <yrigny@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 15:16:43 by yrigny            #+#    #+#             */
/*   Updated: 2024/11/20 17:31:40 by yrigny           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Conf.hpp"
# include "Client.hpp"
# include <iostream>
# include <ostream>
# include <netinet/in.h> // sockaddr_in
# include <arpa/inet.h> // inet_addr, inet_ntoa
# include <cstring> // memset
# include <unistd.h> // close
# include <fcntl.h> // fcntl
# include <cstdlib> // exit, strtol
# include <string> // stoi
# include <vector>
# include <map>
# include <sys/epoll.h>

using namespace std;

class Client;
typedef std::map<int, Client> mapClient;

class Server
{
	public:
		Server();
		Server(ServerInfo& serverInfo);
		~Server();
		Server&	operator=(const Server& rhs);

		void	SetPort(const std::string& port);
		void	SetHost(std::string host);
		void	SetServerName(const std::string& serverName);
		void	SetRoot(const std::string& root);
		void	SetIndexes(std::string indexes);
		void	SetAutoIndex(const std::string& autoIndex);
		void	SetMethods(std::string methods);
		void	SetLocations(const std::string& locations);
		void	SetMaxBodySize(const std::string& maxBodySize);
		void	SetErrorPages(std::string errorPages);
		void	SetUploadPath(const std::string& uploadPath);
		// void	SetReturnURI(const std::string& returnURI);
		void	SetBinPaths(const std::string& binPaths);
		void	SetCgiExtensions(const std::string& cgiExtensions);
		void	InitServer();
		void	CreateSocket();
		void	SetSockAddr();
		void	SetReuseAddr();
		void	SetBind();
		void	SetNonBlocking();
		void	SetListen();
		void	SetClientConnection(int connFd);
		void	SetEpollFd(int epollFd);
		int		HandleRequest(int connFd);
		bool	IsConnectedClient(int connFd);
		void	AddClient(int connFd);
		bool	ProcessRequest(Client& client);
		bool	ModEpoll(int connFd, uint32_t events);
		
		uint16_t					GetPort() const;
		in_addr_t					GetHost() const;
		std::string					GetHostStr() const;
		std::string					GetServerName() const;
		std::string					GetRoot() const;
		std::vector<std::string>	GetIndexes() const;
		bool						GetAutoIndex() const;
		std::vector<HttpMethod>		GetMethods() const;
		std::vector<Location>		GetLocations() const;
		unsigned int				GetMaxBodySize() const;
		std::map<int, std::string>	GetErrorPages() const;
		std::string					GetUploadPath() const;
		std::string					GetBinPaths() const;
		std::string					GetCgiExtensions() const;
		int							GetListenFd() const;
		struct sockaddr_in			GetSockAddr() const;
		int							GetEpollFd() const;
		int							GetConnFd() const;

	private:
		uint16_t					_port;
		in_addr_t					_host;
		std::string 				_serverName;
		std::string 				_root;
		std::vector<std::string> 	_indexes;
		bool 						_autoIndex;
		std::vector<HttpMethod>		_methods;
		std::vector<Location>		_locations;
		unsigned int 				_maxBodySize;
		std::map<int, std::string>	_errorPages;
		std::string					_uploadPath;
		// std::map<int, std::string>	_returnURI;
		std::string					_binPaths;
		std::string					_cgiExtensions;
		int 						_listenFd;
		struct sockaddr_in			_sockAddr;
		int 						_connFd;
		int 						_epollFd;
		mapClient					_clients;
};

typedef std::vector<Server> VecServer;

std::ostream &operator<<(std::ostream &o, Server const &i);

#endif