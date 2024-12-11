/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 15:16:29 by yrigny            #+#    #+#             */
/*   Updated: 2024/12/03 16:33:04 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Log.hpp"

Server::Server() // should be connected to the .conf parsing result later
{
	/* fill in the attributes of server */
	SetPort("8080");
	SetHost("localhost");
	SetServerName("webserv.fr");
	SetRoot("www/_default/");
	SetIndexes("index.html index.htm index.php");
	SetAutoIndex("off");
	SetMethods("GET POST");
	SetLocations("");
	SetMaxBodySize("4096");
	SetErrorPages("404 /_default/404.html");
	SetUploadPath("upload/files/");
	SetBinPaths("");
	SetCgiExtensions("");
}

Server::Server(ServerInfo& serverInfo)
{
	SetPort(serverInfo.listen);
	SetHost(serverInfo.host);
	SetServerName(serverInfo.serverName);
	SetRoot(serverInfo.root);
	SetIndexes(serverInfo.indexes);
	SetAutoIndex(serverInfo.autoIndex);
	SetMethods(serverInfo.methods);
	SetLocations("");
	SetMaxBodySize(serverInfo.clientMaxBodySize);
	SetErrorPages(serverInfo.errorPages);
	SetUploadPath(serverInfo.uploadDir);
	SetBinPaths(serverInfo.cgiBin);
	SetCgiExtensions(serverInfo.cgiExtension);
}

Server::~Server()
{
}

Server&	Server::operator=(Server const &rhs)
{
	_port = rhs.GetPort();
	_host = rhs.GetHost();
	_serverName = rhs.GetServerName();
	_root = rhs.GetRoot();
	_indexes = rhs.GetIndexes();
	_autoIndex = rhs.GetAutoIndex();
	_methods = rhs.GetMethods();
	_locations = rhs.GetLocations();
	_maxBodySize = rhs.GetMaxBodySize();
	_errorPages = rhs.GetErrorPages();
	_uploadPath = rhs.GetUploadPath();
	_listenFd = rhs.GetListenFd();
	_sockAddr = rhs.GetSockAddr();
	_connFd = rhs.GetConnFd();
	_binPaths = rhs.GetBinPaths();
	_cgiExtensions = rhs.GetCgiExtensions();
	return *this;
}

void	Server::InitServer()
{
	CreateSocket();
	SetSockAddr();
	SetReuseAddr();
	SetBind();
	SetNonBlocking();
	SetListen();
	SetClientConnection(-1);
	SetEpollFd(-1);
}

void	Server::SetPort(const std::string& port)
{
	_port = static_cast<uint16_t>(strtol(port.c_str(), NULL, 10));
}

void	Server::SetHost(std::string host)
{
	if (host == "localhost")
		host = "127.0.0.1";
	_host = inet_addr(host.c_str());
}

void	Server::SetServerName(const std::string& serverName)
{
	_serverName = serverName;
}

void	Server::SetRoot(const std::string& root)
{
	_root = root;
}

void	Server::SetIndexes(std::string indexes)
{
	while (indexes.find(",") != std::string::npos)
	{
		_indexes.push_back(indexes.substr(0, indexes.find(",")));
		indexes = indexes.substr(indexes.find(", ") + 2);
	}
	if (indexes[0])
		_indexes.push_back(indexes);
}

void	Server::SetAutoIndex(const std::string& autoIndex)
{
	if (autoIndex != "on" && autoIndex != "off")
	{
		Log::LogMsg(ERROR, "Invalid autoindex value");
		exit(1);
	}
	if (autoIndex == "on")
		_autoIndex = true;
	else
		_autoIndex = false;
}

void	Server::SetMethods(std::string methods_str)
{
	while (methods_str.find(" ") != std::string::npos)
	{
		HttpMethod method;
		if (methods_str.substr(0, methods_str.find(" ")) == "GET")
			method = GET;
		else if (methods_str.substr(0, methods_str.find(" ")) == "POST")
			method = POST;
		else if (methods_str.substr(0, methods_str.find(" ")) == "DELETE")
			method = DELETE;
		else
		{
			Log::LogMsg(ERROR, "Invalid method");
			exit(1);
		}
		_methods.push_back(method);
		methods_str = methods_str.substr(methods_str.find(" ") + 1);
	}
	if (methods_str[0])
	{
		HttpMethod method;
		if (methods_str == "GET")
			method = GET;
		else if (methods_str == "POST")
			method = POST;
		else if (methods_str == "DELETE")
			method = DELETE;
		else
		{
			Log::LogMsg(ERROR, "Invalid method");
			exit(1);
		}
		_methods.push_back(method);
	}
}

