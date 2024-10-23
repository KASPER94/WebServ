/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: peanut <peanut@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 19:30:34 by peanut            #+#    #+#             */
/*   Updated: 2024/10/23 16:58:17 by peanut           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "conf.hpp"
#include "webserv.h"
#include <iostream>

bool webserv(char *config_file) {
    env()->webserv = new Webserv();
    try {
        conf config(config_file);
    }
    catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
    return (true);
}

int main(int ac, char **av) {
    if (ac == 1) {
        webserv((char *)"./config/default.conf");
    }
    else if (ac == 2) {
        webserv(av[1]);
    }
    else {
        std::cerr << "\033[31m" << "to use Webserv : ./webserv [configuration file]" << "\033[0m" << std::endl;
        return (1);
    }
    return (0);
}