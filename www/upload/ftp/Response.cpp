/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 18:19:08 by yrigny            #+#    #+#             */
/*   Updated: 2024/12/03 17:18:33 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

map<string, string> Response::statusCodes;
void Response::InitializeStatusCodes() {
    statusCodes["200"] = "OK";
    statusCodes["204"] = "No Content";
    statusCodes["400"] = "Bad Request";
    statusCodes["404"] = "Not Found";
    statusCodes["405"] = "Method Not Allowed";
    statusCodes["413"] = "Payload Too Large";
    statusCodes["500"] = "Internal Server Error";
    statusCodes["501"] = "Not Implemented";
    statusCodes["505"] = "HTTP Version Not Supported";
}

Response::Response() :
	_statusCode(""),
	_statusMessage(""),
	_contentType(""),
	_serverName(""),
	_contentLen(""),
	_connection(""),
	_body(""),
	_response("")
{
	InitializeStatusCodes();
}

Response::~Response()
{
}

void	Response::SetStatusCode(const string& code)
{
	_statusCode = code;
}

void	Response::SetStatusMessage()
{
	_statusMessage = statusCodes[_statusCode];
}

void	Response::SetServerName(const string& serverName)
{
	_serverName = serverName;
}

void	Response::SetContentType(const string& path)
{
	cout << "static file path: " << path << endl;
	if (path.find(".html") != string::npos || path.find(".htm") != string::npos)
		_contentType = "text/html";
	if (path.find(".jpg") != string::npos)
		_contentType = "image/jpeg";
	if (path.find(".png") != string::npos)
		_contentType = "image/png";
	if (path.find(".gif") != string::npos)
		_contentType = "image/gif";
	if (path.find(".css") != string::npos)
		_contentType = "text/css";
	if (path.find(".js") != string::npos)
		_contentType = "text/javascript";
}

void	Response::SetConnection(const string& connection)
{
	_connection = connection;
}

void	Response::SetContentLen(size_t len)
{
	std::ostringstream oss;
	oss << len;
	_contentLen = oss.str();
}

void	Response::SetBody(const string& body)
{
	_body = body;
}

void	Response::SetResponse(const string& response)
{
	_response = response;
}

void	Response::ResembleResponse()
{
	_response = "HTTP/1.1 " + _statusCode + " " + _statusMessage + "\r\n";
	_response += "Content-Type: " + _contentType + "\r\n";
	_response += "Server: " + _serverName + "\r\n";
	_response += "Connection: " + _connection + "\r\n";
	_response += "Content-Length: " + _contentLen + "\r\n";
	_response += "\r\n";
	_response += _body;
	// cout << "----[ response ]----" << endl;
	// cout << _response << endl;
	// cout << "--------------------" << endl;
}

string	Response::GetStatusCode() const
{
	return _statusCode;
}

string	Response::GetContentType() const
{
	return _contentType;
}

string	Response::GetBody() const
{
	return _body;
}

string&	Response::GetResponse()
{
	return _response;
}

