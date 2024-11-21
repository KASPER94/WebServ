/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrigny <yrigny@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 18:46:03 by yrigny            #+#    #+#             */
/*   Updated: 2024/11/21 11:46:32 by yrigny           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() : _server(NULL), _connFd(-1), _requestStr("")
{
}

Client::Client(Server* server, int connFd) : 
	_server(server), _connFd(connFd), _requestStr("")
{
}

Client::Client(const Client& src) : 
	_server(src._server), _connFd(src._connFd), _requestStr(src._requestStr)
{
}

Client::~Client()
{
}

Client&	Client::operator=(const Client& rhs)
{
	_server = rhs._server;
	_connFd = rhs._connFd;
	_requestStr = rhs._requestStr;
	return *this;
}

ReqStatus	Client::RecvRequest(string request)
{
	_requestStr += request;
	if (!IsCompleteRequest())
		return REQ_INCOMPLETE;
	if (BodyTooLarge())
		return REQ_BODY_TOO_LARGE;
	return REQ_COMPLETE;
}

bool	Client::IsCompleteRequest()
{
	// for GET & DELETE requests, the end is marked with \r\n\r\n
	if (_requestStr.find("GET") == 0 || _requestStr.find("DELETE") == 0)
	{
		if (_requestStr.find("\r\n\r\n") != std::string::npos)
			return true;
	}
	// for POST requests, the body size is given in the header
	if (_requestStr.find("POST") == 0)
	{
		std::string header = _requestStr.substr(0, _requestStr.find("\r\n\r\n"));
		std::string body = _requestStr.substr(_requestStr.find("\r\n\r\n") + 4);
		// if the header contains Content-Length
		if (header.find("Content-Length") != std::string::npos)
		{
			std::string contentLength = header.substr(header.find("Content-Length") + 16);
			contentLength = contentLength.substr(0, contentLength.find("\r\n"));
			if (body.size() == (size_t)strtol(contentLength.c_str(), NULL, 10))
				return true;
		}
		// if the header contains Transfer-Encoding: chunked
		if (header.find("Transfer-Encoding: chunked") != std::string::npos)
		{
			if (body.find("0\r\n\r\n") != std::string::npos)
				return true;
		}
	}
	return false;
}

bool	Client::BodyTooLarge()
{
	std::string header = _requestStr.substr(0, _requestStr.find("\r\n\r\n"));
	std::string body = _requestStr.substr(_requestStr.find("\r\n\r\n") + 4);
	if (body.size() > _server->GetMaxBodySize())
		return true;
	return false;
}

int	Client::GetConnFd() const
{
	return _connFd;
}

string&	Client::GetRequestStr()
{
	return _requestStr;
}