void	Server::SetLocations(const std::string& locations)
{
	(void)locations;
	_locations.clear();
}

void	Server::SetMaxBodySize(const std::string& maxBodySize)
{
	_maxBodySize = static_cast<unsigned int>(strtol(maxBodySize.c_str(), NULL, 10));
}

void	Server::SetErrorPages(std::string errorPages)
{
	while (errorPages.find(" ") != std::string::npos)
	{
		std::string code_str = errorPages.substr(0, errorPages.find(" "));
		int code = static_cast<int>(strtol(code_str.c_str(), NULL, 10));
		std::string path = errorPages.substr(errorPages.find(" ") + 1);
		_errorPages[code] = path;
		errorPages = errorPages.substr(errorPages.find(" ") + 1);
	}
}

void	Server::SetUploadPath(const std::string& uploadPath)
{
	_uploadPath = uploadPath;
}

void	Server::SetBinPaths(const std::string& binPaths)
{
	_binPaths = binPaths;
}

void	Server::SetCgiExtensions(const std::string& cgiExtensions)
{
	_cgiExtensions = cgiExtensions;
}

void	Server::CreateSocket()
{
	_listenFd = socket(PF_INET, SOCK_STREAM, 0);
	if (_listenFd == -1)
	{
		Log::LogMsg(ERROR, "socket() failed");
		exit(1);
	}
}

void	Server::SetSockAddr()
{
	memset(&_sockAddr, 0, sizeof(_sockAddr));
	_sockAddr.sin_family = AF_INET;
	_sockAddr.sin_addr.s_addr = _host;
	_sockAddr.sin_port = htons(_port);
}

void	Server::SetReuseAddr()
{
	int flag = 1;
	if (setsockopt(_listenFd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) == -1)
	{
		Log::LogMsg(ERROR, "setsockopt() failed");
		close(_listenFd);
		exit(1);
	}
}

void	Server::SetBind()
{
	cout << "Binding Fd " << _listenFd << " with port " << _port << " host " << inet_ntoa(*((struct in_addr*)&_host)) << endl;
	if (bind(_listenFd, (struct sockaddr*)&_sockAddr, sizeof(_sockAddr)) == -1)
	{
		Log::LogMsg(ERROR, "bind() failed");
		close(_listenFd);
		exit(1);
	}
}

void	Server::SetNonBlocking()
{
    int old_option = fcntl(_listenFd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    if (fcntl(_listenFd, F_SETFL, new_option) == -1)
	{
		Log::LogMsg(ERROR, "fcntl() failed");
		close(_listenFd);
		exit(1);
	}
}

void	Server::SetListen()
{
	if (listen(_listenFd, 10) == -1)
	{
		Log::LogMsg(ERROR, "listen() failed");
		close(_listenFd);
		exit(1);
	}
}

void	Server::SetClientConnection(int connFd)
{
	_connFd = connFd;
}

void	Server::SetEpollFd(int epollFd)
{
	_epollFd = epollFd;
}

int	Server::HandleRequest(int connFd)
{
	char buf[_maxBodySize + 1];
	memset(buf, 0, _maxBodySize + 1);

	if (!IsConnectedClient(connFd))
		AddClient(connFd);
	Client& client = _clients[connFd];
	
	int bytes = recv(connFd, buf, _maxBodySize, 0);
	std::cout << "number of bytes read: " << bytes << std::endl;
	if (bytes > 0)
	{
		buf[bytes] = 0;
		std::cout << "----[ request ]----" << std::endl;
		std::cout << buf << std::endl;
		std::cout << "-------------------" << std::endl;
		ReqStatus check = client.RecvRequest(std::string(buf));
		if (check == REQ_COMPLETE)
		{
			ProcessRequest(client); // bool, can return a status if needed
			return 0;
		}
		if (check == REQ_INCOMPLETE) // keep reading
			return 0;
		if (check == REQ_BODY_TOO_LARGE)
		{
			// stop reading the connfd, send 413
			
			// close the connection
			close(connFd);
			_clients.erase(connFd);
			return 0;
		}
	}
	else if (bytes == 0)
	{
		Log::LogMsg(INFO, "Client disconnected");
		return -1;
	}
	else if (bytes == -1)
	{
		Log::LogMsg(ERROR, "recv() failed");
		return -1;
	}
	return 0;
}

bool	Server::IsConnectedClient(int connFd)
{
	if (_clients.find(connFd) == _clients.end())
		return false;
	return true;
}

void	Server::AddClient(int connFd)
{
	_clients[connFd] = Client(this, connFd);
}

bool	Server::ProcessRequest(Client& client)
{
	client.ParseRequest();
	client.SearchLocation();
	client.PrepareResponse();
	std::string& response = client.GetResponse();
	// std::cout << "----[ response ]----"<< std::endl;
	// std::cout << response << std::endl;
	// std::cout << "--------------------"<< std::endl;
	// set the events to EPOLLOUT
	if (!ModEpoll(client.GetConnFd(), EPOLLOUT))
		return false;
	// write the response to connFd
	if (send(client.GetConnFd(), response.c_str(), response.size(), 0) == -1)
	{
		Log::LogMsg(ERROR, "send() failed");
		close(client.GetConnFd());
		return false;
	}
	// clear the client request history
	client.Reset();
	// set the events to EPOLLIN
	if (!ModEpoll(client.GetConnFd(), EPOLLIN))
		return false;
	return true;
}

bool	Server::ModEpoll(int connFd, uint32_t events)
{
	struct epoll_event event;
	event.data.fd = connFd;
	event.events = events;
	if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, connFd, &event) == -1)
	{
		Log::LogMsg(ERROR, "epoll_ctl() failed");
		close(connFd);
		return false;
	}
	return true;
}

