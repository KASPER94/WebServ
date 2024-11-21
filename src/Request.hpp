/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrigny <yrigny@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 13:56:39 by yrigny            #+#    #+#             */
/*   Updated: 2024/11/20 15:21:28 by yrigny           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "Server.hpp"
# include "Client.hpp"
using namespace std;

typedef struct s_header
{
	map<string, string>	_fields;
}	Header;

class Request
{
	private:
		// int		_fd;
		// string	_request;
		// Header	_header;
};

#endif
