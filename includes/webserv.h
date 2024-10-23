/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: peanut <peanut@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 13:40:34 by peanut            #+#    #+#             */
/*   Updated: 2024/10/23 14:50:30 by peanut           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_H
# define WEBSERV_H

# include <fstream>
# include <sstream>

enum HttpMethod {
	GET,
	POST,
	DELETE,
	OTHER
};

# include "Server.hpp"
class Server;

typedef struct s_env {
	Server	*server;
}	t_env;

std::vector<std::string> split_trim_conf(std::string str) ;

#endif