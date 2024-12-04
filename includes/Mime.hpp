/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mime.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skapersk <skapersk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 13:24:08 by skapersk          #+#    #+#             */
/*   Updated: 2024/11/22 13:52:52 by skapersk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef MIME_HPP
# define MIME_HPP

# include <string>
# include <map>
# include "webserv.h"

class Mime {
	private:
		static std::map<std::string, std::string> mimeTypes;

	public:
		Mime();
		Mime(const Mime &cpy);
		~Mime();

		Mime &operator=(const Mime &rhs);

		static std::string getMimeType(const std::string &extension);
		void addMimeType(const std::string &extension, const std::string &type);
};

#endif
