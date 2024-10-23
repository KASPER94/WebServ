/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: peanut <peanut@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 13:40:34 by peanut            #+#    #+#             */
/*   Updated: 2024/10/23 16:53:08 by peanut           ###   ########.fr       */
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
# include "Webserv.hpp"
class Webserv;
class Server;

typedef struct s_env {
	Webserv	*webserv;
}	t_env;

t_env *env();
std::vector<std::string> split_trim_conf(std::string str) ;

#endif