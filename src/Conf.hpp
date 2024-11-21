/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Conf.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrigny <yrigny@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 12:26:27 by yrigny            #+#    #+#             */
/*   Updated: 2024/11/18 19:43:06 by yrigny           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONF_HPP
# define CONF_HPP
# include "Log.hpp"
# include <string>
# include <vector>
using namespace std;

enum HttpMethod
{
	GET,
	POST,
	DELETE,
};

typedef struct s_location
{
	std::string 				_path;
	std::string 				_root;
	std::string 				_index;
	bool 						_autoIndex;
	std::vector<std::string>	_cgiExtension;
	std::vector<std::string>	_cgiBin;
	std::vector<HttpMethod>		_methods;
	std::string 				_redir;
}	Location;

typedef struct s_server
{
	string				listen;
	string				serverName;
	string				host;
	string				clientMaxBodySize;
	string				root;
	string				errorPages;
	string				indexes;
	string				autoIndex;
	string				methods;
	string				uploadDir;
	string				cgiBin;
	string				cgiExtension;
	vector<Location>	locations;
}	ServerInfo;

class Conf
{
	public:
		Conf(const string path);
		~Conf();
	
		vector<ServerInfo>	GetServerInfos();
		size_t				GetNbServers();

	private:
		vector<ServerInfo>	_serverInfos;
		int					_nbServers;
				
		void				_ParseServers(ifstream &file);
		bool				_FindServerBlockStart(string line);
		bool				_FindServerBlockEnd(string line);
		bool				_FindLocationBlockStart(string line);
		bool				_FindLocationBlockEnd(string line);
		void				_ParseLocationBlock(ifstream &file, ServerInfo *serverInfo);
		void				_ParseLine(string line, ServerInfo *serverInfo);
		void				_ParseLocationLine(string line, Location *location);
		bool				_SplitToKeyValuePair(string line, string &key, string &value);
};

#endif
