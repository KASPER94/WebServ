/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrigny <yrigny@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 17:27:11 by yrigny            #+#    #+#             */
/*   Updated: 2024/11/21 12:29:56 by yrigny           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Conf.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "Epoller.hpp"
#include <stdexcept>
using namespace std;

int main(int ac, char **av)
{
	string path = "default.conf";
	if (ac == 2)
		path = av[1];
	else if (ac > 2)
	{
		cerr << "Usage: ./webserv [config_file]" << endl;
		exit(1);
	}
	try
	{
		Conf	parsedConf(path);
		vector<ServerInfo> serverInfos = parsedConf.GetServerInfos();
		VecServer cluster(0);
		Server server(serverInfos[0]);
		cluster.push_back(server);
		for (size_t i = 1; i < serverInfos.size(); i++)
		{
			server = Server(serverInfos[i]);
			cluster.push_back(server);
		}
		Epoller	epoller(cluster);
		epoller.InitEpoller();
	}
	catch (std::exception &e)
	{
		cerr << "Error: " << e.what() << endl;
		exit(1);
	}
	return 0;
}