uint16_t	Server::GetPort() const
{
	return _port;
}

in_addr_t	Server::GetHost() const
{
	return _host;
}

std::string	Server::GetHostStr() const
{
	return inet_ntoa(*((struct in_addr*)&_host));
}

std::string	Server::GetServerName() const
{
	return _serverName;
}

std::string	Server::GetRoot() const
{
	return _root;
}

std::vector<std::string>	Server::GetIndexes() const
{
	return _indexes;
}

bool	Server::GetAutoIndex() const
{
	return _autoIndex;
}

std::vector<HttpMethod>	Server::GetMethods() const
{
	return _methods;
}

std::vector<Location>	Server::GetLocations() const
{
	return _locations;
}

unsigned int	Server::GetMaxBodySize() const
{
	return _maxBodySize;
}

std::map<int, std::string>	Server::GetErrorPages() const
{
	return _errorPages;
}

std::string	Server::GetUploadPath() const
{
	return _uploadPath;
}

std::string	Server::GetBinPaths() const
{
	return _binPaths;
}

std::string	Server::GetCgiExtensions() const
{
	return _cgiExtensions;
}

int	Server::GetListenFd() const
{
	return _listenFd;
}

struct sockaddr_in	Server::GetSockAddr() const
{
	return _sockAddr;
}

int	Server::GetEpollFd() const
{
	return _epollFd;
}

int	Server::GetConnFd() const
{
	return _connFd;
}

std::ostream	&operator<<(std::ostream &o, Server const &i)
{
	o << "----[ server info ]----" << std::endl;
	o << "Port: " << i.GetPort() << std::endl;
	o << "Host: " << i.GetHostStr() << std::endl;
	o << "ServerName: " << i.GetServerName() << std::endl;
	o << "Root: " << i.GetRoot() << std::endl;
	o << "Indexes: ";

	const std::vector<std::string>& indexes = i.GetIndexes();
	for (std::vector<std::string>::const_iterator it = indexes.begin(); it != indexes.end(); it++)
		o << *it << " ";
	o << std::endl;
	o << "AutoIndex: " << i.GetAutoIndex() << std::endl;

	const std::vector<HttpMethod>& methods = i.GetMethods();
	o << "Methods: ";
	for (std::vector<HttpMethod>::const_iterator it = methods.begin(); it != methods.end(); it++)
		o << *it << " ";
	o << std::endl;
	o << "MaxBodySize: " << i.GetMaxBodySize() << std::endl;

	const std::map<int, std::string>& errorPages = i.GetErrorPages();
	o << "ErrorPages: ";
	for (std::map<int, std::string>::const_iterator it = errorPages.begin(); it != errorPages.end(); it++)
		o << it->first << " " << it->second << " ";
	o << std::endl;
	o << "UploadPath: " << i.GetUploadPath() << std::endl;
	o << "ListenFd: " << i.GetListenFd() << std::endl;
	o << "ConnFd: " << i.GetConnFd() << std::endl;
	o << "-----------------------" << std::endl;
	return o;
}
