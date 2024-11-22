/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Conf.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrigny <yrigny@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 13:36:33 by yrigny            #+#    #+#             */
/*   Updated: 2024/11/21 12:32:01 by yrigny           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Conf.hpp"
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>

bool	FileExists(const string path)
{
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0);
}

bool	IsDirectory(const string path)
{
	struct stat statbuf;
	if (stat(path.c_str(), &statbuf) != 0)
		return false;
	return S_ISDIR(statbuf.st_mode);
}

bool	IsConfFile(const string path)
{
	return path.substr(path.find_last_of(".") + 1) == "conf";
}

bool	IsReadable(const string path)
{
	return access(path.c_str(), R_OK) == 0;
}

Conf::Conf(const string path) : _serverInfos(), _nbServers(0)
{
	if (path.empty())
		throw std::invalid_argument("Config path is empty");
	if (!FileExists(path))
		throw std::invalid_argument("Config path does not exist");
	if (IsDirectory(path))
		throw std::invalid_argument("Config path is a directory");
	if (!IsConfFile(path))
		throw std::invalid_argument("Config path is not a .conf file");
	if (!IsReadable(path))
		throw std::invalid_argument("Config file is not readable");
	ifstream file(path.c_str());
	if (!file.is_open())
		throw std::invalid_argument("Config file could not be opened");
	this->_ParseServers(file);
}

Conf::~Conf()
{
}

void	Conf::_ParseServers(ifstream &file)
{
	string line;
	int openBracket = 0;

	while (getline(file, line))
	{
		if (line.empty() || line[0] == '#' || line[0] == ';')
            continue;
		if (_FindServerBlockStart(line))
		{
			openBracket = 1;
			ServerInfo serverInfo;
			// parse the server block
			while (getline(file, line))
			{
				if (line.empty() || line[0] == '#' || line[0] == ';')
					continue;
				if (_FindServerBlockEnd(line))
				{
					openBracket = 0;
					_serverInfos.push_back(serverInfo);
					_nbServers++;
					break;
				}
				if (_FindLocationBlockStart(line))
					_ParseLocationBlock(file, &serverInfo);
				else
					_ParseLine(line, &serverInfo);
			}
			if (openBracket == 1)
				throw std::invalid_argument("Invalid server block");
		}
	}
	if (_nbServers == 0)
		throw std::invalid_argument("No valid server blocks found");
}

bool	Conf::_FindServerBlockStart(string line)
{
	string::size_type pos = line.find("server");
	if (pos != string::npos)
	{
		// check if there are no characters before the word "server"
		for (size_t i = 0; i < pos; i++)
		{
			if (line[i] != ' ' && line[i] != '\t')
				throw std::invalid_argument("Invalid server block");
		}
		// move the line to the right of the word "server"
		line = line.substr(pos + 6);
		pos = line.find("{");
		if (pos != string::npos)
		{
			// check if there are no characters before "{"
			for (size_t i = 0; i < pos; i++)
			{
				if (line[i] != ' ' && line[i] != '\t')
					throw std::invalid_argument("Invalid server block");
			}
			// check if there are no characters after "{"
			for (size_t i = pos; i < line.size() - 1; i++)
			{
				if (line[i] != ' ' && line[i] != '\t')
					throw std::invalid_argument("Invalid server block");
			}
			return true;
		}
	}
	throw std::invalid_argument("Invalid server block");
	return false;
}

bool	Conf::_FindServerBlockEnd(string line)
{
	string::size_type pos = line.find("}");
	if (pos != string::npos)
	{
		// check if there are no characters before "}"
		for (size_t i = 0; i < pos; i++)
		{
			if (line[i] != ' ' && line[i] != '\t')
				throw std::invalid_argument("Invalid server block");
		}
		// check if there are no characters after "}"
		for (size_t i = pos + 1; i < line.size() - 1; i++)
		{
			if (line[i] != ' ' && line[i] != '\t')
				throw std::invalid_argument("Invalid server block");
		}
		return true;
	}
	return false;
}

bool	Conf::_FindLocationBlockStart(string line)
{
	string::size_type pos = line.find("location");
	if (pos != string::npos)
	{
		// check if there are no characters before the word "location"
		for (size_t i = 0; i < pos; i++)
		{
			if (line[i] != ' ' && line[i] != '\t')
				throw std::invalid_argument("Invalid location block");
		}
		line = line.substr(pos + 8);
		pos = line.find("{");
		if (pos != string::npos)
		{
			// check if there are no characters after "{"
			for (size_t i = pos; i < line.size() - 1; i++)
			{
				if (line[i] != ' ' && line[i] != '\t')
					throw std::invalid_argument("Invalid location block");
			}
			return true;
		}
	}
	return false;
}

bool	Conf::_FindLocationBlockEnd(string line)
{
	string::size_type pos = line.find("}");
	if (pos != string::npos)
	{
		// check if there are no characters before "}"
		for (size_t i = 0; i < pos; i++)
		{
			if (line[i] != ' ' && line[i] != '\t')
				throw std::invalid_argument("Invalid location block");
		}
		// check if there are no characters after "}"
		for (size_t i = pos + 1; i < line.size() - 1; i++)
		{
			if (line[i] != ' ' && line[i] != '\t')
				throw std::invalid_argument("Invalid location block");
		}
		return true;
	}
	return false;
}

void	Conf::_ParseLocationBlock(ifstream &file, ServerInfo *serverInfo)
{
	string line;
	Location location;
	int openBracket = 1;

	while (getline(file, line))
	{
		if (line.empty() || line[0] == '#' || line[0] == ';')
			continue;
		if (_FindLocationBlockEnd(line))
		{
			openBracket = 0;
			serverInfo->locations.push_back(location);
			break;
		}
		_ParseLocationLine(line, &location);
	}
	if (openBracket == 1)
		throw std::invalid_argument("Invalid location block");
}

void	Conf::_ParseLine(string line, ServerInfo *serverInfo)
{
	string::size_type pos = line.find(";");
	if (pos == string::npos)
		throw std::invalid_argument("Invalid line");
	// remove the ";" from the line
	line = line.substr(0, pos);

	string key;
	string value;
	if (!_SplitToKeyValuePair(line, key, value))
		throw std::invalid_argument("Invalid line");
	if (key.compare("listen") == 0)
		serverInfo->listen = value;
	else if (key.compare("server_name") == 0)
		serverInfo->serverName = value;
	else if (key.compare("host") == 0)
		serverInfo->host = value;
	else if (key.compare("client_max_body_size") == 0)
		serverInfo->clientMaxBodySize = value;
	else if (key.compare("root") == 0)
		serverInfo->root = value;
	else if (key.compare("error_pages") == 0)
		serverInfo->errorPages = value;
	else if (key.compare("index") == 0)
		serverInfo->indexes = value;
	else if (key.compare("autoindex") == 0)
		serverInfo->autoIndex = value;
	else if (key.compare("upload_path") == 0)
		serverInfo->uploadDir = value;
	else if (key.compare("allowedMethods") == 0)
		serverInfo->methods = value;
	else if (key.compare("cgi_bin") == 0)
		serverInfo->cgiBin = value;
	else if (key.compare("cgi_extension") == 0)
		serverInfo->cgiExtension = value;
	else
		throw std::invalid_argument("Invalid line");
}

void	Conf::_ParseLocationLine(string line, Location *location)
{
	string::size_type pos = line.find(";");
	if (pos == string::npos)
		throw std::invalid_argument("Invalid line");
	(void)location;
}

bool	Conf::_SplitToKeyValuePair(string line, string& key, string& value)
{
	istringstream iss(line);
	string word;

	if (!(iss >> word))
		return false;
	key = word;
	if (!(iss >> word))
		return false;
	size_t pos = line.find(word);
	value = line.substr(pos);
	return true;
}

vector<ServerInfo>	Conf::GetServerInfos()
{
	return this->_serverInfos;
}

size_t	Conf::GetNbServers()
{
	return this->_nbServers;
}
