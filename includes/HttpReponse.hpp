/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpReponse.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 14:52:02 by skapersk          #+#    #+#             */
/*   Updated: 2024/11/14 11:36:21 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include <iostream>
# include "webserv.h"

class HttpReponse {
	private:
		Client		*client;

		HttpRequest	*getRequest();
	public:
		HttpReponse(Client *client);
		HttpReponse(const HttpReponse &cpy);
		~HttpReponse();
		HttpReponse &operator=(const HttpResponse &rhs);

		void	sendResponse();
}

#endif
